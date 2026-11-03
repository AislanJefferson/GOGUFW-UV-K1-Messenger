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
/* The passphrase is set with the "Crypto" overlay app (its 16-byte config).
 * Without the app, or with an empty passphrase, every radio derives the same
 * default key: HOP still interoperates and Messenger text stays in clear. */

void     CRYPTO_Reload(void);
bool     CRYPTO_HasKey(void);
/* First word of XTEA(key, a || b). */
uint32_t CRYPTO_Word(uint32_t a, uint32_t b);
/* XOR buf with the CTR keystream XTEA(key, n0 || n1 + block). */
void     CRYPTO_Ctr(uint32_t n0, uint32_t n1, uint8_t *buf, uint8_t len);
#endif

#endif
