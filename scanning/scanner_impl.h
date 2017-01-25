/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WIFICOND_SCANNER_IMPL_H_
#define WIFICOND_SCANNER_IMPL_H_

#include <vector>

#include <android-base/macros.h>
#include <binder/Status.h>

#include "android/net/wifi/BnWifiScannerImpl.h"
#include "wificond/net/netlink_utils.h"

namespace android {
namespace wificond {

class ScanUtils;

class ScannerImpl : public android::net::wifi::BnWifiScannerImpl {
 public:
  ScannerImpl(uint32_t interface_index,
              const BandInfo& band_info,
              const ScanCapabilities& scan_capabilities,
              const WiphyFeatures& wiphy_features,
              ScanUtils* scan_utils_);
  ~ScannerImpl();
  // Returns a vector of available frequencies for 2.4GHz channels.
  ::android::binder::Status getAvailable2gChannels(
      ::std::vector<int32_t>* out_frequencies) override;
  // Returns a vector of available frequencies for 5GHz non-DFS channels.
  ::android::binder::Status getAvailable5gNonDFSChannels(
      ::std::vector<int32_t>* out_frequencies) override;
  // Returns a vector of available frequencies for DFS channels.
  ::android::binder::Status getAvailableDFSChannels(
      ::std::vector<int32_t>* out_frequencies) override;
  // Get the latest scan results from kernel.
  ::android::binder::Status getScanResults(
      std::vector<com::android::server::wifi::wificond::NativeScanResult>*
          out_scan_results) override;
  ::android::binder::Status scan(
      const ::com::android::server::wifi::wificond::SingleScanSettings&
          scan_settings,
      bool* out_success) override;
  ::android::binder::Status startPnoScan(
      const ::com::android::server::wifi::wificond::PnoSettings& pno_settings,
      bool* out_success) override;
  ::android::binder::Status stopPnoScan(bool* out_success) override;

  ::android::binder::Status subscribeScanEvents(
      const ::android::sp<::android::net::wifi::IScanEvent>& handler) override;
  ::android::binder::Status unsubscribeScanEvents() override;
  ::android::binder::Status subscribePnoScanEvents(
      const ::android::sp<::android::net::wifi::IPnoScanEvent>& handler) override;
  ::android::binder::Status unsubscribePnoScanEvents() override;
  void Invalidate() { valid_ = false; }

 private:
  bool CheckIsValid();
  void OnScanResultsReady(
      uint32_t interface_index,
      bool aborted,
      std::vector<std::vector<uint8_t>>& ssids,
      std::vector<uint32_t>& frequencies);
  void OnSchedScanResultsReady(uint32_t interface_index, bool scan_stopped);


  // Boolean variables describing current scanner status.
  bool valid_;
  bool pno_scan_started_;

  uint32_t interface_index_;

  // Scanning relevant capability information for this wiphy/interface.
  const BandInfo band_info_;
  const ScanCapabilities scan_capabilities_;
  const WiphyFeatures wiphy_features_;


  ScanUtils* scan_utils_;
  ::android::sp<::android::net::wifi::IPnoScanEvent> pno_scan_event_handler_;
  ::android::sp<::android::net::wifi::IScanEvent> scan_event_handler_;

  DISALLOW_COPY_AND_ASSIGN(ScannerImpl);
};

}  // namespace wificond
}  // namespace android

#endif  // WIFICOND_SCANNER_IMPL_H_
