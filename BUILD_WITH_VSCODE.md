# Build KLEANFW with VS Code and Docker

Docker is enough for the firmware build. VS Code is only a convenient front-end.

## Install once

1. Install **Visual Studio Code**.
2. Install and start **Docker** (Docker Desktop on Windows/macOS).
3. Optional VS Code extensions: C/C++ and CMake Tools by Microsoft.

## Build

1. Open the folder that contains `compile-with-docker.sh` (**File > Open Folder...**).
2. Open a terminal (**Terminal > New Terminal**) and run:

```bash
./compile-with-docker.sh   # firmware only
./build-all.sh             # firmware + overlay apps + CHIRP module check
```

The first run builds the `uvk1-uvk5v3` Docker image. Later builds take a few
seconds.

## Output files

- `build/Fusion/kleanfw.bin` and `kleanfw.hex`: firmware image for normal flashing;
- `build/Fusion/f4hwn.kleanfw.<version>.bin`: the same image under the
  multiboot file name;
- `build/dist/`: everything collected by `build-all.sh` (firmware, `Apps/*.app`,
  CHIRP module).

The firmware `.bin` must not exceed 120,832 bytes; `build-all.sh` checks it.

The CMake preset is still named `Fusion` because KLEANFW keeps the F4HWN Fusion
feature set through GOGUFW. SystemInfo and the multiboot menu show `KLEANFW v<version>`.
