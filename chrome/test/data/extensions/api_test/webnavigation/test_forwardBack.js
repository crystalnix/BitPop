// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function runTests() {
  var getURL = chrome.extension.getURL;
  chrome.tabs.create({"url": "about:blank"}, function(tab) {
    var tabId = tab.id;

    chrome.test.runTests([
      // First navigates to a.html which redirects to to b.html which uses
      // history.back() to navigate back to a.html
      function forwardBack() {
        expect([
          { label: "a-onBeforeNavigate",
            event: "onBeforeNavigate",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       url: getURL('forwardBack/a.html') }},
          { label: "a-onCommitted",
            event: "onCommitted",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       transitionQualifiers: [],
                       transitionType: "link",
                       url: getURL('forwardBack/a.html') }},
          { label: "a-onDOMContentLoaded",
            event: "onDOMContentLoaded",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       url: getURL('forwardBack/a.html') }},
          { label: "a-onCompleted",
            event: "onCompleted",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       url: getURL('forwardBack/a.html') }},
          { label: "b-onBeforeNavigate",
            event: "onBeforeNavigate",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       url: getURL('forwardBack/b.html') }},
          { label: "b-onCommitted",
            event: "onCommitted",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       transitionQualifiers: ["client_redirect"],
                       transitionType: "link",
                       url: getURL('forwardBack/b.html') }},
          { label: "b-onDOMContentLoaded",
            event: "onDOMContentLoaded",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       url: getURL('forwardBack/b.html') }},
          { label: "b-onCompleted",
            event: "onCompleted",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       url: getURL('forwardBack/b.html') }},
          { label: "c-onBeforeNavigate",
            event: "onBeforeNavigate",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       url: getURL('forwardBack/a.html') }},
          { label: "c-onCommitted",
            event: "onCommitted",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       transitionQualifiers: ["forward_back"],
                       transitionType: "link",
                       url: getURL('forwardBack/a.html') }},
          { label: "c-onDOMContentLoaded",
            event: "onDOMContentLoaded",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       url: getURL('forwardBack/a.html') }},
          { label: "c-onCompleted",
            event: "onCompleted",
            details: { frameId: 0,
                       tabId: 0,
                       timeStamp: 0,
                       url: getURL('forwardBack/a.html') }}],
          [ navigationOrder("a-"), navigationOrder("b-"), navigationOrder("c-"),
            isLoadedBy("b-", "a-"), isLoadedBy("c-", "b-")]);
        chrome.tabs.update(tabId, { url: getURL('forwardBack/a.html') });
      },
    ]);
  });
}
