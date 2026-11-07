/* GOGUFW shared-secret helpers: XTEA block cipher (encrypt only), CTR
 * keystream and the "CryKey" passphrase used by the encrypted Messenger and
 * the HOP voice scrambler. */
#ifndef HELPER_CRYPTO_H
#define HELPER_CRYPTO_H

#include <stdbool.h>
#include <stdint.h>

#if defined(ENABLE_MESSENGER) && defined(ENABLE_GOGUFW_SCRAMBLER)
#define ENABLE_GOGUFW_CRYPTO 1

#define CRYPTO_KEY_TEXT_LEN   10u
/* The passphrase is set in the CryKey menu and stored right after the
 * Messenger config (320 B) in its sector. With an empty passphrase every radio
 * derives the same default key: HOP still interoperates and Messenger text
 * stays in clear. */
#define CRYPTO_KEY_FLASH_ADDR (0x012000u + 320u)

void     CRYPTO_Reload(void);
/* Stored passphrase, NUL-padded to CRYPTO_KEY_TEXT_LEN + 1 bytes. */
void     CRYPTO_ReadText(char out[CRYPTO_KEY_TEXT_LEN + 1]);
/* Store a passphrase (trailing spaces dropped) and reload the key. */
void     CRYPTO_SaveText(const char *text);
bool     CRYPTO_HasKey(void);
/* First word of XTEA(key, a || b). */
uint32_t CRYPTO_Word(uint32_t a, uint32_t b);
/* XOR buf with the CTR keystream XTEA(key, n0 || n1 + block). */
void     CRYPTO_Ctr(uint32_t n0, uint32_t n1, uint8_t *buf, uint8_t len);
#endif

#endif
