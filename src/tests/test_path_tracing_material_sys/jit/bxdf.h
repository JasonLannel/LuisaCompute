//
// Created by jasonlannel on 24-10-14.
//
#ifndef TOY_BXDF_H
#define TOY_BXDF_H

#include <any>
#include <utility>

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
#include "../base/color.h"

using namespace luisa;
using namespace luisa::compute;

// Flags for BxDF, same as PBRT4
class BxDFFlags {
public:
    static constexpr auto Unset = 0;
    static constexpr auto Reflection = 1u << 0u;
    static constexpr auto Transmission = 1u << 1u;
    static constexpr auto Diffuse = 1u << 2u;
    //static constexpr auto Glossy = 1u << 3u;
    //static constexpr auto Specular = 1u << 4u;
    static constexpr auto DiffuseReflection = Diffuse | Reflection;
    static constexpr auto DiffuseTransmission = Diffuse | Transmission;
    //static constexpr auto GlossyReflection = Glossy | Reflection;
    //static constexpr auto GlossyTransmission = Glossy | Transmission;
    //static constexpr auto SpecularReflection = Specular | Reflection;
    //static constexpr auto SpecularTransmission = Specular | Transmission;
    //static constexpr auto All = Diffuse | Glossy | Specular | Reflection | Transmission;
};

struct BxDFSample {
    Float pdf{};
    Float3 wi;
    UInt flag{};
    #define TOY_CHECK_BXDF_FLAG(name)             \
    static Bool Is##name(UInt &&flag) noexcept {  \
        return flag & BxDFFlags::name;            \
    }
    TOY_CHECK_BXDF_FLAG(Unset)
    TOY_CHECK_BXDF_FLAG(Reflection)
    TOY_CHECK_BXDF_FLAG(Transmission)
    TOY_CHECK_BXDF_FLAG(Diffuse)
    //TOY_CHECK_BXDF_FLAG(Glossy)
    //TOY_CHECK_BXDF_FLAG(Specular)
    #undef TOY_CHECK_BXDF_FLAG
};

// Base BxDF
class BxDF {
public:
    BxDF() noexcept = default;
    virtual ~BxDF() noexcept = default;
    [[nodiscard]] virtual RGBSpectrum evaluate(Float3 wo, Float3 wi) const noexcept = 0;
    [[nodiscard]] virtual BxDFSample sample(Float3 wo, Float2 u) const noexcept = 0;
    [[nodiscard]] virtual Float pdf(Float3 wi) const noexcept = 0;
    [[nodiscard]] virtual UInt setFlag() const noexcept { return BxDFFlags::Unset; }
    
    template<typename T>
    void bind(T &&ctx) noexcept {
        if (!_context.has_value()) {
            _context = std::forward<T>(ctx);
        } else {
            auto p_data = std::any_cast<T>(&_context);
            assert(p_data != nullptr);
            *p_data = std::forward<T>(ctx);
        }
    }

    void unbind() noexcept {
        _context.reset();
    }

    template<typename T>
    [[nodiscard]] const T &context() const noexcept {
        auto ctx = std::any_cast<T>(&_context);
        assert(ctx != nullptr);
        return *ctx;
    }

private:
    std::any _context;
};

class DiffuseReflection final : public BxDF {
public:
    struct Context {
        RGBSpectrum r;
    };
    ~DiffuseReflection() noexcept override = default;
    [[nodiscard]] RGBSpectrum evaluate(Float3 wo, Float3 wi) const noexcept override {
        RGBSpectrum eval{};
        $if(wo.z * wi.z > 0) {
          eval = context<Context>().r * inv_pi;
        };
        return eval;
    }
    [[nodiscard]] BxDFSample sample(Float3 wo, Float2 u) const noexcept override {
        Callable cosine_sample_hemisphere = [](Float2 &rnd) noexcept {
            Float r = sqrt(rnd.x);
            Float phi = 2.0f * constants::pi * rnd.y;
            return make_float3(r * cos(phi), r * sin(phi), sqrt(1.0f - rnd.x));
        };
        BxDFSample samp{};
        samp.wi = cosine_sample_hemisphere(u);
        $if(wo.z < 0) {
            samp.wi.z *= -1;
        };
        samp.pdf = abs(samp.wi.z) * inv_pi;
        return samp;
    }
    [[nodiscard]] Float pdf(Float3 wi) const noexcept override {
        return abs(wi.z) * inv_pi;
    }
    [[nodiscard]] UInt setFlag() const noexcept override {
        return BxDFFlags::DiffuseReflection;
    }
};


class DiffuseTransmission final : public BxDF {
public:
    struct Context {
        RGBSpectrum t;
    };
    ~DiffuseTransmission() noexcept override = default;

    [[nodiscard]] RGBSpectrum evaluate(Float3 wo, Float3 wi) const noexcept override {
        RGBSpectrum eval{};
        $if(wo.z * wi.z < 0) {
            eval = context<Context>().t * inv_pi;
        };
        return eval;
    }
    [[nodiscard]] BxDFSample sample(Float3 wo, Float2 u) const noexcept override {
        Callable cosine_sample_hemisphere = [](Float2 &rnd) noexcept {
            Float r = sqrt(rnd.x);
            Float phi = 2.0f * constants::pi * rnd.y;
            return make_float3(r * cos(phi), r * sin(phi), sqrt(1.0f - rnd.x));
        };
        BxDFSample samp{};
        samp.wi = cosine_sample_hemisphere(u);
        $if(wo.z > 0) {
            samp.wi.z *= -1;
        };
        samp.pdf = abs(samp.wi.z) * inv_pi;
        return samp;
    }
    [[nodiscard]] Float pdf(Float3 wi) const noexcept override {
        return abs(wi.z) * inv_pi;
    }
    [[nodiscard]] UInt setFlag() const noexcept override {
        return BxDFFlags::DiffuseTransmission;
    }
};
/*
class BxDFEvaluator {
public:
    template<typename T = BxDF>
        requires std::derived_from<T, BxDF>
    void registerBasicBxDF() {
        #define TOY_REGISTER_BASIC_BXDF_MAPPING(type) \
            _bxdf_map.emplace(#type, _f.create<type>());
        TOY_REGISTER_BASIC_BXDF_MAPPING(DiffuseReflection)
        TOY_REGISTER_BASIC_BXDF_MAPPING(DiffuseTransmission)
        #undef TOY_REGISTER_BASIC_BXDF_MAPPING
    }
    auto evaluate(UInt bxdf_id, Float3 wo, Float3 wi) {
        RGBSpectrum eval;
        _f.dispatch(bxdf_id, [&](auto f) {
            //f->bind(_context);
            eval = f->evaluate(wo, wi);
            //f->unbind();
        });
        return eval;
    }
    auto sample(UInt bxdf_id, Float3 wo, Float2 u) {
        BxDFSample samp;
        _f.dispatch(bxdf_id, [&](auto f) {
            //f->bind(_context);
            samp = f->sample(wo, u);
            //f->unbind();
        });
        return samp;
    }
    auto pdf(UInt &bxdf_id, Float3 wi) {
        Float pdf;
        _f.dispatch(bxdf_id, [&](auto f) {
            //f->bind(_context);
            pdf = f->pdf(wi);
            //f->unbind();
        });
        return pdf;
    }
    
    template<typename T>
    void bind(T &&ctx) noexcept {
        if (!_context.has_value()) {
            _context = std::forward<T>(ctx);
        } else {
            auto p_data = std::any_cast<T>(&_context);
            assert(p_data != nullptr);
            *p_data = std::forward<T>(ctx);
        }
    }

    void unbind() noexcept { _context.reset(); }

private:
    Polymorphic<BxDF> _f;
    std::any _context;
    std::unordered_map<std::string, uint> _bxdf_map;
};
*/
#endif  // TOY_BXDF_H
