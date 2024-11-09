//
// Created by jasonlannel on 24-10-27.
//

#ifndef MATERIAL_H
#define MATERIAL_H

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

#include "../util/color.h"

#include <any>

using namespace luisa;
using namespace luisa::compute;

class Material {
  Material() noexcept = default;
  virtual ~Material() noexcept = default;
  [[nodiscard]] virtual Spectrum evaluate(Float3 wo, Float3 wi) const noexcept = 0;
  [[nodiscard]] virtual BxDFSample sample(Float3 wo, Float2 u) const noexcept = 0;

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

#endif //MATERIAL_H
