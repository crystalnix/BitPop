// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Defines helper functions for all interfaces supported by the Native Client
// proxy.

#ifndef NATIVE_CLIENT_TESTS_PPAPI_TEST_PPB_TEMPLATE_GET_BROWSER_INTERFACE_H
#define NATIVE_CLIENT_TESTS_PPAPI_TEST_PPB_TEMPLATE_GET_BROWSER_INTERFACE_H

#include "ppapi/c/ppb_core.h"
#include "ppapi/c/ppb_file_io.h"
#include "ppapi/c/ppb_file_ref.h"
#include "ppapi/c/ppb_file_system.h"
#include "ppapi/c/ppb_fullscreen.h"
#include "ppapi/c/ppb_graphics_2d.h"
#include "ppapi/c/ppb_graphics_3d.h"
#include "ppapi/c/ppb_image_data.h"
#include "ppapi/c/ppb_input_event.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_opengles2.h"
#include "ppapi/c/ppb_url_loader.h"
#include "ppapi/c/ppb_url_request_info.h"
#include "ppapi/c/ppb_url_response_info.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppb_view.h"
#include "ppapi/c/dev/ppb_cursor_control_dev.h"
#include "ppapi/c/dev/ppb_font_dev.h"
#include "ppapi/c/dev/ppb_memory_dev.h"
#include "ppapi/c/dev/ppb_scrollbar_dev.h"
#include "ppapi/c/dev/ppb_testing_dev.h"
#include "ppapi/c/dev/ppb_widget_dev.h"

// Looks up the interface and returns its pointer or NULL.
const void* GetBrowserInterface(const char* interface_name);
// Uses GetBrowserInterface() and CHECKs for NULL.
const void* GetBrowserInterfaceSafe(const char* interface_name);

//
// Stable interfaces.
// Lookup guarantees that the interface is available by using NULL CHECKs.
//

const PPB_Core* PPBCore();
const PPB_FileIO* PPBFileIO();
const PPB_FileRef* PPBFileRef();
const PPB_FileSystem* PPBFileSystem();
const PPB_Fullscreen* PPBFullscreen();
const PPB_Graphics2D* PPBGraphics2D();
const PPB_Graphics3D* PPBGraphics3D();
const PPB_ImageData* PPBImageData();
const PPB_InputEvent* PPBInputEvent();
const PPB_Instance* PPBInstance();
const PPB_KeyboardInputEvent* PPBKeyboardInputEvent();
const PPB_Messaging* PPBMessaging();
const PPB_MouseInputEvent* PPBMouseInputEvent();
const PPB_OpenGLES2* PPBOpenGLES2();
const PPB_URLLoader* PPBURLLoader();
const PPB_URLRequestInfo* PPBURLRequestInfo();
const PPB_URLResponseInfo* PPBURLResponseInfo();
const PPB_Var* PPBVar();
const PPB_View* PPBView();
const PPB_WheelInputEvent* PPBWheelInputEvent();

//
// Experimental (aka Dev) interfaces.
// Lookup returns NULL if the interface is not available.
//

const PPB_CursorControl_Dev* PPBCursorControlDev();
const PPB_Font_Dev* PPBFontDev();
const PPB_Memory_Dev* PPBMemoryDev();
const PPB_Scrollbar_Dev* PPBScrollbarDev();
const PPB_Testing_Dev* PPBTestingDev();
const PPB_Widget_Dev* PPBWidgetDev();

#endif  // NATIVE_CLIENT_TESTS_PPAPI_TEST_PPB_TEMPLATE_GET_BROWSER_INTERFACE_H