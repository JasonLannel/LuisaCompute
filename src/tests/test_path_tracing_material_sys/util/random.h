//
// Created by jasonlannel on 24-10-19.
//

#ifndef RANDOM_H
#define RANDOM_H

#include <luisa/core/logging.h>
#include <luisa/runtime/context.h>
#include <luisa/runtime/device.h>
#include <luisa/runtime/stream.h>
#include <luisa/runtime/event.h>
#include <luisa/runtime/swapchain.h>
#include <luisa/dsl/sugar.h>

using namespace luisa;
using namespace luisa::compute;

Float lcg(UInt &state) noexcept {
    constexpr uint lcg_a = 1664525u;
    constexpr uint lcg_c = 1013904223u;
    state = lcg_a * state + lcg_c;
    return cast<float>(state & 0x00ffffffu) *
           (1.0f / static_cast<float>(0x01000000u));
}

#endif //RANDOM_H
