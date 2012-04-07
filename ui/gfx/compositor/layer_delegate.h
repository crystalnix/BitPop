// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_COMPOSITOR_LAYER_DELEGATE_H_
#define UI_GFX_COMPOSITOR_LAYER_DELEGATE_H_
#pragma once

#include "ui/gfx/compositor/compositor_export.h"

namespace gfx {
class Canvas;
}

namespace ui {

// A delegate interface implemented by an object that renders to a Layer.
class COMPOSITOR_EXPORT LayerDelegate {
 public:
  // Paint content for the layer to the specified canvas. It has already been
  // clipped to the Layer's invalid rect.
  virtual void OnPaintLayer(gfx::Canvas* canvas) = 0;

 protected:
  virtual ~LayerDelegate() {}
};

}  // namespace ui

#endif  // UI_GFX_COMPOSITOR_LAYER_DELEGATE_H_
