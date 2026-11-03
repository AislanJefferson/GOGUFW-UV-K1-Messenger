/* GOGUFW "HOP" voice scrambler.
 *
 * The BK4829 only offers spectrum inversion. HOP changes the inversion
 * carrier every HOP_SLOT_MS following XTEA(CryKey, seed || slot). The seed is
 * sent in a short FSK burst at the start of every PTT; both ends anchor the
 * slot clock to the end of the 8th burst byte, computed from the programmed
 * FSK timing (no bench calibration). Not encryption: it only defeats quick
 * decoding with a fixed-inversion scrambler. */
#ifndef APP_SCR_HOP_H
#define APP_SCR_HOP_H

#include <stdbool.h>
#include <stdint.h>

#include "helper/crypto.h"

#ifdef ENABLE_GOGUFW_CRYPTO

#define SCRAMBLE_TYPE_HOP 11u   /* index of "HOP" in gSubMenu_SCRAMBLER */

typedef struct {
    uint32_t seed;
    uint32_t t0;        /* ms: end of the 8th burst byte          */
    uint32_t stamp;     /* ms: RX time of the pending FSK IRQ     */
    uint16_t slot;
    uint16_t reg48;     /* saved AF gain during the RX guard mute */
    uint8_t  active;
    uint8_t  rx;        /* session started by a received burst    */
    uint8_t  rx_wait;   /* RX: sync seen, waiting for first FIFO  */
} SCR_HOP_State_t;

extern SCR_HOP_State_t gScrHop;

void SCR_HOP_Reset(void);
/* Program the scrambler for a HOP channel. TX without a session keeps a
 * fixed key-derived carrier, so a HOP TX is never sent in clear. */
void SCR_HOP_Apply(bool tx);
void SCR_HOP_TxBurst(void);
/* Validate the first 4 FIFO words of a received burst and start hopping. */
bool SCR_HOP_RxStart(const uint16_t *words, uint32_t stamp_ms);
void SCR_HOP_Stop(void);
void SCR_HOP_Tick(void);

#endif
#endif
