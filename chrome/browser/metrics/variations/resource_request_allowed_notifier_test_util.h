// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_METRICS_VARIATIONS_RESOURCE_REQUEST_ALLOWED_NOTIFIER_TEST_UTIL_H_
#define CHROME_BROWSER_METRICS_VARIATIONS_RESOURCE_REQUEST_ALLOWED_NOTIFIER_TEST_UTIL_H_

#include "chrome/browser/metrics/variations/resource_request_allowed_notifier.h"

// A subclass of ResourceRequestAllowedNotifier used to expose some
// functionality for testing.
//
// By default, the constructor sets this class to override
// ResourceRequestsAllowed, so its state can be set with SetRequestsAllowed.
// This is meant for higher level tests of services to ensure they adhere to the
// notifications of the ResourceRequestAllowedNotifier. Lower level tests can
// disable this by calling SetRequestsAllowedOverride with the value they want
// it to return.
class TestRequestAllowedNotifier : public ResourceRequestAllowedNotifier {
 public:
  TestRequestAllowedNotifier();
  virtual ~TestRequestAllowedNotifier();

#if defined(OS_CHROMEOS)
  void SetNeedsEulaAcceptance(bool needs_acceptance);
#endif

  // Makes ResourceRequestsAllowed return |allowed| when it is called.
  void SetRequestsAllowedOverride(bool allowed);

  // Notify observers that requests are allowed. This will only work if
  // the observer is expecting a notification.
  void NotifyObserver();

  virtual bool ResourceRequestsAllowed() OVERRIDE;

 protected:
#if defined(OS_CHROMEOS)
  virtual bool NeedsEulaAcceptance() OVERRIDE;
#endif

 private:
#if defined(OS_CHROMEOS)
  bool needs_eula_acceptance_;
#endif
  bool override_requests_allowed_;
  bool requests_allowed_;

  DISALLOW_COPY_AND_ASSIGN(TestRequestAllowedNotifier);
};

#endif  // CHROME_BROWSER_METRICS_VARIATIONS_RESOURCE_REQUEST_ALLOWED_NOTIFIER_TEST_UTIL_H_
