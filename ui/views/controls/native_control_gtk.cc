// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/controls/native_control_gtk.h"

#include <gtk/gtk.h>

#include "base/logging.h"
#include "ui/base/accessibility/accessibility_types.h"
#include "ui/views/focus/focus_manager.h"
#include "ui/views/widget/widget.h"

namespace views {

NativeControlGtk::NativeControlGtk() {
}

NativeControlGtk::~NativeControlGtk() {
  if (native_view())
    gtk_widget_destroy(native_view());
}

////////////////////////////////////////////////////////////////////////////////
// NativeControlGtk, View overrides:

void NativeControlGtk::OnEnabledChanged() {
  View::OnEnabledChanged();
  if (native_view())
    gtk_widget_set_sensitive(native_view(), enabled());
}

void NativeControlGtk::ViewHierarchyChanged(bool is_add, View* parent,
                                            View* child) {
  // Call the base class to hide the view if we're being removed.
  NativeViewHost::ViewHierarchyChanged(is_add, parent, child);

  if (!is_add && child == this && native_view()) {
    Detach();
  } else if (is_add && GetWidget() && !native_view()) {
    // Create the widget when we're added to a valid Widget. Many
    // controls need a parent widget to function properly.
    CreateNativeControl();
  }
}

void NativeControlGtk::VisibilityChanged(View* starting_from, bool is_visible) {
  if (!native_view()) {
    if (GetWidget())
      CreateNativeControl();
  } else {
    // The view becomes visible after native control is created.
    // Layout now.
    Layout();
  }
}

void NativeControlGtk::OnFocus() {
  DCHECK(native_view());
  gtk_widget_grab_focus(native_view());
  GetWidget()->NotifyAccessibilityEvent(
      parent(), ui::AccessibilityTypes::EVENT_FOCUS, true);
}

void NativeControlGtk::NativeControlCreated(GtkWidget* native_control) {
  Attach(native_control);

  // Update the newly created GtkWidget with any resident enabled state.
  gtk_widget_set_sensitive(native_view(), enabled());

  // Listen for focus change event to update the FocusManager focused view.
  g_signal_connect(native_control, "focus-in-event",
                   G_CALLBACK(CallFocusIn), this);
}

// static
gboolean NativeControlGtk::CallFocusIn(GtkWidget* gtk_widget,
                                       GdkEventFocus* event,
                                       NativeControlGtk* control) {
  Widget* widget = Widget::GetTopLevelWidgetForNativeView(gtk_widget);
  FocusManager* focus_manager = widget ? widget->GetFocusManager() : NULL;
  if (!focus_manager) {
    // TODO(jcampan): http://crbug.com/21378 Reenable this NOTREACHED() when the
    // options page is only based on views.
    // NOTREACHED();
    NOTIMPLEMENTED();
    return false;
  }
  focus_manager->SetFocusedView(control->focus_view());
  return false;
}

}  // namespace views