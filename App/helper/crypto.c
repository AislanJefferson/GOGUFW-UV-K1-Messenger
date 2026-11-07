#include <string.h>

#include "helper/crypto.h"

#ifdef ENABLE_GOGUFW_CRYPTO

#include "driver/py25q16.h"

/* .noncacheable is NOLOAD: initialised by CRYPTO_Reload() at boot. */
static uint32_t sKey[4]   __attribute__((section(".noncacheable")));
static bool     sKeyValid __attribute__((section(".noncacheable")));

static void xtea(uint32_t v[2], const uint32_t k[4])
{
    uint32_t v0 = v[0], v1 = v[1], sum = 0;

    for (uint8_t i = 0; i < 32u; i++) {
        v0  += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3u]);
        sum += 0x9E3779B9u;
        v1  += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3u]);
    }
    v[0] = v0;
    v[1] = v1;
}

void CRYPTO_ReadText(char out[CRYPTO_KEY_TEXT_LEN + 1])
{
    PY25Q16_ReadBuffer(CRYPTO_KEY_FLASH_ADDR, out, CRYPTO_KEY_TEXT_LEN);
    out[CRYPTO_KEY_TEXT_LEN] = 0;
    /* Text ends at the first NUL or erased (0xFF) byte (never set = empty
     * passphrase); zero the rest so the derived key is well defined. */
    bool end = false;
    for (uint8_t i = 0; i < CRYPTO_KEY_TEXT_LEN; i++) {
        if (out[i] == 0 || (uint8_t)out[i] == 0xFFu)
            end = true;
        if (end)
            out[i] = 0;
    }
}

void CRYPTO_Reload(void)
{
    /* Key = XTEA(fixed, passphrase padded to 16 bytes), one block per half. */
    static const uint32_t kdf[4] = {0x474F4755u, 0x46572D4Bu, 0x45592D31u, 0x2E302E30u};
    char text[16];

    memset(text, 0, sizeof(text));
    CRYPTO_ReadText(text);
    sKeyValid = text[0] != 0;

    memcpy(sKey, text, sizeof(sKey));
    xtea(&sKey[0], kdf);
    xtea(&sKey[2], kdf);
}

void CRYPTO_SaveText(const char *text)
{
    char buf[CRYPTO_KEY_TEXT_LEN];
    uint8_t len = 0;

    while (len < CRYPTO_KEY_TEXT_LEN && text[len])
        len++;
    while (len && text[len - 1u] == ' ')   // trailing spaces are editor padding
        len--;
    memset(buf, 0, sizeof(buf));
    memcpy(buf, text, len);
    PY25Q16_WriteBuffer(CRYPTO_KEY_FLASH_ADDR, buf, sizeof(buf), false);
    CRYPTO_Reload();
}

bool CRYPTO_HasKey(void)
{
    return sKeyValid;
}

uint32_t CRYPTO_Word(uint32_t a, uint32_t b)
{
    uint32_t v[2] = {a, b};
    xtea(v, sKey);
    return v[0];
}

void CRYPTO_Ctr(uint32_t n0, uint32_t n1, uint8_t *buf, uint8_t len)
{
    uint32_t v[2];

    for (uint8_t i = 0; i < len; i++) {
        if ((i & 7u) == 0u) {
            v[0] = n0;
            v[1] = n1 + (i >> 3);
            xtea(v, sKey);
        }
        buf[i] ^= ((const uint8_t *)v)[i & 7u];
    }
}

#endif
