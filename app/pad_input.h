// Gamepad adapter for the app layer (no PicoDeck headers in core/).
// Firmware with the gamepad (api->version >= 9) gives the player's bindings
// (Settings -> Controls); older firmware has no gamepad table, so the keys
// the game always used are translated into the same PAD_* mask: F5 throttle
// (A), F4 brake (B), BACKSPACE handbrake (Y), arrows. The same mask feeds the
// sim either way. On the gamepad A (F4) is throttle and B (F5) brake.
#pragma once

#include <stdint.h>
#include <strings.h>

#include "os.h"

static inline int pad_has(const PicoCalcAPI *api) {
    return api->version >= 9 && api->gamepad;
}

static inline uint32_t pad_from_keys(uint32_t keys) {
    uint32_t pad = 0;
    if (keys & BTN_UP)        pad |= PAD_UP;
    if (keys & BTN_DOWN)      pad |= PAD_DOWN;
    if (keys & BTN_LEFT)      pad |= PAD_LEFT;
    if (keys & BTN_RIGHT)     pad |= PAD_RIGHT;
    if (keys & BTN_F5)        pad |= PAD_A;
    if (keys & BTN_F4)        pad |= PAD_B;
    if (keys & BTN_BACKSPACE) pad |= PAD_Y;
    if (keys & BTN_F1)        pad |= PAD_START;
    return pad;
}

static inline uint32_t pad_held(const PicoCalcAPI *api) {
    return pad_has(api) ? api->gamepad->getButtons()
                        : pad_from_keys(api->input->getButtons());
}

static inline uint32_t pad_pressed(const PicoCalcAPI *api) {
    return pad_has(api) ? api->gamepad->getButtonsPressed()
                        : pad_from_keys(api->input->getButtonsPressed());
}

// The buttons Rally reads. Only these can shadow a dev shortcut: the default
// bindings put L on F2 and R on F3 and the game never reads them.
#define PAD_RALLY_USED (PAD_A | PAD_B | PAD_Y | PAD_START | PAD_LEFT | PAD_RIGHT)

// Is the key `name` ("F3", "R", ...) bound to a gamepad button the game reads?
// Such a key is a pad button, not a dev shortcut: firmware still reports the
// key itself (and its char), so the app must ignore it there.
static inline int pad_key_bound(const PicoCalcAPI *api, const char *name) {
    if (!pad_has(api)) return 0;
    for (int b = 0; b < 12; b++) {
        if (!((1u << b) & PAD_RALLY_USED)) continue;
        for (int slot = 0; slot < 2; slot++) {
            const char *l = api->gamepad->getLabel(1u << b, slot);
            if (l && strcasecmp(l, name) == 0) return 1;
        }
    }
    return 0;
}

// Name of the key a button is bound to, for on-screen hints.
static inline const char *pad_label(const PicoCalcAPI *api, uint32_t btn) {
    if (pad_has(api)) {
        const char *s = api->gamepad->getLabel(btn, 0);
        if (!s) s = api->gamepad->getLabel(btn, 1);
        if (!s && btn == PAD_A) {   // A unbound: Start also begins and retries
            s = api->gamepad->getLabel(PAD_START, 0);
            if (!s) s = api->gamepad->getLabel(PAD_START, 1);
        }
        return s ? s : "?";
    }
    return btn == PAD_A ? "F5" : btn == PAD_B ? "F4" : "?";
}
