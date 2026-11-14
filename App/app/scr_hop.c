/* Copyright 2026 AislanJefferson
 * https://github.com/AislanJefferson/KLEANFW
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

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

#define HOP_MAGIC      0x47u     /* word0 = 'G' << 8 | resync index */
#define HOP_CHECK_N    0x48505351u
#define HOP_SLOT_MS    500u
/* A burst (initial idx 0, then one every HOP_RESYNC_MS) ends its 8th byte at
 * t0 + idx * HOP_RESYNC_MS + HOP_POS_MS: mid-slot, away from hop boundaries.
 * The TX never moves t0, so a receiver that misses a resync stays aligned. */
#define HOP_RESYNC_MS  8000u
#define HOP_POS_MS     290u
/* RX keeps hopping through fades: stop only after this long without RX. */
#define HOP_HANG_MS    2000u
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
    /* 16 carriers in a 500 Hz window (REG_71: 1032 units = 100 Hz), chosen so
     * the inverted voice fits the TX audio band: N/N+ (LPF 2.5 kHz)
     * 2600..3100 Hz, Wide (LPF 3 kHz) 2900..3400 Hz. Seed bit 0 = Wide. */
    return (uint16_t)(0x68DCu + ((seed & 1u) ? 3096u : 0u) +
                      (CRYPTO_Word(seed, slot) & 15u) * 344u);
}

static uint32_t tx_wide(void)
{
    return gCurrentVfo->CHANNEL_BANDWIDTH == BANDWIDTH_WIDE;
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
                                                       : carrier(tx_wide(), 0xFFFFFFFFu));
}

static void start(uint32_t seed, uint32_t t0, bool rx)
{
    gScrHop.seed    = seed;
    gScrHop.t0      = t0;
    gScrHop.last_rx = SCHEDULER_GetMs();
    gScrHop.slot    = (uint16_t)(elapsed() / HOP_SLOT_MS);
    gScrHop.idx     = 0;
    gScrHop.rx      = rx;
    gScrHop.active  = 1;
    SCR_HOP_Apply(!rx);
}

static void wait_until(uint32_t ms)
{
    while ((int32_t)(elapsed() - ms) < 0) {}
}

void SCR_HOP_TxBurst(uint16_t idx)
{
    /* idx 0 starts a new over with a fresh seed; later ones resend it. */
    const uint32_t seed = idx ? gScrHop.seed
                              : (CRYPTO_Word(((uint32_t)BK4819_ReadRegister(BK4819_REG_67) << 16) ^
                                             SysTick->VAL, SCHEDULER_GetMs()) & ~1u) | tx_wide();
    const uint16_t word0 = (HOP_MAGIC << 8) | (idx & 0xFFu);
    const uint16_t words[4] = {
        word0, (uint16_t)seed, (uint16_t)(seed >> 16),
        (uint16_t)CRYPTO_Word(seed, HOP_CHECK_N ^ word0),
    };
    const uint16_t irq_mask = BK4819_ReadRegister(BK4819_REG_3F);

    if (!idx)
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

    /* Resync: start on the exact ms so the 8th byte ends at its anchor. A late
     * one is skipped: it would re-anchor every receiver to the wrong time. */
    bool send = true;
    if (idx) {
        const uint32_t target = idx * HOP_RESYNC_MS + HOP_POS_MS - HOP_ANCHOR_MS;
        wait_until(target);
        send = elapsed() - target <= 2u;
    }
    const uint32_t t_start = SCHEDULER_GetMs();
    if (send) {
        BK4819_WriteRegister(BK4819_REG_59, 0x28F8);   // start FSK TX
        /* Never stop before the whole burst is on air, whatever REG_0C says. */
        while (SCHEDULER_GetMs() - t_start < HOP_BURST_MS) {}
        for (uint8_t n = 0; n < 20u && !(BK4819_ReadRegister(BK4819_REG_0C) & 1u); n++)
            SYSTEM_DelayMs(5);
    }

    /* Back to voice without BK4819_ResetFSK(): it would idle REG_30 (TX off). */
    BK4819_WriteRegister(BK4819_REG_02, 0);
    BK4819_WriteRegister(BK4819_REG_59, 0x0068);
    BK4819_WriteRegister(BK4819_REG_70, 0);
    BK4819_WriteRegister(BK4819_REG_58, 0);
    BK4819_WriteRegister(BK4819_REG_3F, irq_mask);

    if (wide)
        BK4819_SetFilterBandwidth(BK4819_FILTER_BW_WIDE, false);

    if (idx)
        SCR_HOP_Apply(true);
    else
        start(seed, t_start + HOP_ANCHOR_MS - HOP_POS_MS, false);
    BK4819_ExitTxMute();
}

bool SCR_HOP_RxStart(const uint16_t *words, uint32_t stamp_ms)
{
    const uint32_t seed = (uint32_t)words[1] | ((uint32_t)words[2] << 16);

    if ((words[0] >> 8) != HOP_MAGIC ||
        words[3] != (uint16_t)CRYPTO_Word(seed, HOP_CHECK_N ^ words[0]))
        return false;
    start(seed, stamp_ms - (words[0] & 0xFFu) * HOP_RESYNC_MS - HOP_POS_MS, true);
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

void SCR_HOP_Stop(void)
{
    const bool rx_session = gScrHop.active && gScrHop.rx;

    gScrHop.active  = 0;
    gScrHop.rx_wait = 0;
    /* The RX VFO may already be a fixed-scrambler channel (channel change or
     * DW within the hang): restore its state instead of forcing REG_31<1> off. */
    if (rx_session)
        RADIO_ApplyScramble(gRxVfo, false);
}

bool SCR_HOP_VfoUsesHop(const VFO_Info_t *vfo)
{
    return vfo->SCRAMBLING_TYPE == SCRAMBLE_TYPE_HOP &&
           vfo->Modulation == MODULATION_FM && gSetting_ScrambleEnable;
}

void SCR_HOP_Tick(void)
{
    if (!gScrHop.active)
        return;

    if (gScrHop.rx) {
        /* Keep the slot clock through squelch fades; DW moving off ends it. */
        if (FUNCTION_IsRx())
            gScrHop.last_rx = SCHEDULER_GetMs();
        if (SCHEDULER_GetMs() - gScrHop.last_rx > HOP_HANG_MS ||
            !SCR_HOP_VfoUsesHop(gRxVfo)) {
            SCR_HOP_Stop();
            return;
        }
    } else if (gCurrentFunction != FUNCTION_TRANSMIT) {
        SCR_HOP_Stop();
        return;
    } else if (elapsed() >= (gScrHop.idx + 1u) * HOP_RESYNC_MS) {
        SCR_HOP_TxBurst(++gScrHop.idx);   // resync for late/faded receivers
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
