#ifndef WIFI_AP_H
#define WIFI_AP_H

void wifi_ap_init(void);

void wifi_ap_start(const char* ssid, 
                    const char* password, 
                    int channel, 
                    int max_conn);

#endif
