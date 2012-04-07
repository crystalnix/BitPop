// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/first_run/first_run.h"

#include "base/path_service.h"
#include "base/utf_string_conversions.h"
#include "chrome/browser/first_run/first_run_internal.h"
#include "chrome/browser/importer/importer_host.h"
#include "chrome/browser/importer/importer_list.h"
#include "chrome/browser/importer/importer_progress_dialog.h"
#include "chrome/browser/importer/importer_progress_observer.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/installer/util/master_preferences.h"
#include "chrome/installer/util/master_preferences_constants.h"

namespace {

// This class acts as an observer for the ImporterProgressObserver::ImportEnded
// callback. When the import process is started, certain errors may cause
// ImportEnded() to be called synchronously, but the typical case is that
// ImportEnded() is called asynchronously. Thus we have to handle both cases.
class ImportEndedObserver : public importer::ImporterProgressObserver {
 public:
  ImportEndedObserver() : ended_(false),
                          should_quit_message_loop_(false) {}
  virtual ~ImportEndedObserver() {}

  // importer::ImporterProgressObserver:
  virtual void ImportStarted() OVERRIDE {}
  virtual void ImportItemStarted(importer::ImportItem item) OVERRIDE {}
  virtual void ImportItemEnded(importer::ImportItem item) OVERRIDE {}
  virtual void ImportEnded() OVERRIDE {
    ended_ = true;
    if (should_quit_message_loop_)
      MessageLoop::current()->Quit();
  }

  void set_should_quit_message_loop() {
    should_quit_message_loop_ = true;
  }

  bool ended() {
    return ended_;
  }

 private:
  // Set if the import has ended.
  bool ended_;

  // Set by the client (via set_should_quit_message_loop) if, when the import
  // ends, this class should quit the message loop.
  bool should_quit_message_loop_;
};

}  // namespace

namespace first_run {
namespace internal {

bool GetFirstRunSentinelFilePath(FilePath* path) {
  FilePath first_run_sentinel;

  if (!PathService::Get(chrome::DIR_USER_DATA, &first_run_sentinel))
    return false;

  *path = first_run_sentinel.AppendASCII(kSentinelFile);
  return true;
}

bool ImportSettings(Profile* profile,
                    scoped_refptr<ImporterHost> importer_host,
                    scoped_refptr<ImporterList> importer_list,
                    int items_to_import) {
  const importer::SourceProfile& source_profile =
      importer_list->GetSourceProfileAt(0);

  // Ensure that importers aren't requested to import items that they do not
  // support.
  items_to_import &= source_profile.services_supported;

  scoped_ptr<ImportEndedObserver> observer(new ImportEndedObserver);
  importer_host->SetObserver(observer.get());
  importer_host->StartImportSettings(source_profile,
                                     profile,
                                     items_to_import,
                                     new ProfileWriter(profile),
                                     true);
  // If the import process has not errored out, block on it.
  if (!observer->ended()) {
    observer->set_should_quit_message_loop();
    MessageLoop::current()->Run();
  }

  // Unfortunately there's no success/fail signal in ImporterHost.
  return true;
}

}  // namespace internal
}  // namespace first_run

namespace first_run {

// TODO(port): Import switches need to be ported to both Mac and Linux. Not all
// import switches here are implemented for Linux. None are implemented for Mac
// (as this function will not be called on Mac).
int ImportNow(Profile* profile, const CommandLine& cmdline) {
  return internal::ImportBookmarkFromFileIfNeeded(profile, cmdline);
}


}  // namespace first_run
