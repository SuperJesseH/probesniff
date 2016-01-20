#include "ESP8266WiFi.h"
extern "C" {
#include "user_interface.h"
}
#include "network_80211.h"

void handle_pkt(uint8_t *buf, uint16 len) {
  // Serial.println("packet, len " + String(len));
  uint8_t type;

  lpframectrl_80211 framectrl;
  struct router_info *info = NULL;
  if (len < 64) {
    return;
  }
  struct sniffer_buf *sniffer = (struct sniffer_buf*)buf;
  buf += sizeof(struct RxControl);
  struct probe_request_80211 *probe_buf = (struct probe_request_80211*)buf;


  if (FRAME_TYPE_MANAGEMENT == probe_buf->framectrl.Type) {
   
    /* Management frame */
    if (FRAME_SUBTYPE_PROBE_REQUEST == probe_buf->framectrl.Subtype) {
      uint8* tsaddr = probe_buf->tsaddr;
      String addr = String(tsaddr[0], HEX) + ":" +
                    String(tsaddr[1], HEX) + ":" +
                    String(tsaddr[2], HEX) + ":" +
                    String(tsaddr[3], HEX) + ":" +
                    String(tsaddr[4], HEX) + ":" +
                    String(tsaddr[5], HEX) ; 
   
      /* Probe Request */
      ptagged_parameter tag = (ptagged_parameter)(buf + sizeof(probe_request));
      if (tag->tag_length != 0)
      {
        char ssid_buff[32];
        os_memset(ssid_buff, 0, 32);
        os_memcpy(ssid_buff, (uint8_t *)tag + 2, tag->tag_length);
        Serial.print("Probe request from " + addr + " ");
        Serial.println("for " +  String(ssid_buff));
      }
    } 
  }
}


void setup() {
  Serial.begin(115200);
  delay(500);
  wifi_set_opmode(WIFI_STA);
  wifi_promiscuous_enable(1);
  wifi_set_promiscuous_rx_cb(handle_pkt);
}

void loop() {  }