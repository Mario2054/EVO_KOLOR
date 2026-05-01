#pragma once
#include <Arduino.h>

class DlnaDescription {
public:
  // descUrl = pl. http://IP:PORT/desc/device.xml
  bool resolveControlURL(const String& descUrl, String& outControlUrl);

  // Probe well-known DLNA HTTP ports when SSDP discovery fails.
  // Returns true and sets outDescUrl if a valid ContentDirectory is found.
  bool probeDescriptionUrl(const String& host, String& outDescUrl);

private:
  bool parseStream(Stream& s, String& outControlPath);
  bool parseString(const String& xml, String& controlPath);
  String extractBaseUrl(const String& fullUrl);
};
