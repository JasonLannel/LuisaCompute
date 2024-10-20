//
// Created by jasonlannel on 24-10-19.
//

#ifndef BSDF_DIFFUSE_H
#define BSDF_DIFFUSE_H

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

#include "../util/types.h"
#include "../util/sampler.h"

inline Spectrum evaluate_diffuse(Float3 wo, Float3 wi, Spectrum r) noexcept {
    Spectrum eval{};
    $if(wo.z * wi.z > 0) {
        eval = r * inv_pi;
    };
    return eval;
}

inline BxDFSample sample_diffuse(Float3 wo, Float2 u) noexcept {
    BxDFSample samp{};
    samp.wi = cosineSampleHemisphere(u);
    $if(wo.z < 0) {
        samp.wi.z *= -1;
    };
    samp.pdf = abs(samp.wi.z) * inv_pi;
    return samp;
}

#endif //BSDF_DIFFUSE_H
