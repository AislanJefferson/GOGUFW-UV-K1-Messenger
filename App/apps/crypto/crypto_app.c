/* Crypto - editor for the shared CryKey passphrase (up to 10 characters).
 *
 * The firmware derives the HOP voice-scrambler key and the encrypted
 * Messenger key from this app's 16-byte config. Without this app, or with an
 * empty passphrase, every radio uses the same default key: HOP still works
 * between them and Messenger text stays in clear.
 *
 * Keys: 0-9 multi-tap (hold for the digit) · STAR mode · UP/DOWN cursor ·
 * F delete · MENU save · EXIT quit without saving. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../app_api.h"

#define KEY_LEN   10u
#define CFG_LEN   16u
#define LOOP_MS   10u
#define LONG_MS   400u

static const app_api_t *A;
static const gogu_api_t *G;
static struct {
    char    text[KEY_LEN + 1u];
    uint8_t pos, mode, tick;
    bool    running, dirty;
} g;

void *memset(void *dst, int value, size_t n)
{
    uint8_t *d = dst;
    while (n--) *d++ = (uint8_t)value;
    return dst;
}

static void editor(uint8_t key, uint8_t action)
{
    const uint16_t s = G->text_editor(g.text, KEY_LEN, key, action);
    if ((uint8_t)s != g.pos || (uint8_t)(s >> 8) != g.mode || action != APP_TEXT_TICK_50MS)
        g.dirty = true;
    g.pos  = (uint8_t)s;
    g.mode = (uint8_t)(s >> 8);
}

static void draw(void)
{
    char mode[4] = {'A', 'B', 'C', '\0'};
    if (g.mode == 2u) { mode[0] = '1'; mode[1] = '2'; mode[2] = '3'; }
    else if (g.mode == 1u) { mode[0] = 'a'; mode[1] = 'b'; mode[2] = 'c'; }
    A->display_clear();
    A->status_clear();
    A->draw_battery();
    G->set_text_editor_cursor(g.pos);
    G->draw_text_editor("CRYKEY", g.text, KEY_LEN, "SAVE", mode, false, "F:DEL");
    A->blit_status();
    A->blit_full();
}

static void save(void)
{
    uint8_t cfg[CFG_LEN], len = 0u;
    memset(cfg, 0, sizeof(cfg));
    while (len < KEY_LEN && g.text[len]) len++;
    while (len && g.text[len - 1u] == ' ') len--;   // trailing spaces are padding
    for (uint8_t i = 0u; i < len; i++) cfg[i] = (uint8_t)g.text[i];
    A->cfg_save(cfg, sizeof(cfg));                  // committed when the app exits
}

static void on_short(uint8_t key)
{
    if (key == APP_KEY_MENU) { save(); g.running = false; }
    else if (key == APP_KEY_EXIT) g.running = false;
    else editor(key, APP_TEXT_SHORT);              // digits, STAR, F, UP, DOWN
}

__attribute__((section(".text.entry"), used))
void app_main(const app_api_t *api)
{
    uint8_t cfg[CFG_LEN];
    uint8_t held = APP_KEY_INVALID;
    uint16_t held_ms = 0u;
    bool fired = false, wait_release = true;

    A = api;
    G = APP_GoguApi(api);
    if (!G || G->level < 2u || !G->text_editor || !G->set_text_editor_cursor) return;

    api->cfg_load(cfg, sizeof(cfg));
    for (uint8_t i = 0u; i < KEY_LEN && cfg[i] && cfg[i] != 0xFFu; i++)
        g.text[i] = (char)cfg[i];
    editor(APP_KEY_INVALID, APP_TEXT_START);
    api->backlight_on();
    g.running = true;

    while (g.running) {
        const uint8_t key = A->get_key();
        if (wait_release) {                          // ignore the launching key
            if (key == APP_KEY_INVALID) wait_release = false;
        } else if (key == APP_KEY_SAVER || key == APP_KEY_WAKE || key == APP_KEY_PTT) {
            held = APP_KEY_INVALID; fired = false; g.dirty = true;
        } else if (key == APP_KEY_INVALID) {
            if (held != APP_KEY_INVALID && !fired) on_short(held);
            held = APP_KEY_INVALID; fired = false;
        } else if (key != held) {
            held = key; held_ms = 0u; fired = false;
            A->backlight_on();
        } else if (!fired && key <= APP_KEY_9 && (held_ms += LOOP_MS) >= LONG_MS) {
            fired = true;
            editor(key, APP_TEXT_LONG);
        }
        if (++g.tick >= 5u) { g.tick = 0u; editor(APP_KEY_INVALID, APP_TEXT_TICK_50MS); }
        if (g.dirty) { g.dirty = false; draw(); }
        A->delay_ms(LOOP_MS);
        A->backlight_update();
    }
}
