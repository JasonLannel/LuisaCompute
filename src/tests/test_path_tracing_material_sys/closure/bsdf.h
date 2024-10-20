//
// Created by jasonlannel on 24-10-18.
//

#ifndef BXDF_H
#define BXDF_H

#include "bsdf_diffuse.h"

#define CASE_CLOSURE_BSDF(name)     \
    $case(static_cast<int>(name))
#define CASE_CLOSURE_BSDF_DEFAULT   \
    $default

//TODO: Input ShaderData Instead;
inline Spectrum evaluate_bsdf(Float3 wo, Float3 wi, Spectrum r, Int type) noexcept {
    Spectrum eval;
    $switch(type){
        CASE_CLOSURE_BSDF(CLOSURE_BSDF_DIFFUSE_ID) {
            eval = evaluate_diffuse(wo, wi, r);
        };
        CASE_CLOSURE_BSDF_DEFAULT {
            eval = evaluate_diffuse(wo, wi, r);
        };
    };
    return eval;
}

inline BxDFSample sample_bsdf(Float3 wo, Float2 u, Int type) noexcept {
    BxDFSample sample;
    $switch(type){
        CASE_CLOSURE_BSDF(CLOSURE_BSDF_DIFFUSE_ID) {
            sample = sample_diffuse(wo, u);
        };
        CASE_CLOSURE_BSDF_DEFAULT {
            sample = sample_diffuse(wo, u);
        };
    };
    return sample;
}


#endif //BXDF_H
