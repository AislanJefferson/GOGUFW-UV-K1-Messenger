# KLEANFW - native build on macOS (no Docker)

## Requirements
- CMake (3.22 or newer)
- Ninja
- ARM GNU embedded toolchain (`arm-none-eabi-gcc`)
- Optional: VS Code with the CMake Tools extension

The included toolchain file searches both PATH and common Homebrew locations:
- `/opt/homebrew/bin` (Apple Silicon)
- `/usr/local/bin` (Intel/Homebrew)

## Build

```bash
cmake --preset Fusion
cmake --build --preset Fusion
```

With CMake Tools, select the `Fusion` configure preset and run **CMake: Build**.

## Clean build

```bash
rm -rf build/Fusion
cmake --preset Fusion
cmake --build --preset Fusion
```

## Important
- Do not copy an old `build/` directory between machines or project folders.
  CMake caches absolute paths.
- The release images are built with the Docker toolchain
  (`./compile-with-docker.sh`). A different GCC version can produce a different
  size, and the image must not exceed 120,832 bytes.

The generated firmware files are written to `build/Fusion/` with the `kleanfw`
target name.
