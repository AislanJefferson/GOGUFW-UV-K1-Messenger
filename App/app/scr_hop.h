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

/* KLEANFW "HOP" voice scrambler.
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

/* Byte fields first: Thumb-1 LDRB/STRB reach offsets 0..31 only. */
typedef struct {
    uint8_t  active;
    uint8_t  rx;        /* session started by a received burst       */
    uint8_t  rx_wait;   /* RX: sync seen, waiting for first FIFO     */
    uint8_t  stamp_ok;  /* stamp from the REG_0C spin (~1 ms), not a late sync */
    uint8_t  ref_idx;
    uint8_t  ref_ok;
    int16_t  adj;       /* RX: TX/RX clock ratio - 1, Q16 (0 on TX)  */
    uint16_t slot;
    uint16_t idx;       /* TX: last resync burst sent                */
    uint16_t reg48;     /* saved AF gain during the RX guard mute    */
    uint32_t seed;
    uint32_t anchor;    /* local ms: end of the 8th byte of the anchoring burst */
    uint32_t base;      /* TX slot-clock ms at anchor                */
    uint32_t stamp;     /* ms: RX time of the pending FSK IRQ        */
    uint32_t ref_stamp; /* RX: precise stamp the drift is measured from */
    uint32_t last_rx;   /* ms: last tick seen in RX (fade hang)      */
} SCR_HOP_State_t;

extern SCR_HOP_State_t gScrHop;

void SCR_HOP_Reset(void);
/* Program the scrambler for a HOP channel. TX without a session keeps a
 * fixed key-derived carrier, so a HOP TX is never sent in clear. */
void SCR_HOP_Apply(bool tx);
void SCR_HOP_TxBurst(uint16_t idx);
/* Validate the first 4 FIFO words of a received burst and start hopping.
 * precise: stamp_ms came from the REG_0C spin (used for drift tracking). */
bool SCR_HOP_RxStart(const uint16_t *words, uint32_t stamp_ms, bool precise);
void SCR_HOP_Stop(void);
void SCR_HOP_Tick(void);
/* HOP is active on this VFO: same rule as RADIO_ApplyScramble() (FM only,
 * ScraEn on). Every HOP path that bypasses RADIO_ApplyScramble() uses it. */
struct VFO_Info_t;
bool SCR_HOP_VfoUsesHop(const struct VFO_Info_t *vfo);
/* The RX listens to a HOP channel (either VFO with Dual Watch) and can decode
 * its seed burst: battery save must not sleep through that ~0.2 s burst. */
bool SCR_HOP_Watching(void);

#endif
#endif
