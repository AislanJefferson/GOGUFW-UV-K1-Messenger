# Changelog

KLEANFW is a modified version of GOGUFW by Gogu-Qs. The KLEANFW releases come
first. The GOGUFW history (upstream, by Gogu-Qs) is kept unchanged below them.

## Unreleased

Image 120,452 / 120,832 bytes (380 bytes free); RAM 15,608 / 16,384 bytes at
link time.

### Fixed

- HOP: a channel with a fixed scrambler (2600–3500 Hz) could receive without
  descrambling. When a HOP reception ended (2 s hang) after a channel change
  or a Dual Watch switch to that channel, the end of the HOP session turned
  the scrambler off (REG_31<1>) over the fixed one, and the first reception
  on it came out inverted. The end of a HOP RX session now reapplies the
  scrambler of the channel being received (+16 B).
- Messenger: after every HOP seed burst it received, the Messenger stayed
  "busy" for 1.2 s (SEND, ACK and PONG deferred, battery save held, the
  squelch-close fallback off) and then reset the FSK RX FIFO late. The
  accepted burst now clears that timer (+8 B).
- Spectrum (F+5): the sweep filter (REG_43) was cut to 8 bits, inherited from
  F4HWN. Steps of 5–12.5 kHz swept with a 2 kHz RF filter and steps of
  15 kHz and up (25 kHz is the default) with 4 kHz instead of 7 kHz, so a
  signal off the step centre could be weak or missing in the graph. The
  full value from the step table is now used (steps under 5 kHz unchanged;
  -4 B). The noise floor on 25 kHz steps reads ~2 dB higher: AUTO
  recalibrates, but a manual trigger and the fixed MR trigger (-110 dBm) stay
  where they were, ~2 dB closer to the noise.
- Spectrum (F+5) listened through the voice scrambler of the channel it was
  opened from: with a fixed scrambler (or a live HOP session) every FM signal
  found came out inverted. Spectrum now listens in clear and the channel gets
  its scrambler back on exit (+4 B).

- Spectrum (F+5) in MR mode with the scan priority on (`ScPri`, `+`) left the
  2 priority channels out of a `01`–`24` list. A list made of them swept
  nothing and Spectrum fell back to `ALL`, and key 4 only alternated `ALL`
  and `OFF`. Spectrum has no priority watch, so the priority channels are
  now swept with their list. The memory scan is unchanged.

### Changed

- HOP RX follows the transmitter's clock rate. From the first 8 s resync of an
  over (the second if the opening burst was stamped late), the receiver compares the resync interval with its own clock and
  scales the slot clock by the ratio, so a clock difference between radios no
  longer drifts past the 8 ms guard before the next resync. Only resyncs
  stamped in the REG_0C spin (~1 ms) are used. Under 12 ms of difference
  nothing changes, so radios without drift behave exactly as before. A host
  simulation gives at most 7–8 ms of slot error for 0.1–3 % of drift (before:
  13–238 ms). The correction stops growing after ~8.7 min without a resync
  (a carrier can keep the session alive), so it cannot overflow. RX only; TX
  and the over-the-air format are unchanged (+156 B).
- HOP and battery save: while the RX (or either VFO with Dual Watch) listens
  to a HOP channel that can decode the seed burst (MsgRx ON, VOX OFF),
  `BatSav` does not sleep, as with `BatSav` OFF, also while `SetOff` has
  turned the screen off. A sleeping receiver (1:4 is ~400 ms off per 500 ms)
  could miss the ~0.2 s burst and stay scrambled until the 8 s resync
  (+76 B).
- Spectrum (F+5) in MR mode sweeps the scan list of the channel on screen
  (`ChList`) instead of the active memory scan list (`ScList`). `01`–`24`
  sweeps that list and `ALL` sweeps the channels that are in any list, both
  with the memory scan rules (excluded channels skipped; priority channels
  included). `OFF` sweeps every
  saved channel, with or without a list, excluded and priority channels
  included; the label shows `OFF <n>`. A list with nothing to sweep falls
  back to `ALL`, then to every channel.
- Spectrum key 4 (MR) is local to Spectrum and cycles `ALL`, then each list
  that has channels to sweep (`01`, `02`, ...), then `OFF` (every channel),
  then `ALL` again. `ScList` is no longer changed.
- Image size, no behaviour change: the libc start files and the
  static-constructor call are gone (C-only firmware, -260 bytes, -32 bytes of
  RAM) and the embedded printf no longer carries the `+` and `#` flags, the
  length modifiers and the `%i`, `%x`, `%b` and `%p` conversions, which no
  format in the firmware uses (-408 bytes). The Spectrum, BK4829 register
  access and SysTick code and the multiboot RAM stub are byte-identical.

### Known issues and proposals

From a review of HOP, Messenger and Spectrum against the BK4829 register
table (register use matches it). None of these is changed without a radio
A/B test:

- HOP RX guard: zeroing REG_48<9:4> (AF Rx Gain2) is about 30 dB of
  attenuation, not a mute (the table's range is -28 to +3.5 dB). The
  Messenger FSK audio mute uses the same method. A full mute would also need
  REG_48<3:0> = 0 or REG_47<11:8> = 0, if the slot-boundary click is audible.
- HOP slot clock: the drift tracking above starts at the first resync, so
  the first 8 s of an over still need about 0.1 % relative clock accuracy
  between radios. The MCU clock is set by the bootloader and its source is
  not known here.
- Messenger TX keeps the channel's voice scrambler as set (fixed, or HOP with
  a live RX session), while the HOP seed burst turns it off. The table does
  not say whether REG_31<1> affects the FSK modem; both ends use the same
  setting today, so changing one side alone could break what works.

## KLEANFW 0.1.0 — 2026-11-10

First KLEANFW release, based on GOGUFW 3.6.2. Firmware identity
`KLEANFW v0.1.0` (SystemInfo, multiboot; the USB host identity stays
`v7 GGFW v6.3.0`), multiboot image `f4hwn.kleanfw.v0.1.0.bin`, CHIRP module
`KLEANFW_0.1.0_chirp_module.py`. Image 120,772 / 120,832 bytes (60 bytes
free) including the 920-byte multiboot load image; RAM 15,640 / 16,384 bytes
at link time.

### Upgrade notes

- The first boot runs the version-change reset: KEY_LOCK, MENU_LOCK, SET_KEY
  and SET_INV are cleared, `SetRxG` is turned back ON, invalid boot-logo lines
  and erased dBm corrections are repaired (one write to the settings sector).
- Use `KLEANFW_0.1.0_chirp_module.py`. The CHIRP model is now
  **UV-K1 / UV-K5 V3 KLEANFW**; GOGUFW modules are not compatible.
- Reinstall `FMVFO.app`, `FMMemory.app` and `Search.app` from this release if
  they come from a GOGUFW build older than 3.6.0.
- A CryKey set with the `Crypto.app` of a development build is not read:
  enter it again in **MENU → Msg → CryKey**.

### Added

- HOP voice scrambler (scramble type 11): each PTT sends an 8-byte FSK seed
  burst (~0.25 s), then the inversion carrier changes every 500 ms over 16
  carriers following XTEA(CryKey, seed ‖ slot). Carrier windows per bandwidth
  (N/N+ 2600–3100 Hz, Wide 2900–3400 Hz), a resync burst every 8 s, fade
  tolerance (the RX session ends 2 s after the last signal) and a ±8 ms guard
  mute at each hop. A HOP TX without a session uses a fixed key-derived
  carrier, so it is never sent in clear. Not encryption.
- CryKey (**MENU → Msg → CryKey**, up to 10 characters, also in CHIRP): with
  a passphrase, Messenger TEXT packets are sent as type 6, XTEA-CTR encrypted
  with a random salt and a 16-bit key check. No MAC. Stored at 0x012140 per
  multiboot settings bank, written only when it changes.
- SCRAMBLE side-key action: toggles the channel scrambler and restores its
  last type (HOP by default).
- F4HWN menu categories (`ENABLE_FEAT_F4HWN_MENU_CAT`) with an `Msg` category;
  SetRxG under Audio. The status bar shows `MENU` or the open category.
- Spectrum MR mode sweeps the active scan list (key 4 moves to the next list,
  the label shows the list and count, bottom labels show the position inside
  the list) and shows the channel frequency, modulation and listen bandwidth
  like VFO.
- Spectrum key 6 (25k / 12.5k / 6.25k) works in MR as in VFO and persists for
  every channel until EXIT or a long MENU.
- Spectrum backlight follows `BLTime`; SIDE2 runs the F+8 cycle (always on →
  always off → `BLTime`) with the F+8 bulb in the status bar.
- `build-all.sh` builds the firmware, every overlay app and checks the CHIRP
  module, collecting everything in `build/dist/`.
- MANUAL.md: complete user manual in Portuguese, with every menu item.

### Changed

- SPI flash wear: the PY25Q16 driver defers sector erases and commits a
  changed sector once (2 s after the last write while idle, on another sector,
  or before reset, bank switch, overlay app, Spectrum or multiboot work).
  Messenger IDs are reserved in blocks of 64. Mute no longer writes volume 0
  to the calibration sector. Leaving the menu commits pending writes on the
  next idle tick.
- F Lock offers only BRASIL (Anatel Ato 14448 general-use channels,
  462.5625–462.7125 and 467.5625–467.7125 MHz, default), DISABLE ALL and
  UNLOCK ALL. Other stored plans load as BRASIL.
- Fresh channels and VFOs default to Narrow (12.5 kHz); a factory reset turns
  SetNFM off.
- Typing a VFO frequency starts a clean simplex setting (tones, offset and
  reverse cleared); EXIT restores and re-saves the previous VFO.
- Spectrum PTT×2: a peak that matches a saved MR channel opens it in MR mode;
  any other peak lands in the VFO like a typed frequency.
- Spectrum listen bandwidth starts from the VFO/channel mode and uses the VFO
  REG_43 filters. Spectrum UI follows the GOGUFW HOP branch (own battery icon,
  no forced `M---` refresh) with 16..2048 VFO samples and band-edge clamping.
- The fixed and HOP scramblers are FM-only on TX and RX (menu preview, icon
  and the SCRAMBLE key included).
- Boot always behaves as `POnMsg = NONE`; the option was removed.
- CHIRP module renamed to `KLEANFW_0.1.0_chirp_module.py` with the CryKey,
  HOP and three-plan F Lock fields; older modules were removed.
- Name, version and build artifacts: `KLEANFW v0.1.0`, edition `KLFW`,
  `kleanfw.bin`, multiboot `f4hwn.kleanfw.v0.1.0.bin`, the `KLEANFW APPS`
  header and the `KLEANFW MULTIBOOT` banner. `HostVersion` stays
  `v7 GGFW v6.3.0` so UV Studio keeps selecting the matching app catalog.

### Removed from the Fusion build

- K5Viewer, Audio Scope, RF Log Lite, Resume State and the scan progress bar
  (`SCAN_PROGRESS`), to fit the flash. The code stays behind its build flag.
- PMR 446, GMRS/FRS/MURS, CA, FCC/CE/GB HAM and 430/438 F Lock plans and their
  build flags.
- `Crypto.app` (the CryKey is a menu item).

### Fixed

- The radio froze on any USB serial command (UV Studio Apps/handshake, CHIRP
  over USB): the USB reply buffer is now 4-byte aligned and
  `-Wl,--sort-section=alignment` is no longer used.
- Spectrum: the graph froze with more than 128 MR channels/steps; the sparse
  ruler stride could wrap with more than 32768 samples; SIDE1 excluded the
  first channel and its list emptied after 256 entries; the MR and VFO fixed
  listen lost the channel profile or never reopened the squelch; LNA/PGA gains
  leaked into normal RX; the 6.25k listen filter flipped a TX power bit when
  copied to the VFO.
- AM: builds without `ENABLE_FEAT_F4HWN_AUDIO` write the stock REG_2B/2F
  values again.
- Roger = MDC no longer leaves the FSK sync word changed.
- Messenger: the 30 ms squelch-close fallback runs only while the FSK sidecar
  is armed.
- UI overlaps: TX lock icon over `CALLTX`, scan-range tone label on the bottom
  line, FM pointer into the band-edge labels.
- The per-screen key/display tables are sized by `DISPLAY_N_ELEM`, so builds
  without Messenger and RF Log (RDSProbe) compile again.
- HOP: an RX session now ends when ScraEn is turned off (before, the guard
  mute kept cutting the audio every 500 ms and the idle flash commit stayed
  blocked until the squelch closed). The FM + HOP + ScraEn rule is one
  predicate shared by every HOP path (-48 B).
- HOP/CryKey now need `ENABLE_FEAT_F4HWN`, like the HOP menu entry: a
  non-F4HWN build could store a scrambler type it then rejected and transmit
  in clear. No change for Fusion.

### Known issues and proposals

- Squelch chopping on a strong repeater signal with STE ON: a false CTCSS/DCS
  tail ends the reception while the carrier is up (same in GOGUFW and F4HWN).
  Workaround: STE OFF on such channels (MANUAL, STE item and section 28). A
  code proposal that resumes audio when the carrier is still present is kept,
  untested on the radio, in tag `archive/ste-tail-resume`.
- Deferred flash writes are committed only while the radio is idle (no RX,
  TX, Messenger transaction or HOP session). A setting changed during a long
  reception and followed by a power-off before the reception ends is lost.
  Committing during RX would need a radio test (erase time vs. audio), so it
  is not changed.
- HOP RX timing: the receiver stamps the seed burst by polling REG_0C for up
  to 80 ms. If the main loop serves the FSK sync late (both IRQ bits at once)
  the slot clock is offset until the next 8 s resync, and on a HOP channel a
  Messenger frame whose first FIFO IRQ comes after 80 ms does not get the
  Dual Watch hold. Needs a radio A/B test before any change.
- Spectrum: with the SIDE2 fixed light, the bulb icon covers the 10th
  character of a 10-character channel name (documented in the MANUAL).

<details>
<summary>Development notes before 0.1.0 (chronological, newest first; some
items were superseded by later ones, such as image sizes and build flags)</summary>

- Spectrum: the backlight now follows BLTime instead of staying on. Keys
  restart the timer (EXIT on a dark screen only lights it, as on the main
  screen). Finding a transmission lights the screen and restarts the
  timer; the end of the transmission leaves the light as it is. SIDE2 now runs the F+8 cycle (always on, always off, back to
  BLTime) with the F+8 bulb next to the battery; the fixed light holds
  against keys and transmissions and is cancelled on exit. Before, SIDE2
  toggled BLMax / BLMin outside the timer. +108 B of program flash.
- Fixed: the radio froze on any USB serial command (UV Studio Apps/handshake,
  CHIRP over USB). The `-Wl,--sort-section=alignment` link flag placed the USB
  reply buffer at an odd address, and the 16-bit store of the reply footer
  then HardFaulted (Cortex-M0+ has no unaligned access). The flag is removed
  and the buffer is now explicitly 4-byte aligned. Installed apps are not
  affected (their load address is fixed). +96 B of program flash.
- Spectrum MR: key 6 (25k / 12.5k / 6.25k) now works as in VFO. The chosen
  listen bandwidth stays for every channel, while sweeping and listening,
  until EXIT or the long-MENU reset. Before the first press each channel
  still uses its own bandwidth, and the first press starts from the one on
  screen. Before, key 6 was ignored while sweeping and lasted only until the
  squelch closed. Nothing is saved: a channel opened with the second PTT keeps
  its own bandwidth. +92 B of program flash.
- UI overlaps fixed:
  - The TX lock icon (x=24..30) could be drawn over the `CALLTX` label
    (x=10..33) while transmitting. This happened on split or cross-band
    channels whose RX frequency is outside the F Lock plan. The transmitting
    VFO now runs the lock check with its TX frequency, so the lock is never
    shown there.
  - MAIN ONLY + SetGUI CLASSIC + scan range with a detected subtone: the
    `CTCSS`/`DCS` label was drawn over the VFO tone and step on the bottom
    line, leaving stray characters such as `D023NK`. The line is now cleared
    first.
  - FM radio: the frequency pointer no longer runs into the band-edge
    labels of the scale.
- MANUAL section 6: every menu item now has its own entry with what it does,
  its options, the default after `Reset ALL`, when to change it and caveats.
  The defaults come from the code. Missing defaults are filled in (SetCtr 15,
  SetMet/SetGUI CLASSIC, SetTOT/SetEOT OFF, SetTmr ON), and `SetLck` is
  corrected to KEYS+PTT. The Service items in section 21.2 get the same
  treatment.
- Messenger: the 30 ms squelch-close fallback now runs only while the FSK
  sidecar is armed, the only case where it can hide `SQL_FOUND`. With MsgRx
  OFF, only the stock receive logic ends a reception. +12 B.
- Squelch chopping on a strong repeater signal in VFO/MR (audio and green LED
  dropping at random, Spectrum unaffected): with STE ON, a false CTCSS/DCS
  tail ends the reception while the carrier is still up. Confirmed on a UHF
  repeater at S9: with STE OFF the chopping stops. The behaviour is the same
  in upstream GOGUFW, in armel's uv-k1-k5v3-firmware-custom and before the
  3.6.2 merge, so no code change; MANUAL (STE item and section 28) now says
  to use STE OFF on such channels.
- 120,524 / 120,832 bytes (308 bytes free); the Messenger fix is +12 B, the
  UI fixes -4 B.

- HOP follows the same FM-only rule as the fixed scrambler: an AM channel set
  to Scramb = HOP no longer sends the FSK seed burst or inverts the TX voice,
  and the HOP receiver only starts (and keeps) a session on FM with ScraEn on.
- The voice scrambler is FM-only everywhere, TX and RX: `RADIO_SetModulation()`
  turns it off when leaving FM (fast scan on an AM channel, Spectrum key 0, RX
  mode switch), the Scramb menu preview and the scrambler icon only act on FM,
  and the SCRAMBLE key on an AM/USB channel changes nothing and gives the
  double beep (silent with Beep OFF).
- Leaving the menu commits pending flash writes on the next idle 10 ms tick
  (still never inside RX/TX, a Messenger ACK/PONG window or a HOP session), so
  a power-off right after a menu change keeps it. No extra sector erases.
- Spectrum is back to the HOP/crypto branch behaviour: its own battery icon
  and no forced `M---` refresh. From upstream it keeps only 16..2048 samples
  (sparse ruler), the band-edge clamp and the center-mode window, which
  2048 samples need so the start frequency cannot wrap below 0.
- CHIRP module (`Gogufw_3.6.2_chirp_module.py`, now v3.6.2m) matches the
  current firmware only: F Lock lists BRASIL / DISABLE ALL / UNLOCK ALL
  (stored 0 / 9 / 10), POnMsg is gone, RF LOG is no longer offered as a key
  action, and the new **CryKey** field edits the passphrase at 0x012140.
  The older modules (3.1.1, 3.2.0, 3.3.5, 3.6.0) are removed.
- Build: `-Wl,--sort-section=alignment` in Release (-100 B, also -96 B of
  RAM; the multiboot RAM stub is byte-identical). LTO is also requested at
  link time and the toolchain no longer passes each C flag twice. Measured
  and not used: `-fno-move-loop-invariants` (-348 B, but the bit-banged
  BK4829 SPI gets ~1% slower per Spectrum scan step),
  `-fno-optimize-crc` and `-fno-tree-loop-distribute-patterns` (no change),
  `-fno-jump-tables` and `-fno-tree-loop-optimize` (larger).
- 120,516 / 120,832 bytes (316 bytes free), RAM 15,544 B.

- CryKey is now a firmware menu item (**Msg → CryKey**, up to 10
  characters, case-sensitive) using the shared text editor; `Crypto.app` is
  removed. The passphrase is stored right after the Messenger config in its
  sector (0x012140), written only when it changes, kept by CHIRP uploads and,
  like `CllSgn`, kept per multiboot config bank. **After updating, enter the
  CryKey again in the menu**: the old Crypto.app value is not read. +212 B
  (120,364 / 120,832 bytes, 468 bytes free).
- Menu categories: SetRxG (from upstream 3.6.0) now appears under Audio, not
  only in All.

- Merged GOGUFW 3.6.0/3.6.2 (upstream main) into this branch: per-channel
  scrambler fixes (saved on MR channels, kept off DTMF/CSS tail/Roger/FSK/tone
  TX), overlay API 4 and GOGU API 4 (`key_state`/`voice_tx`, needed by the
  FMVFO/FMMemory 3.6.0 apps), Spectrum 16..2048 samples with band-edge
  clamping, `M---` refresh and the standard battery icon, BK4829 compander
  register fix, SetRxG, AUTO CHECK blocked-TX handling, power save reset on
  app exit, new status icons and 520 B of RAM savings. The branch's
  `RADIO_ApplyScramble()` (with HOP) remains the only scrambler entry point
  and gains upstream's FM-only and range guards; the double-wave scrambler
  icon is kept instead of upstream's `S`.
- Spectrum: the sparse ruler stride is 16-bit, so a scan range with more than
  32768 samples can no longer wrap it to 0 and hang the ruler loop.
- CHIRP: `Gogufw_3.6.2_chirp_module.py` carries the HOP scrambler and the
  SCRAMBLE side-key action.
- `ENABLE_FEAT_F4HWN_SCAN_PROGRESS` is off in Fusion to fit the merge (it
  cost 1,820 B): no `nn/total` counter, progress bar or 1 s `SCAN LIST`
  banner; the status bar still shows the active list. Firmware is now
  `v3.6.2m`, 120,152 / 120,832 bytes (680 bytes free) including the 920-byte
  multiboot load image; RAM 15,640 / 16,384 bytes.

- Spectrum (F+5) in MR mode now sweeps only the active scan list (ScList, or
  the last list chosen during a scan) instead of every valid channel, so the
  channel count and the `M:` first/last labels match the list. Excluded
  channels are skipped like in the memory scan; an empty list falls back to
  every valid channel. The top-left label shows the list and the count
  (`ALL 42`, `RPT 12`, `03 7`) instead of `MR:<n>`. Key 4 moves to the next
  valid list and restarts the sweep; the choice stays in RAM (no flash write)
  and is used by the memory scan afterwards. Net -20 B of program flash: the
  scan-list label code in the status bar and the scan-list banner now share
  one formatter. The status-bar highlight box fits 1-2 letter list names.
  In MR mode the trigger line leaves a fixed gap for the list label.
  The bottom labels show the first and last position inside the list
  (`M:1` … `M:<n>`) instead of MR channel numbers; the right one is
  right-aligned so `M:1024` fits.
- Spectrum SIDE1 (ignore the current peak for this Spectrum session): a
  press with no peak yet (right after another exclusion or a sweep restart)
  no longer silently excludes the first step/channel, and the MR key-4 list
  change clears the exclusions, which are stored by list position and would
  otherwise hit other channels of the new list.
- Spectrum MR fixed listen (PTT on a channel): when the squelch closed, the
  radio went back to the sweep profile (the VFO's modulation and bandwidth at
  F+5), so the next opening of an AM channel, or of an N channel with the VFO
  on W, was heard with the wrong demodulation/filter, and a key-6 change was
  lost. The fixed listen now keeps the channel's profile until EXIT.
- Spectrum MR now looks like VFO: the frequency uses the same font, and the
  listen bandwidth (`25k`/`12.5k`/`6.25k`) is shown next to the modulation.
  While sweeping both are the peak channel's own; while listening they are the
  live ones (key 6 in MR: see the first entry). TuneToPeak() reuses the peak
  channel profile already cached for the display instead of reading the
  channel again. +28 B of program flash in total.
- Spectrum VFO fixed listen: key 0 set the trigger to "none" (65535), so the
  squelch never reopened and the audio stayed off until EXIT (or monitor).
  The fixed listen now keeps the trigger level; the sweep after EXIT still
  recalibrates scale and trigger for the new modulation. +4 B; the .bin is
  120828 B (4 B free).
- Flash wear: the SPI flash driver now defers sector erases (write-back). A
  change that needs an erase stays in the 4 KiB sector cache and is committed
  once, 2 s after the last write while the radio is idle (foreground or power
  save, no Messenger transaction, no HOP session), when another sector is
  touched, or before a reset, bank switch, overlay app, Spectrum or multiboot
  operation; reads of the pending sector are served from the cache. `SaveSettings` (5 blocks),
  `SaveFM`, `WriteCurrentState`, Messenger config, `ResetTxLock`, UP/DOWN
  tuning, typed frequencies and CHIRP uploads (8-byte chunks) now cost about
  one erase per sector instead of one per write. A power-off within 2 s of a
  change loses that change but no longer risks a half-erased sector.
- Messenger IDs are reserved in blocks of 64: the flash keeps a ceiling and
  is written once per 64 TEXT/PING packets instead of on every one (an AUTO
  CHECK every 30 s used to erase the Messenger sector 120 times an hour). The
  repeated `MSG_STORE_Init` reloads can no longer move the ID backwards. The
  block reservation is committed immediately, before the TX and its ACK/PONG
  window.
- Mute no longer saves volume 0 into the shared calibration sector: the saved
  volume is always the unmuted level, so mute does not survive a reboot and
  unmuting cannot erase the calibration sector.
- Typing a frequency in VFO starts a clean simplex setting: RX/TX CTCSS/DCS,
  offset direction/value and reverse are cleared (power, bandwidth, step,
  modulation and scrambler are kept). EXIT restores and re-saves the previous
  VFO, so the flash no longer keeps the partial frequency after a cancel.
- Spectrum (F+5) PTT×2: a peak that matches a saved MR channel (closest
  within half a scan step, max 6.25 kHz) now opens that channel in MR mode
  with its own settings. Any other peak lands in the VFO like a typed
  frequency: bandwidth, modulation, step and power come from the band's VFO
  (Narrow by default) instead of the Spectrum listen filter (which made it
  Wide), and tones/offset/reverse are cleared. Fixes the 6.25k listen filter
  writing bandwidth 2 into the VFO, which flipped a TX power bit, and 8.33k
  peaks being applied unrounded.
- Spectrum listen bandwidth follows the radio: on F+5 (and on the MENU-hold
  reset) it starts from the VFO/channel mode (W = 25k, N = 12.5k, N with
  SetNFM = 6.25k), MR channels in a channel sweep use N+ when SetNFM is on,
  and key 6 now changes the filter at once while listening. The listen
  filters are the same REG_43 values the VFO uses. PTT×2 to a VFO applies
  the Spectrum mode: 25k = Wide, 12.5k = Narrow with SetNFM off, 6.25k =
  Narrow with SetNFM on (SetNFM is global and is saved only when it
  changes). The stored listen bandwidth bits are no longer used, so the
  Spectrum settings block is not rewritten when the VFO mode differs.
- AM: builds without `ENABLE_FEAT_F4HWN_AUDIO` write the stock AM REG_2B/2F
  values again (0x0400/0x9990) instead of keeping the FM ones. The AM audio
  profile comments now describe what the values do: SHARP and STOCK differ
  only in REG_2B (3 kHz LPF off vs. 300 Hz HPF off). No change for Fusion.
- Spectrum: LNA/PGA gain changed in the Still register menu is restored on
  exit (REG_13 was not saved), so it no longer leaks into normal RX until a
  FM/AM change or reboot.
- Fusion: `ENABLE_FEAT_F4HWN_RESUME_STATE` off to make room (-372 B). The
  radio no longer resumes scan/Spectrum/FM after a power cycle, and starting
  a scan or Spectrum no longer writes the state byte to flash. A scan list
  chosen during a scan (two digits or the SCAN action) is still kept across
  reboots through a normal settings save. Byte 0xA00F now holds `VFO_OPEN`
  as in upstream builds without RESUME_STATE; existing values (state | list
  << 3, always >= 8) still load as VFO open.
- Image: 120,784 / 120,832 bytes (48 bytes free), including the 920-byte
  multiboot load image; RAM 16,240 / 16,384 bytes; multiboot RAM-stub
  isolation checks pass.
- Scan interface aligned with F4HWN: the Fusion build now enables the scan
  progress bar (`nn/total`, `P1`/`P2`, excluded columns), the scan-list name
  and the RSSI sparkline (`ENABLE_FEAT_F4HWN_SCAN_PROGRESS` and
  `ENABLE_FEAT_F4HWN_SCAN_RSSI`).
- The scan-list name is now counted on the 10 ms tick and stays on screen for
  one second, as in F4HWN.
- Holding MENU to exclude a memory channel during a scan no longer shows the
  list name or pauses the scan; the bar is rebuilt and scanning continues.
- Added F4HWN menu categories (`ENABLE_FEAT_F4HWN_MENU_CAT`) with a GOGUFW
  `Msg` category for the Messenger settings, CALLTX settings under Audio and
  Scramb under Channels. The status bar shows `MENU` or the open category.
- Removed `POnMsg`. The radio always boots as with `NONE`: no welcome screen,
  no boot beeps and no boot delay. The settings byte is written as `NONE`; the
  boot message text is kept for CHIRP and for the APRS apps' callsign fallback.
- Removed K5Viewer and RF Log Lite from the Fusion build to free RAM and Flash
  for the scan interface. A side key still set to RF LOG does nothing.
- Spectrum (F+5): ported the F4HWN fix that keeps the trigger line off the
  `12.5k`/`6.25k` bandwidth text. SIDE1 no longer also excludes the first
  step/memory, the exclusion list no longer empties itself after 256
  exclusions, and with more than 128 steps (large MR lists or long scan
  ranges) excluding a peak no longer stops its neighbouring steps from being
  measured, and its column keeps showing their signal. MR sweeps no
  longer rescan the channel list from the start on every step.
- F Lock now offers only **BRASIL** (Anatel Ato 14448 general-use channels,
  462.5625–462.7125 and 467.5625–467.7125 MHz), **DISABLE ALL** and
  **UNLOCK ALL**. The DEFAULT+, FCC/CA/CE/GB HAM, 430/438, PMR 446 and
  GMRS/FRS/MURS plans were removed, along with their build flags. The stored
  byte keeps the legacy codes: radios set to DISABLE ALL or UNLOCK ALL keep
  them, and any other stored plan (including the former DEFAULT+) loads as
  BRASIL, so 2 m/70 cm TX now needs UNLOCK ALL. The CHIRP module is unchanged
  and still shows the old labels; any choice other than DISABLE/UNLOCK ALL
  becomes BRASIL on the radio.
- The per-screen key and display tables are now sized by `DISPLAY_N_ELEM`, so
  builds without both Messenger and RF Log (e.g. `RDSProbe`) compile again.
- The SCRAMBLE side-key action now enables 3200 Hz by default (was 2600 Hz)
  when no scrambler tone has been used since power-on.
- Version string is now `v3.3.5m` (radio identity `GOGUFW v3.3.5m`; multiboot
  image `f4hwn.gogufw.v3.3.5m.bin`). The first boot after updating runs the
  usual version-change reset of KEY_LOCK, MENU_LOCK, SET_KEY and SET_INV.
- Release C flags add `-fno-tree-ch -fno-ssa-phiopt` (measured -276 bytes).
  `mb_flash.c` restores both passes, so the multiboot RAM stub stays
  byte-identical (920 bytes, 85 checked branches).
- Final image: 118,932 / 120,832 bytes (1,900 bytes free), including the
  920-byte multiboot load image. Linked firmware uses 118,012 bytes; RAM usage
  is 16,328 / 16,384 bytes, and multiboot RAM-stub isolation checks pass.

</details>

---

# GOGUFW history (upstream, by Gogu-Qs)

The entries below are the GOGUFW changelog as published by Gogu-Qs up to
GOGUFW 3.6.2, the base of KLEANFW 0.1.0.

## GOGUFW 3.6.2 — 2026-11-07

### Scrambler

- Restored the per-channel scrambler setting in the GOGUFW Fusion build and
  made channel changes retain it correctly. Returning to a channel now restores
  that channel's saved scrambler state instead of silently switching it off.
- Applied the selected scrambler consistently to normal FM receive and voice
  transmit paths. DTMF, FSK, alarm/tone, CSS-tail and Roger/MDC signalling are
  explicitly sent without voice inversion, then normal voice operation restores
  the channel setting.
- Added a compact uppercase `S` in the smallest status font between the No FSK
  and No Roger indicators. The FSK indicator now starts at x=111 so the three
  indicators remain tightly spaced without overlapping.

### Overlay power save

- Confirmed that overlay applications do not enter power save while they own
  the main loop. The idle timer can still expire in the background, however,
  leaving a pending power-save request behind.
- Reset the idle countdown and clear pending/expired power-save requests when
  an overlay closes. Returning from an app is now treated as user activity, so
  the radio no longer enters power save immediately or flashes the `Zz` status
  icon as soon as the normal screen returns.

### Compatibility and release files

- Existing overlay applications remain compatible and were not updated for
  this firmware-only release. The previous application versions may continue
  to be used.
- `Apps.zip` is supplied for convenience and contains the unchanged
  `FMVFO.app`, `FMMemory.app` and `Search.app` files.
- Updated the CHIRP module's driver, variant and firmware-comparison identifiers
  to 3.6.2. Its channel/settings schema is unchanged from the 3.6.0 module.
- Firmware: `GOGUFW-v3.6.2.bin` (single image for direct flashing and the
  supported multiboot workflow).
- CHIRP module: `Gogufw_3.6.2_chirp_module.py`.
- Final published image: **120,220 / 120,832 bytes** (612 bytes free), including
  the 920-byte multiboot load image. Linked code/data uses 119,300 bytes.
- RAM with the enforced stack reservation: **15,832 / 16,384 bytes** (552 bytes
  free); the 4,096-byte runtime stack and multiboot RAM-stub isolation checks pass.

## GOGUFW 3.6.0 — 2026-11-04

### Spectrum

- Expanded VFO Spectrum from the original 16/32/64/128 sample choices to
  **16/32/64/128/256/512/1024/2048 samples**. Higher values cover a much wider
  frequency span without requiring one LCD column for every RF sample; multiple
  samples are combined into the 128-column graph.
- Corrected centred VFO-window arithmetic and band-edge clamping for the new
  spans. Changing the sample count, entering a frequency or launching Spectrum
  near a band edge no longer shifts, wraps or truncates the requested range.
- Kept the established pre-expansion graph/ruler height in both VFO and Memory
  Spectrum. Memory channel labels fit around the unchanged ruler instead of
  pushing the graph upward.
- Reduced ruler density at 512/1024/2048 samples and clears only the small
  marker field before drawing the selected-peak triangle. Wide-span rulers no
  longer turn into a solid comb or obscure the marker.
- Fixed the temporary `M---` value that could remain after listening to a
  signal. The first valid RSSI measurement now refreshes the status line
  immediately.
- Replaced Spectrum's private battery drawing with the exact normal-radio
  battery icon and low-battery blink behaviour. VFO channel-name refreshes no
  longer erase the battery's left edge and make it flash.

### Overlay API and F4HWN 7.0 compatibility

- Advanced the public overlay contract from official **ABI 1 / API 3** to the
  append-only **F4HWN API 4** layout. It adds memory-channel/list access and
  staging, per-app TX-power control and the GPS-position service slot. The
  existing API-3 prefix is byte-for-byte unchanged, so applications built for
  GOGUFW 3.3.1–3.3.5 remain compatible.
- Added API-4 memory staging: a compatible overlay can select a stored channel,
  return to the main radio and have the new VFO/MR state committed safely after
  the overlay has closed.
- Added the official GPS entry point without inventing a stale fix. Until a live
  K5Viewer GPS position is retained by the firmware, it returns unavailable so
  applications can use their saved-coordinate fallback.
- Extended the separately versioned GOGU capability table with resident analog
  voice PTT and shared F/key-lock state. This does not consume unofficial public
  API numbers and leaves future F4HWN levels free to append normally.
- Corrected overlay analog voice TX to enter the same resident transmit/audio
  path as physical PTT, including decoder reset, audio filtering, PTT-ID policy
  and TX ownership. Overlay exit also restores the selected VFO's squelch and
  output-power state.
- Added the F4HWN 7.0 `SetRxG` option for the BK4829 receive audio gate. RX
  Compander forces the required gate automatically; otherwise the user may
  disable it to pass weaker modulation at the cost of more inter-word hiss.
- Corrected BK4829 Compander handling. BK4819 ratio-table writes are no longer
  sent to BK4829 noise-gate/HPF registers, preventing muted or damaged ordinary
  analogue RX audio.

### FM VFO and FM Memory overlays

- Updated both paired Broadcast FM applications to **3.6.0-gogu**. The resident
  status bar is now continuous from launch and remains visible while tuning,
  changing channels or using menus; its non-battery icons no longer disappear
  for about one second and reappear.
- Added normal keypad-lock behaviour to both FM overlays. Holding F toggles the
  resident lock, locked keys are ignored, and the resident F/lock indicators
  remain synchronized rather than being redrawn privately by the app.
- Reduced FM VFO's overlay-RAM state by compiling Memory-only name-editor data
  only into FM Memory. Both applications still preserve all existing FM
  settings, names and 48 saved stations.
- **Reinstall both `FMVFO.app` and `FMMemory.app`** to receive these changes.
  Installing only one leaves the paired FM interface at mixed versions.

### Messenger, HEARD and status/audio feedback

- When AUTO CHECK cannot transmit because of No FSK, frequency lock, modulation,
  configuration or battery policy, it now behaves like manual SEND PING: AUTO
  CHECK is cancelled and the same visible TX-blocked notice is shown. A blocked
  background attempt brings HEARD forward so the reason cannot expire unseen.
- The TX-blocked warning now plays immediately rather than being queued until a
  later key press. Its new restrained negative alert is a 650 Hz short/long
  `dit, diit`, clearly different from PONG/ACK and new-message sounds.
- Replaced the Power Save text with the requested compact 9x7 `Zz` symbol and
  refreshed the 11x7 unread-message envelope, 10x7 AUTO CHECK chain link, No
  FSK and No Roger marks from the supplied pixel references. Status-bar symbols
  are top-aligned with the existing resident icons.
- Kept AUTO CHECK and unread-message state independent: the chain link still
  reports the most recent automatic PING result while the envelope is driven
  only by unread Inbox messages.

### Applications and reliability

- Updated the APRS DIGI source to **0.2-gogu**. It now reads the shared APRS
  profile/callsign configured by APRS Edit through the bounded config proxy and
  no longer stores a conflicting private one-byte mode. APRS DIGI is not one of
  the three overlay binaries bundled with this release.
- Updated the CHIRP module's driver, variant and firmware-comparison identifiers
  to 3.6.0. Its channel/settings schema is unchanged from the 3.3.5 module.
- Saved 520 bytes of resident RAM without reducing the 4 KiB runtime stack or
  4 KiB overlay workspace: UART and USB share one synchronous decoded-command
  staging buffer (256 bytes), the full-speed USB endpoint buffer is one packet
  instead of two (64 bytes saved), and RF Log Lite's guarded 200-byte entry
  array occupies the otherwise unused gap before the fixed overlay anchor.
  UART and USB RX rings remain independent.
- Preserved RF Log Lite's 20-entry behaviour, CHIRP/K5Viewer USB operation,
  multiboot workspace isolation and all normal radio functions. Host regression
  tests pass under AddressSanitizer and UndefinedBehaviorSanitizer for the font
  dictionary/framebuffer, USB, Messenger protocol/store and HEARD layout.

### Compatibility, release files and build information

- `Search.app` remains **1.5-gogu** and is byte-for-byte identical to the 3.3.5
  release (`SHA-256 acda9da...5753`). It is supplied as the requested Scan/Search
  package for convenience and does not need to be reinstalled if already present.
- `FMVFO.app` and `FMMemory.app` changed and should be reinstalled together.
- Old custom GOGUFW binaries made for the pre-3.3.1 experimental API 4–9 layout
  remain incompatible. Official API-3 apps made for GOGUFW 3.3.1 and later are
  still compatible because official API 4 only appends services.
- Firmware: `GOGUFW-v3.6.0.bin`
- Multiboot firmware: `GOGUFW-v3.6.0-multiboot.bin`
- CHIRP module: `Gogufw_3.6.0_chirp_module.py`
- Overlay packages: `Search.zip`, `FMVFO.zip`, `FMMemory.zip` and the combined
  `Apps.zip` containing those three applications.
- Final published image: **120,028 / 120,832 bytes** (804 bytes free), including
  the 920-byte multiboot load image. Linked code/data uses 119,108 bytes.
- RAM with the enforced stack reservation: **15,840 / 16,384 bytes** (544 bytes
  free); the 4,096-byte runtime stack and multiboot RAM-stub isolation checks pass.

## GOGUFW 3.3.5 — 2026-11-01

- Replaced the generic new-message double beep with the measured timing of the
  requested classic Nokia alert: four equal 932 Hz, 200 ms notes in two close
  pairs, with an approximately 735 ms pause between the pairs. ACK packets no
  longer enter the new-message alert path and now use the same short 500 Hz
  confirmation chirp as a manual PONG. Both remain controlled by `MsgBeep`.
- Extended HEARD with a MENU containing manual **SEND PING** and background
  **AUTO CHECK** choices for OFF, 30 seconds, 1 minute, 5 minutes and 10 minutes.
  Automatic replies update HEARD silently; each unanswered check produces one
  restrained 450-to-320 Hz descending alert directly from the background tick
  instead of accumulating until a key is pressed. This is distinct from the
  positive manual-PONG chirp and the new-message alert. Checks continue
  on every normal firmware screen and defer only while voice TX/RX or scanning
  owns the RF path.
- Added a chain-link AUTO CHECK result indicator using the otherwise
  empty VOX slot. Selecting an interval sends the first PING immediately and
  leaves the result slot blank until it completes: the link stays fixed after
  a PONG and blinks at one-second intervals after an unanswered 12-second
  window. Later checks retain the previous completed result until the new
  result is known. The real VOX icon has priority whenever VOX is enabled, and
  the Messenger envelope, F/key-lock, mute/backlight and battery areas remain
  unchanged.
- Decoupled the Messenger envelope from AUTO CHECK. It now follows only the
  Inbox unread flags and refreshes immediately when the final unread message is
  opened or deleted.
- Made private overlay tuning explicitly end resident RX state and pending
  dual-watch activity before taking ownership of the radio, preventing a late
  resident squelch event from pulling an active overlay back to the main screen.
- Recalibrate the VFO Spectrum scale and trigger after `1`/`7` scan-step
  changes. This prevents ordinary traces from falling below the graph when the
  BK4819 scan filter switches between the 15 kHz-and-up and sub-15 kHz paths.
- Apply the same recalibration after `0` switches between FM, AM and USB, so a
  previous modulation's noise floor cannot hide the new trace.
- There is no overlay ABI/API change in 3.3.5. Applications built for the
  official F4HWN 6.3.0 ABI 1 / API 3 contract used by GOGUFW 3.3.1 and 3.3.2
  remain compatible; a firmware-only update is supported.
- `FMVFO.app`, `FMMemory.app` and `Search.app` are included again for
  convenience but are unchanged from 3.3.2. If they are already installed,
  they do not need to be reinstalled; updating only the firmware is sufficient.
- Firmware: `GOGUFW-v3.3.5.bin`
- Multiboot firmware: `GOGUFW-v3.3.5-multiboot.bin`
- CHIRP module: `Gogufw_3.3.5_chirp_module.py`
- Final published image: 118,992 / 120,832 bytes (1,840 bytes free), including
  the 920-byte multiboot load image. Linked firmware code/data uses 118,072
  bytes; RAM usage is 16,360 / 16,384 bytes, and multiboot RAM-stub isolation
  checks pass.

## GOGUFW 3.3.2 — 2026-10-26

- Ported the insertion-style text editor from Messenger PR #14 to every
  resident text-entry screen: Messenger Compose, channel name, Messenger
  callsign and FM channel name. UP/DOWN (LEFT/RIGHT on UV-K1) moves a visible
  caret, insertion and backspace work in the middle of text, and `0` inserts a
  space immediately. Input modes now display as `ABC`, `abc` and `123`. The
  caret stays under a multi-tap character until it is committed. Thanks to
  [@booqoffsky](https://github.com/booqoffsky) for proposing and implementing
  the original interaction improvements in
  [PR #14](https://github.com/Gogu-Qs/GOGUFW-UV-K1-Messenger/pull/14).
- Applied immediate space insertion to APRS Edit and moved FM Memory name entry
  onto the same resident editor through the append-only GOGU extension level 2.
  The new FM Memory declares its capability so older firmware rejects it
  cleanly instead of attempting to call an unavailable service.
- Preserved the navigation-layout setting across power cycles in non-RescueOps
  builds and made Reset All clear both the navigation and key-lock bits.
- Fixed the Spectrum graph in both VFO and Memory modes so weak traces remain
  visible above the bottom axis. Initial scaling now also considers the weakest
  sample in the completed sweep, making the scale easier to recover on mixed
  or quiet channel ranges.
- Renamed the assignable side-key action from `PTT` to `PTT MODE` in the
  firmware and CHIRP module. Its behavior is unchanged: it switches the SetPTT
  setting between Classic and OnePush; it is not a transmit command.
- There is no breaking overlay ABI/API compatibility change in this release. Existing
  GOGUFW 3.3.1 applications remain compatible and do not need to be reinstalled;
  users may update only the firmware. Refreshed FM Memory/APRS Edit builds are
  optional and only carry the new text-entry behavior.
- Firmware: `GOGUFW-v3.3.2.bin`
- Multiboot firmware: `GOGUFW-v3.3.2-multiboot.bin`
- Optional refreshed overlays: `FMMemory.app`, `APRSEdit.app`
- CHIRP module: `Gogufw_3.3.2_chirp_module.py`
- Final Flash image: 117,864 / 120,832 bytes (2,968 bytes free), including the
  920-byte multiboot RAM-stub load image; isolation checks pass.

## GOGUFW 3.3.1 — 2026-10-25

### Breaking overlay API change

- Replaced GOGUFW's experimental public API levels 4–9 with the exact F4HWN
  6.3.0 ABI 1 / API 3 prefix. The official layout now includes number helpers,
  direct RX-audio capture, 48 MHz timing, app hand-off, quiet TX preparation,
  USB serial, paced LCD output and deferred VFO staging.
- Moved GOGU-only services into the separately versioned `APP_CAP_GOGU_EXT`
  table discovered through the official API-2 asset service. This leaves room
  for future F4HWN API levels without forcing another rewrite of every GOGU app.
- **Old GOGUFW overlay binaries built for experimental API levels 4–9 do not
  work with 3.3.1.** The loader deliberately reports them as incompatible.
  Reinstall `Search.app`, `FMVFO.app` and `FMMemory.app` from this release after
  flashing the firmware. Other custom GOGU apps also require their API-3 builds.

### Firmware and radio fixes

- Ported F4HWN 6.3.0 application services and added the serial capability used
  by the official KISS TNC app. KISS temporarily owns USB while running, then
  cleanly resynchronizes K5Viewer; CHIRP and Live Viewer remain available after
  exit. KISS starts on the frequency saved by APRS Edit/APRS TX and falls back
  to 144.800 MHz when the shared profile is absent or invalid.
- Fixed range scanning stopping on a detected carrier without opening audio
  when Messenger was enabled. The normal scan state machine now continues
  independently while Messenger still receives its radio interrupts.
- Compared the normal analog receive path with F4HWN 6.3.0. Restored the
  missing RX expander/TX compressor setup and limited the AM weak-signal filter
  behavior to AM instead of forcing it on normal FM channels. The upstream
  CTCSS/DCS found/lost and squelch state machine is retained.
- Restored GOGUFW's compact 20-entry RAM-only RF Log Lite. Removed the larger
  F4HWN RX/TX log, its external-Flash history and its UV Studio export protocol;
  generic K5Viewer/Live Viewer screen capture and CHIRP USB handling stay.
- Removed the resident SysInfo menu screen because diagnostics are supplied by
  `SystemInfo.app`. Its current stack value now follows the real stack pointer,
  and its minimum value uses a boot-time high-water pattern rather than the old
  fixed `_estack - _ebss` calculation.
- Preserved the 48-slot F4HWN 6.3.0 Labs identity used by UV Studio. APRS Edit
  and APRS RX use catalog-matching version metadata to avoid false warnings.
- Added ACARS RX fixed at 131.725 MHz and APRS-profile tuning for KISS TNC in
  source; the public release assets below remain limited to the requested core
  firmware, Search and paired Broadcast FM apps.

### Release files and build information

- Firmware: `GOGUFW-v3.3.1.bin`
- Multiboot firmware: `GOGUFW-v3.3.1-multiboot.bin`
- Required overlay updates: `Search.app`, `FMVFO.app`, `FMMemory.app`
- CHIRP module: `Gogufw_3.3.1_chirp_module.py`
- Final Flash image: 117,236 / 120,832 bytes (3,596 bytes free), including
  the 920-byte multiboot RAM-stub load image omitted by the linker region total
- RAM/linker reservation: 16,360 / 16,384 bytes; 4,096-byte runtime stack
  reservation retained and multiboot RAM-stub isolation verified

## GOGUFW 3.2.0 — 2026-10-21

- Rebuilt Broadcast FM as two paired overlays, `FMVFO.app` and
  `FMMemory.app`. Both are required. F+0 still opens the last-used FM mode and
  EXIT/F+0 returns to the exact radio screen and VFO/MR state that launched it.
- Preserved all GOGUFW FM features and visuals: four bands, manual and automatic
  scan, 48 named memories, save/delete, name editor and Live RSSI. Existing FM
  storage addresses and formats are unchanged.
- Fixed the held-3 hand-off leak between FM Memory and VFO, RSSI initialization
  after launch/mode change/RX resume, scan continuation, menu and Ch Save key
  routing, empty-band selection and the name editor's `*:B`/`*:b`/`*:2` label.
- During selected-channel RX, FM yields to the main screen and radio audio, then
  restores the same FM mode and tuning five seconds after RX ends. EXIT remains
  usable during continuous RX.
- Fixed missed squelch-close recovery on modal screens, preventing RX and audio
  from remaining latched while browsing Apps or editing FM data.
- Removed the resident FM UI/state machine and replaced the 768-byte FM-name
  staging table with a safe single deferred record. Firmware Flash use is
  114,020 / 120,832 bytes, saving 5,888 bytes versus 3.1.1.
- Enforced a 4 KiB minimum runtime stack at link time. The final image has a
  4,148-byte stack gap, restoring APRS RX operation and preventing silent stack
  overlap from future resident-RAM growth.
- Restored the per-channel Scrambler and Compander firmware controls.
- Added the matching `Gogufw_3.2.0_chirp_module.py`. It retains the 3.1.1
  FrozenMemory/invalid-value regression fix and exposes Scrambler, Compander,
  No FSK TX and No Roger using the unchanged EEPROM fields.
- Published Search v1.5 with the release. Its Frequency and Tone status/result
  rows are vertically centred between the dotted separators; install it to
  enable the F+4 and F+STAR shortcuts.

### Build information

- Firmware: `f4hwn.gogufw.v3.2.0.bin`
- Multiboot image: 114,940 bytes (5,892 bytes free)
- Firmware Flash before multiboot stub: 114,020 / 120,832 bytes (6,812 bytes free)
- Runtime stack gap: 4,148 bytes (4,096-byte minimum enforced)
- FM VFO overlay: 4,092 / 4,096 bytes
- FM Memory overlay: 4,072 / 4,096 bytes
- Search overlay: 3,060 / 4,096 bytes

## GOGUFW 3.1.1 — 2026-10-17

- Removed Rescue Ops and the 1750 Hz call tone from the Fusion build, retained Screenshot support, and switched LTO to a single optimization partition for lower Flash usage without changing runtime features.
- Fixed Full Watch VFO/MR switching while priority channel C or D is selected. V/M now changes the selected C/D object's temporary mode in place without jumping to or modifying its physical A/B host; A/B/C/D navigation preserves that mode until the priority channel is reloaded.
- Reclaimed 576 bytes of overlay-app runtime stack without removing firmware features by allocating Spectrum's channel bitmap, RSSI history and peak-hold arrays only while Spectrum owns the foreground. This fixes APRS RX launch failure in both the bundled and original Armel packages while leaving Spectrum's own effective memory budget unchanged.
- Fixed overlay launch preparation and cleanup retaining an oversized live stack frame while an app was running. The safe split-phase launcher leaves only a 40-byte dispatcher frame alive and never moves the stack over suspended caller data.
- Apps re-arm the backlight when closing, and settings loading repairs a persisted invalid `BLMax=0` to 10.
- Restricted the middle-row pre-RSSI cleanup to Full Watch. Triple/Quad Watch still clears its center indicator before drawing the RSSI meter, while Main Only RX no longer erases unrelated middle-screen content when its RSSI meter is drawn on the lower row.
- Verified the late F4HWN erased packed-settings-byte correction is already present in GOGUFW. Erased FM/AM audio-profile data is stored separately and already falls back safely to profile 0, so no audio profile is removed or loaded from an erased value.
- Renamed the Messenger `MsgCsg` menu item to `CllSgn` and moved it to the end of the Messenger settings immediately before `RngRsp`. Compatible APRS TX and Beacon apps now use this shared station callsign, falling back to the unchanged Power On Message only when `CllSgn` is empty or invalid.
- Updated firmware and CHIRP version identifiers to 3.1.1.
- Restored the complete deferred Broadcast FM overlay bridge when the resident FM radio is enabled. Armel's original Broadcast FM app now receives real BK1080/state/save callbacks; GOGUFW's resident FM names, live-RSSI interface and Messenger return flow remain unchanged.
- Added the bounded configuration-proxy capability used by current Armel APRS Edit/TX packages, extended it for GOGUFW's 122-byte APRS profile, and added saved-screen persistence for current SSTV packages.
- Corrected overlay shortcut parsing for current single-ID headers. GOGUFW Search's combined ID still owns F+4/F+STAR regardless of slot order, while unrelated IDs can no longer be mistaken for multiple bit-mask shortcuts.
- Verified the archived Armel v6.1.0 and v6.2.0 app sets against the final loader headers, VMA, CRCs and capability mask. Beam remains intentionally unsupported because its dedicated resident FSK bridge is disabled; all other archived apps validate with their required services.
- Ported the applicable F4HWN 6.2.0 reliability fixes: the automatic keypad-lock countdown is now wide enough for every menu duration, S-meter band 1/2 corrections are preserved on settings saves and repaired after the historical zeroing bug, and the K1 battery entry is correctly labelled 1400 mAh.
- Expanded the external-Flash app region from 16 to 48 slots. Empty slots remain hidden; held UP/DOWN repeats through long lists, and `*` toggles the right column between code size and app version. The loader, UART/USB management and config-proxy lookup all use the same 48-slot limit.
- Fixed UV Studio still reporting `16 of 16 slots used`: the host-only UART identity now advertises the compatible F4HWN 6.2 baseline first, allowing UV Studio to select its 48-slot layout. The radio UI and SysInfo continue to show GOGUFW 3.1.1.
- Suppressed the false main-channel RX indication caused by a manual Broadcast FM retune on frequency-sensitive VFO combinations such as 439.400 MHz. The guard follows the BK4829 squelch latch instead of expiring while its stale open event is still active, with a bounded timeout for a real carrier.

Only the firmware image and matching CHIRP module are published as 3.1.1 release assets. Personal GOGUFW-modified overlay-app packages are not included.

### Build information

- Firmware: `f4hwn.gogufw.v3.1.1.bin`
- Multiboot image: 120,828 bytes
- Firmware Flash: 119,908 / 120,832 bytes (924 bytes free before the 920-byte multiboot stub; 4 bytes free in the final image)
- RAM: 13,288 / 16,384 bytes (3,096 bytes free after the linker reservation; the overlay launcher's live frame while app code runs is reduced from 224 to 40 bytes without moving the stack)

## GOGUFW 3.1.0 — 2026-10-10

- Ported Full Watch from F4HWN 6.1.0, including Full RX/Main TX modes, priority-channel rotation and Triple/Quad Watch display.
- Prevented the Full Watch middle-line indicator from overlapping the `UNLOCK KEYBOARD` notice; it is hidden for the notice duration and restored automatically.
- Removed Survival Mode and disabled Action Picker in the Fusion firmware profile.
- Merged Range Check into the HEARD screen. `WAIT` is drawn at the one-pixel left inset before PING transmission starts; existing MSG/ACK/PNG/PON rows remain visible, and PONG responders move to the top with evenly grouped voltage and RSSI details.
- Restored F4HWN 6.1.0's original Full Watch foreground/background pointer swap. A promoted RX channel remains visible, the displaced foreground stays in the rotation, and the normal cycle resumes after RX.
- Full Watch background-channel Messenger TEXT/PING RX now promotes only after a valid packet is decoded; ACK/PONG uses the promoted channel's actual RX/TX configuration and the normal Full Watch cycle resumes afterward.
- Fixed automatic ACK transmission from a promoted Full Watch priority channel: the ACK path now selects the actual promoted `VFO_Info` target rather than treating its shared A/B index as the physical foreground VFO, and restores the locked RX target after TX.
- Fixed manual PTT after a background message promotion in Full RX/Respond: Messenger voice-path cleanup now preserves the active received channel, while Full RX/Main TX continues to use the configured main TX VFO.
- Extended the A/B action in Full RX/Respond to cycle through every transmit-capable watched target (`A`, `B`, priority 1 and priority 2). Invalid or duplicate priority entries are skipped; Main TX/Full RX keeps its fixed-main-TX behavior.
- Extended the same watched-target selection to Main TX/Full RX; after a manual A/B action the displayed watched target becomes the PTT target, while automatic Full Watch reception keeps the configured main-TX behavior. Menu edits follow the displayed virtual priority VFO instead of silently editing physical B.
- Restored the overlay system-information capability and fixed the append-only app API layout. SystemInfo now receives the v6.1-compatible resident metadata, battery, storage and stack services.
- Cleared the middle display line immediately before drawing the RX RSSI meter, eliminating the first-frame overlap with the Triple/Quad Watch indicator.
- Updated the armed F-key status icon to the F4HWN 6.1.0 bitmap.
- Quad Watch battery saving now completes the full four-channel receive cycle before sleeping. Range Check explicitly releases its RF lock on EXIT/timeout, Full Watch restarts with a fresh 100 ms dwell after any Messenger lock, and an idle channel change can re-arm the Messenger sidecar in the same 10 ms pass. Long ACK/PONG waits intentionally continue to hold scanning.
- Integrated the applicable late F4HWN fixes: erased battery-calibration protection, the BK4829 134.4 Hz DCS tail-enable bit, Full Watch navigation after a priority swap, compact Cortex-M0+ signed division, packed small fonts, and centralized byte-setting menu descriptors.
- Confirmed that the scan-start priority-swap reset, selected-TX/current-VFO synchronization, packed large font, and overlay external-Flash asset API v2 were already present in GOGUFW and retained them unchanged.
- FM Radio now releases its audio path while Messenger handles an incoming or outgoing FSK frame, then restores the FM tuner, audio route and FM screen after ACK/PONG processing completes.
- Removed the analog voice Scrambler from the firmware configuration, radio UI and matching CHIRP controls; the legacy EEPROM byte is cleared and retained only for layout compatibility.
- Removed Compander from the Fusion UI and runtime. The legacy EEPROM field remains only to preserve storage compatibility.
- Removed Spectrum's VFO `AUTO` mode; VFO Spectrum now starts with one calibration pass and continues in manual mode.
- Centralized Full Watch target resolution in `RADIO_SelectVfos()`. Messenger, Search Tone, Spectrum, normal PTT, CALLTX and channel-menu edits now follow the actually displayed A/B/C/D target instead of a stale physical A/B slot.
- Added an exact physical-channel RF lock for Messenger. Background TEXT/PING replies remain on the channel that received the packet through the complete ACK/PONG exchange, then release the lock and resume Full Watch with a fresh 100 ms dwell.
- Added a 250 ms post-carrier Messenger receive hold so Triple/Quad Watch does not rotate away during the intentional gap between a WAKE frame and its TEXT/PING frame. ACK/PONG reception uses the same stable target.
- Messenger Compose, Inbox Read, Sent Read and HEARD now show `TO:` plus the selected memory name or VFO frequency, so send, reply, resend and Range Check destinations are visible.
- Fixed Search under Full Watch: opening the overlay no longer stalls the radio, and Frequency/Tone Search starts from the displayed A/B/C/D channel even after repeated watch rotations. The corrected app is released as `Search-v1.2-gogu.zip` and requires this firmware's corrected append-only API layout.
- Fixed the overlay API layout so optional resident SysInfo fields no longer shift later API entries between builds. This restores APRS Edit launch/config access and preserves compatibility for API-level 2/3 apps. The corrected APRS editor is released as `APRSEdit-message-only-v1.1-gogu.zip`.
- Added API level 4's temporary private-frequency service. The bundled custom APRS RX/TX app sources use it to start on 144.800 MHz and restore the user's A/B/C/D target on exit; unchanged upstream Armel APRS apps continue to use the selected radio channel.
- Updated firmware and CHIRP version identifiers to 3.1.0.

### Build information

- Firmware: `f4hwn.gogufw.v3.1.0.bin`
- Multiboot image: 120,636 bytes
- Firmware Flash: 119,712 / 120,832 bytes (1,120 bytes free before the 924-byte multiboot stub; 196 bytes free in the final image)
- RAM: 13,744 / 16,384 bytes (2,640 bytes free)

## GOGUFW 2.3.3 — 2026-09-30

This release contains the changes made since GOGUFW 2.3.1. Version 2.3.2 was an internal development version and was not published as a stable release.

### FM radio and CHIRP

- Fixed FM memories created with CHIRP not appearing in FM memory mode after **Reset All** or on a fresh installation. The matching 2.3.3 CHIRP module now initializes an erased or inconsistent FM selected-frequency, selected-channel and band state from the uploaded FM memories while preserving a valid existing state.
- Fixed empty FM station-name fields being difficult or impossible to edit in CHIRP by disabling automatic space padding in the editor and applying the required flash padding only when saving.
- Preserved an already-valid FM-name header instead of reinitializing it for every edited name.
- During FM VFO scanning, the RSSI area continues to show five dashes while the tuner is stepping, then samples the found station at least once. With **LIVE RSSI** enabled, the meter continues to update while stopped on that station.
- Aligned the FM RSSI meter and `LIVE` label at the same one-pixel left inset.
- Corrected the right alignment of the `108.0` FM scale label.

### Keys and audio feedback

- Fixed locked **MENU** handling: the `UNLOCK KEYBOARD` notice now appears on the initial press, and holding MENU can no longer execute its assigned long-press action while the keypad is locked.
- Removed the duplicate optional beep that occurred when opening Messenger with **F + MENU**.
- On FM Radio, Messenger Home and HEARD/idle Range Check, **EXIT** now clears an active **F** state first; a second EXIT leaves the screen.

### Interface

- Matched the `TX BLOCKED` title typography and popup alignment to the `UNLOCK KEYBOARD` notice while retaining the smaller reason text.
- Vertically centred empty-state text in HEARD, Range Check, Inbox, Sent and Drafts.
- Moved the FSK icon one pixel left and the Roger icon one pixel right to improve spacing on the main radio screen.
- Inset custom-screen header counters, channel numbers, editor mode indicators and delivery markers by one pixel and aligned their baseline with the centred title.

### Radio compatibility

- Corrected BK4829 Scrambler enable/disable handling to match the known-good Sonic implementation and avoid modifying register `0x2B` unnecessarily.
- Messenger framing, message IDs, wake-preamble policy, ACK/retry behavior, ping/pong format and persistent Messenger data layout are unchanged from 2.3.1.

### Release files

- Firmware: `f4hwn.gogufw.v2.3.3.bin`
- CHIRP module: `Gogufw_2.3.3_chirp_module.py`
- Multiboot image size: 119,996 / 120,832 bytes (836 bytes free)
- Firmware FLASH usage: 119,076 / 120,832 bytes
- RAM usage: 13,520 / 16,384 bytes
