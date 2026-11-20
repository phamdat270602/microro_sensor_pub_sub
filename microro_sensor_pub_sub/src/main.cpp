// Template_Application_ESP32_V2.0

// #include "../src/apps/atApp_CP.h"
// #include "../src/apps/atApp_Publisher.h"
// #include "../src/apps/atApp_Subscriber.h"
#include "../src/apps/atApp_Wifi.h"
// #include "../src/apps/atApp_Sensor.h"
// #include "../src/apps/atApp_Blink_led.h"


void setup() {

Serial.begin(9600);

// uncomment to put the app in debug mode
    atApp_Wifi.Debug();
    // atApp_Blink_led.Debug();
    // atApp_Sensor.Debug();
    // atApp_Publisher.Debug();
    // atApp_Subscriber.Debug();
    // atApp_CP.Debug();

//--------------------------------------------------------------------Tasks table ----------------------------------------------------------------------------------//
// || Creation           ||     Task function         ||     PC name                    || heap size  || Parameter || Priority  || Task handle          || CoreID   ||
  xTaskCreatePinnedToCore(atApp_Wifi_Task_Func        , "atApp_Wifi_Application"        ,  4096       ,     NULL    ,   1       , &Task_atApp_Wifi        ,    1     );
  // xTaskCreatePinnedToCore(atApp_CP_Task_Func          , "atApp_CP_Application"          ,  10000      ,     NULL    ,   1       , &Task_atApp_CP          ,    0     );
  // xTaskCreatePinnedToCore(atApp_Sensor_Task_Func      , "atApp_Sensor_Application"      ,  8192       ,     NULL    ,   1       , &Task_atApp_Sensor      ,    0     );
  // xTaskCreatePinnedToCore(atApp_Publisher_Task_Func   , "atApp_Publisher_Application"   ,  8192       ,     NULL    ,   1       , &Task_atApp_Publisher      ,    0     );
  // xTaskCreatePinnedToCore(atApp_Subscriber_Task_Func  , "atApp_Subscriber_Application"  ,  8192       ,     NULL    ,   1       , &Task_atApp_Subscriber  ,    0     );
  // xTaskCreatePinnedToCore(atApp_Blink_led_Task_Func   , "atApp_Blink_led_Application"   ,  8192       ,     NULL    ,   1       , &Task_atApp_Blink_led   ,    0     );

//------------------------------------------------------------------------------------------------------------------------------------------------------------------//


  // atApp_CP creates the publisher task only after Wi-Fi is connected.
}
void loop() {}
