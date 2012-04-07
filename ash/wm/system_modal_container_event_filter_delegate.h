// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_WM_MODALITY_EVENT_FILTER_DELEGATE_H_
#define ASH_WM_MODALITY_EVENT_FILTER_DELEGATE_H_
#pragma once

#include "ash/ash_export.h"

namespace aura {
class Window;
}

namespace ash {
namespace internal {

class ASH_EXPORT SystemModalContainerEventFilterDelegate {
 public:
  // Returns true if |window| can receive the specified event.
  virtual bool CanWindowReceiveEvents(aura::Window* window) = 0;
};

}  // namespace internal
}  // namespace ash

#endif  // ASH_WM_MODALITY_EVENT_FILTER_DELEGATE_H_
