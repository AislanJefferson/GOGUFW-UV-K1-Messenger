#define PRINTF_DISABLE_SUPPORT_LONG_LONG
#define PRINTF_DISABLE_SUPPORT_EXPONENTIAL
#define PRINTF_DISABLE_SUPPORT_PTRDIFF_T
#define PRINTF_DISABLE_SUPPORT_FLOAT
// KLEANFW: no '+' and '#' flags, length modifiers (l, h, j, z) or the %i,
// %x, %b and %p conversions (-408 B). No sprintf/snprintf format in the
// firmware uses them: only d u s c X o %, the '0', '-' and ' ' flags, a
// numeric or '*' width and precision. Check new formats against this list.
// Debug builds keep them all.
#ifndef DEBUG
#define PRINTF_DISABLE_SUPPORT_RARE
#endif
