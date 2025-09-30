#ifndef WIFI_STATION_H
#define WIFI_STATION_H

void wifi_init(void);

void wifi_start(const char* ssid,
                const char* password); 

#endif
