//
// Created by jasonlannel on 24-10-18.
//

#ifndef SAMPLER_H
#define SAMPLER_H

#include <luisa/core/clock.h>
#include <luisa/core/logging.h>
#include <luisa/runtime/context.h>
#include <luisa/runtime/device.h>
#include <luisa/runtime/stream.h>
#include <luisa/runtime/event.h>
#include <luisa/runtime/swapchain.h>
#include <luisa/dsl/sugar.h>
#include <luisa/runtime/rtx/accel.h>
#include <stb/stb_image_write.h>
#include <luisa/gui/window.h>

using namespace luisa;
using namespace luisa::compute;

Float3 cosineSampleHemisphere (Float2 &rnd) noexcept {
    Float r = sqrt(rnd.x);
    Float phi = 2.0f * constants::pi * rnd.y;
    return make_float3(r * cos(phi), r * sin(phi), sqrt(1.0f - rnd.x));
}

#endif //SAMPLER_H
