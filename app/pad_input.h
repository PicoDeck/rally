// Gamepad adapter for the app layer (no PicoDeck headers in core/).
// Firmware with the gamepad (api->version >= 9) gives the player's bindings
// (Settings -> Controls); older firmware has no gamepad table, so the keys
// the game always used are translated into the same PAD_* mask: F5 throttle
// (A), F4 brake (B), BACKSPACE handbrake (Y), arrows. The same mask feeds the
// sim either way. On the gamepad A (F4) is throttle and B (F5) brake.
#pragma once

#include <stdint.h>

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

// Name of the key a button is bound to, for on-screen hints.
static inline const char *pad_label(const PicoCalcAPI *api, uint32_t btn) {
    if (pad_has(api)) {
        const char *s = api->gamepad->getLabel(btn, 0);
        if (!s) s = api->gamepad->getLabel(btn, 1);
        return s ? s : "?";
    }
    return btn == PAD_A ? "F5" : btn == PAD_B ? "F4" : "?";
}
