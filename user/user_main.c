/*
 * Simplest TCP server EVER!
 * 2015, Salvatore Merone
 *
 */

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"
#include "gpio.h"
#include <ip_addr.h>

#define LOCAL_PORT 444

#define startServer_Prio 0
#define startServer_QueueLen 1

os_event_t startServer_Queue[startServer_QueueLen];

static void ICACHE_FLASH_ATTR dataRecvCallback(void *arg, char *pusrdata, unsigned short lenght){
    struct espconn *pCon = arg;
    char string[10] = "ACK";

    gpio_output_set(BIT5, 0, BIT5, 0);
    os_delay_us(10000);
    gpio_output_set(0, BIT5, 0, BIT5);

    espconn_sent(pCon, string, os_strlen(string));
}

static void ICACHE_FLASH_ATTR dataSentCallback(void *arg){
}

static void ICACHE_FLASH_ATTR connectionCallback(void *arg){
    struct espconn *pCon = arg;
    char string[10] = "OK";

    espconn_regist_recvcb(pCon, dataRecvCallback);
    espconn_regist_sentcb(pCon, dataSentCallback);

    espconn_sent(pCon, string, os_strlen(string));

    gpio_output_set(BIT5, 0, BIT5, 0);
    os_delay_us(50000);
    gpio_output_set(0, BIT5, 0, BIT5);

}

static void ICACHE_FLASH_ATTR startServer(os_event_t *events){
    struct espconn *pCon = NULL;
    pCon = (struct espconn *)os_zalloc(sizeof(struct espconn));
    ets_memset(pCon, 0, sizeof(struct espconn));

    espconn_create(pCon);
    pCon->type = ESPCONN_TCP;
    pCon->state = ESPCONN_NONE;
    pCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
    pCon->proto.tcp->local_port = LOCAL_PORT;
    pCon->proto.tcp->remote_port = LOCAL_PORT;
    espconn_regist_connectcb(pCon, connectionCallback);

    // Start listening
    espconn_accept(pCon);

    // After 15 seconds close TCP connection
    espconn_regist_time(pCon, 15, 0);
}



// Setup function
void user_init(void){
    const char ssid[32] = "SSID";
    const char password[32] = "PASSWORD";

    struct station_config stationConf;

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,FUNC_GPIO5);
    PIN_PULLDWN_DIS(PERIPHS_IO_MUX_GPIO5_U);
    //PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);

    // Blink Led Once
    gpio_output_set(BIT5, 0, BIT5, 0);
    os_delay_us(10000);
    gpio_output_set(0, BIT5, 0, BIT5);

    wifi_set_opmode( STATION_MODE );
    os_memcpy(&stationConf.ssid, ssid, 32);
    os_memcpy(&stationConf.password, password, 32);
    wifi_station_set_config(&stationConf);
    wifi_set_phy_mode(PHY_MODE_11B);                // Set 11Mbps connection, draws less power
    wifi_station_set_auto_connect(1);
    wifi_station_connect();

    // Start server task
    system_os_task(startServer, startServer_Prio, startServer_Queue, startServer_QueueLen);
    system_os_post(startServer_Prio, 0, 0 );

}

