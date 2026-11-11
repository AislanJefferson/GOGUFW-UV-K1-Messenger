# Modified by AislanJefferson for KLEANFW, 2026 (see git history).
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Robust ARM GNU toolchain discovery for macOS/Linux and VS Code launched from GUI.
# Prefer PATH, then common Homebrew locations.
set(_ARM_TOOLCHAIN_HINTS
    /opt/homebrew/bin
    /usr/local/bin
    $ENV{HOME}/.local/bin
    $ENV{HOME}/arm-gnu-toolchain/bin
)

find_program(ARM_NONE_EABI_GCC NAMES arm-none-eabi-gcc HINTS ${_ARM_TOOLCHAIN_HINTS} REQUIRED)
find_program(ARM_NONE_EABI_GXX NAMES arm-none-eabi-g++ HINTS ${_ARM_TOOLCHAIN_HINTS} REQUIRED)
find_program(ARM_NONE_EABI_OBJCOPY NAMES arm-none-eabi-objcopy HINTS ${_ARM_TOOLCHAIN_HINTS} REQUIRED)
find_program(ARM_NONE_EABI_SIZE NAMES arm-none-eabi-size HINTS ${_ARM_TOOLCHAIN_HINTS} REQUIRED)

set(CMAKE_C_COMPILER   "${ARM_NONE_EABI_GCC}" CACHE FILEPATH "ARM GCC compiler" FORCE)
set(CMAKE_ASM_COMPILER "${ARM_NONE_EABI_GCC}" CACHE FILEPATH "ARM assembler" FORCE)
set(CMAKE_CXX_COMPILER "${ARM_NONE_EABI_GXX}" CACHE FILEPATH "ARM G++ compiler" FORCE)
set(CMAKE_OBJCOPY      "${ARM_NONE_EABI_OBJCOPY}" CACHE FILEPATH "ARM objcopy" FORCE)
set(CMAKE_SIZE         "${ARM_NONE_EABI_SIZE}" CACHE FILEPATH "ARM size" FORCE)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C   ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TARGET_FLAGS "-mcpu=cortex-m0plus ")

# CMake reads this file more than once; drop repeated flags so each one is
# passed a single time (the duplicates were harmless, only noisy).
function(_ggfw_unique_flags var)
    separate_arguments(_flags UNIX_COMMAND "${${var}}")
    list(REMOVE_DUPLICATES _flags)
    list(JOIN _flags " " _flags)
    set(${var} "${_flags}" PARENT_SCOPE)
endfunction()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
_ggfw_unique_flags(CMAKE_C_FLAGS)
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fdata-sections -ffunction-sections")
_ggfw_unique_flags(CMAKE_C_FLAGS)

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
# Measured size win with ARM GCC; retain RF regression testing when changed.
# -fno-tree-ch / -fno-ssa-phiopt only disable two optimization passes (loop
# header copying, PHI optimization): measured -276 B on the Fusion image.
# Not used: -fno-move-loop-invariants (-348 B) makes the bit-banged BK4829
# SPI ~1% slower per Spectrum scan step. Every pass disabled here is
# re-enabled for mb_flash.c (CMakeLists.txt).
set(CMAKE_C_FLAGS_RELEASE "-Oz -g0 -fno-inline-small-functions -fno-tree-ch -fno-ssa-phiopt -flto-partition=one")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
# No crt0/crti/crtbegin: Reset_Handler (Core/startup_py32f071xx.s) is the
# entry point and there are no static constructors or destructors, so the
# libc start files only add dead code (frame_dummy, __do_global_dtors_aux,
# _init/_fini). Measured -184 B, plus -76 B from dropping __libc_init_array.
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")
# Not used: -Wl,--sort-section=alignment (-100 B) moves byte arrays to odd
# addresses; code that casts them to wider structs then HardFaults (it froze
# every USB reply, e.g. the UV Studio handshake).
set(TOOLCHAIN_LINK_LIBRARIES "m")

set(CMAKE_EXE_LINKER_FLAGS_INIT
    "--specs=nosys.specs -Wl,--gc-sections -Wl,--no-warn-rwx-segment")
