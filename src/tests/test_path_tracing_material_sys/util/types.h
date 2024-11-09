//
// Created by jasonlannel on 24-10-19.
//

#ifndef TYPE_H
#define TYPE_H

#include <luisa/core/clock.h>
#include <luisa/core/logging.h>
#include <luisa/runtime/context.h>
#include <luisa/runtime/device.h>
#include <luisa/runtime/stream.h>
#include <luisa/runtime/event.h>
#include <luisa/runtime/swapchain.h>
#include <luisa/dsl/sugar.h>

#include "color.h"

using namespace luisa;
using namespace luisa::compute;
// Shader data
struct ShaderData {
  /* Position */
  Float3 P;
  /* Normal */
  Float3 N;
  /* True Geometric Normal */
  //Float3 Ng;
  /* view direction */
  Float3 wi;

  /* lcg state for random numbers */
  UInt lcg_state;
};

/*
typedef enum KernelType : int {
  INTERSECT,                      //Intersect with scene
  INTEGRATOR_SHADER_LIGHT,        //Evaluate spectrum for light
  INTEGRATOR_SHADER_SURFACE,      //Evaluate spectrum for surface
  INTEGRATOR_SHADER_BACKGROUND,   //Evaluate spectrum for background/environment

  KERNEL_TYPE_NUM
} KernelType;
*/

typedef enum ClosureType : int {
  /* Special type, flags generic node as a non-BSDF. */
  CLOSURE_NONE_ID,

  CLOSURE_BSDF_ID,

  /* Diffuse */
  CLOSURE_BSDF_DIFFUSE_ID,
} ClosureType;

typedef enum ShaderNodeType : int {
  NODE_END = 0,
  NODE_CLOSURE_BSDF,
  NODE_CLOSURE_EMISSION,
  NODE_CLOSURE_BACKGROUND,
  NODE_TEXTURE_COORD,
  NODE_TEXTURE_IMAGE,
  //NODE_NORMAL,
  //NODE_BUMP,
  //NODE_DISPLACEMENT,
  NODE_NUM
} ShaderNodeType;



struct BxDFSample {
  Float pdf;  // pdf for sampling
  Float3 wi;  // sampled direction for bounce
};


#endif //TYPE_H
