// Copyright (c) 2012 House of Life Property Ltd. All rights reserved.
// Copyright (c) 2012 Crystalnix <vgachkaylo@crystalnix.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_OPTIONS2_BITPOP_PROXY_DOMAIN_SETTINGS_HANDLER_H_
#define CHROME_BROWSER_UI_WEBUI_OPTIONS2_BITPOP_PROXY_DOMAIN_SETTINGS_HANDLER_H_

#include "chrome/browser/ui/webui/options2/bitpop_options_ui.h"

class KeywordEditorController;

namespace extensions {
class Extension;
}

namespace options2 {

class BitpopProxyDomainSettingsHandler : public BitpopOptionsPageUIHandler {
 public:
  BitpopProxyDomainSettingsHandler();
  virtual ~BitpopProxyDomainSettingsHandler();

  // OptionsPageUIHandler implementation.
  virtual void GetLocalizedValues(
      base::DictionaryValue* localized_strings) OVERRIDE;
  virtual void InitializeHandler() OVERRIDE;
  virtual void InitializePage() OVERRIDE;

  virtual void RegisterMessages() OVERRIDE;

 private:
  void OnUpdateDomains(const base::ListValue* params);
  void ChangeSiteList(const base::ListValue* params);

  DISALLOW_COPY_AND_ASSIGN(BitpopProxyDomainSettingsHandler);
};

}  // namespace options2

#endif  // CHROME_BROWSER_UI_WEBUI_OPTIONS2_BITPOP_PROXY_DOMAIN_SETTINGS_HANDLER_H_
