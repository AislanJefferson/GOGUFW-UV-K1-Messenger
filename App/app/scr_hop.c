#include <string.h>

#include "app/scr_hop.h"

#ifdef ENABLE_GOGUFW_CRYPTO

#include "driver/bk4819.h"
#include "driver/bk4819-regs.h"
#include "driver/system.h"
#include "functions.h"
#include "misc.h"
#include "radio.h"
#include "scheduler.h"

#define HOP_MAGIC      0x4847u   /* "GH" */
#define HOP_CHECK_N    0x48505351u
#define HOP_SLOT_MS    500u
/* RX guard mute around each slot boundary. Covers the chips' internal
 * latencies and one byte of REG_59 preamble uncertainty (6.7 ms). */
#define HOP_GUARD_MS   8u
/* REG_59 0x28F8: 16-byte preamble + 4-byte sync, then the 8 data bytes the
 * RX FIFO (threshold 4 words, REG_5E 0x3204) reports as "almost full".
 * 1200 bps -> 20/3 ms per byte. */
#define HOP_ANCHOR_MS  (((16u + 4u + 8u) * 20u + 1u) / 3u)
/* Whole burst on air: preamble + sync + 8 data + 2 CRC bytes. */
#define HOP_BURST_MS   (((16u + 4u + 8u + 2u) * 20u + 2u) / 3u)

SCR_HOP_State_t gScrHop __attribute__((section(".noncacheable")));

void SCR_HOP_Reset(void)
{
    memset(&gScrHop, 0, sizeof(gScrHop));
}

static uint16_t carrier(uint32_t seed, uint32_t slot)
{
    /* 16 inversion carriers, 2600..3500 Hz (EnableScramble steps 1032/100 Hz). */
    return (uint16_t)(0x68DCu + (CRYPTO_Word(seed, slot) & 15u) * 619u);
}

static uint32_t elapsed(void)
{
    return SCHEDULER_GetMs() - gScrHop.t0;
}

void SCR_HOP_Apply(bool tx)
{
    if (!gScrHop.active && !tx) {
        BK4819_DisableScramble();
        return;
    }
    BK4819_EnableScramble(0);
    BK4819_WriteRegister(BK4819_REG_71, gScrHop.active ? carrier(gScrHop.seed, gScrHop.slot)
                                                       : carrier(0, 0xFFFFFFFFu));
}

static void start(uint32_t seed, uint32_t t0, bool rx)
{
    gScrHop.seed   = seed;
    gScrHop.t0     = t0;
    gScrHop.slot   = 0;
    gScrHop.rx     = rx;
    gScrHop.active = 1;
    SCR_HOP_Apply(!rx);
}

void SCR_HOP_TxBurst(void)
{
    const uint32_t seed = CRYPTO_Word(((uint32_t)BK4819_ReadRegister(BK4819_REG_67) << 16) ^
                                      SysTick->VAL, SCHEDULER_GetMs());
    const uint16_t words[4] = {
        HOP_MAGIC, (uint16_t)seed, (uint16_t)(seed >> 16),
        (uint16_t)CRYPTO_Word(seed, HOP_CHECK_N),
    };
    const uint16_t irq_mask = BK4819_ReadRegister(BK4819_REG_3F);

    gScrHop.active = 0;
    BK4819_EnterTxMute();
    BK4819_DisableScramble();
    /* Same narrow-lock as Messenger FSK TX: a Wide channel sends the burst
     * with Narrow (N+: Narrower) deviation, the proven decode path for
     * Wide, Narrow and Narrower receivers alike. */
    const bool wide = gCurrentVfo->CHANNEL_BANDWIDTH == BANDWIDTH_WIDE;
    if (wide) {
        BK4819_FilterBandwidth_t bw = BK4819_FILTER_BW_NARROW;
#ifdef ENABLE_FEAT_F4HWN_NARROWER
        if (gSetting_set_nfm == 1)
            bw = BK4819_FILTER_BW_NARROWER;
#endif
        BK4819_SetFilterBandwidth(bw, false);
    }
    BK4819_SetupAircopy();
    SYSTEM_DelayMs(30);                            // same settle as Messenger TX
    BK4819_WriteRegister(BK4819_REG_5D, (sizeof(words) - 1u) << 8);
    BK4819_WriteRegister(BK4819_REG_3F, BK4819_REG_3F_FSK_TX_FINISHED);
    BK4819_WriteRegister(BK4819_REG_02, 0);
    BK4819_WriteRegister(BK4819_REG_59, 0x80F8);   // clear TX FIFO, 16-byte preamble
    BK4819_WriteRegister(BK4819_REG_59, 0x00F8);
    for (uint8_t i = 0; i < 4u; i++)
        BK4819_WriteRegister(BK4819_REG_5F, words[i]);
    SYSTEM_DelayMs(20);

    BK4819_WriteRegister(BK4819_REG_59, 0x28F8);   // start FSK TX
    const uint32_t t_start = SCHEDULER_GetMs();

    /* Never stop before the whole burst is on air, whatever REG_0C says. */
    while (SCHEDULER_GetMs() - t_start < HOP_BURST_MS) {}
    for (uint8_t n = 0; n < 20u && !(BK4819_ReadRegister(BK4819_REG_0C) & 1u); n++)
        SYSTEM_DelayMs(5);

    /* Back to voice without BK4819_ResetFSK(): it would idle REG_30 (TX off). */
    BK4819_WriteRegister(BK4819_REG_02, 0);
    BK4819_WriteRegister(BK4819_REG_59, 0x0068);
    BK4819_WriteRegister(BK4819_REG_70, 0);
    BK4819_WriteRegister(BK4819_REG_58, 0);
    BK4819_WriteRegister(BK4819_REG_3F, irq_mask);

    if (wide)
        BK4819_SetFilterBandwidth(BK4819_FILTER_BW_WIDE, false);

    start(seed, t_start + HOP_ANCHOR_MS, false);
    BK4819_ExitTxMute();
}

bool SCR_HOP_RxStart(const uint16_t *words, uint32_t stamp_ms)
{
    const uint32_t seed = (uint32_t)words[1] | ((uint32_t)words[2] << 16);

    if (words[0] != HOP_MAGIC || words[3] != (uint16_t)CRYPTO_Word(seed, HOP_CHECK_N))
        return false;
    start(seed, stamp_ms, true);
    return true;
}

static void guard_mute(bool mute)
{
    if (mute) {
        gScrHop.reg48 = BK4819_ReadRegister(BK4819_REG_48);
        BK4819_WriteRegister(BK4819_REG_48, gScrHop.reg48 & ~(0x3Fu << 4));
    } else {
        BK4819_WriteRegister(BK4819_REG_48, gScrHop.reg48);
    }
}

static void wait_until(uint32_t ms)
{
    while ((int32_t)(elapsed() - ms) < 0) {}
}

void SCR_HOP_Stop(void)
{
    if (gScrHop.active && gScrHop.rx)
        BK4819_DisableScramble();
    gScrHop.active  = 0;
    gScrHop.rx_wait = 0;
}

void SCR_HOP_Tick(void)
{
    if (!gScrHop.active)
        return;

    /* RX: the burst can arrive before the squelch state machine reports RX. */
    if (gScrHop.rx ? ((!FUNCTION_IsRx() && elapsed() > 200u) ||
                      gRxVfo->SCRAMBLING_TYPE != SCRAMBLE_TYPE_HOP)   // DW moved on
                   : gCurrentFunction != FUNCTION_TRANSMIT) {
        SCR_HOP_Stop();
        return;
    }

    const uint32_t boundary = (gScrHop.slot + 1u) * HOP_SLOT_MS;
    if ((int32_t)(boundary - elapsed()) > (int32_t)(10u + HOP_GUARD_MS))
        return;

    /* The boundary falls inside the next 10 ms tick: switch on the exact ms. */
    if (gScrHop.rx) {
        wait_until(boundary - HOP_GUARD_MS);
        guard_mute(true);
    }
    wait_until(boundary);
    gScrHop.slot = (uint16_t)(elapsed() / HOP_SLOT_MS);
    /* Scrambler off means DTMF/beep owns REG_71 (tone generator): leave it. */
    if (BK4819_ReadRegister(BK4819_REG_31) & (1u << 1))
        BK4819_WriteRegister(BK4819_REG_71, carrier(gScrHop.seed, gScrHop.slot));
    if (gScrHop.rx) {
        wait_until(boundary + HOP_GUARD_MS);
        guard_mute(false);
    }
}

#endif
