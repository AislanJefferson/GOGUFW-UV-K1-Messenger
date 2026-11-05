# Changelog

## Unreleased

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
  measured, and its column keeps showing their signal. Key 6 is ignored while
  MR Spectrum is sweeping (each channel's own bandwidth is applied there) but
  still changes the listening bandwidth while parked on a peak. MR sweeps no
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
