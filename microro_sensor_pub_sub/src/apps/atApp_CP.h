#ifndef _Application_atApp_CP_
#define _Application_atApp_CP_

#include "App.h"
#include "atApp_Wifi.h"
#include "atApp_Publisher.h"
#include "atApp_Subscriber.h"
#include "atApp_Blink_led.h"

TaskHandle_t Task_atApp_CP;
void atApp_CP_Task_Func(void *parameter);

class App_CP : public Application
{
public:
  App_CP();
  ~App_CP();

private:
  static void App_CP_Pend();
  static void App_CP_Start();
  static void App_CP_Restart();
  static void App_CP_Execute();
  static void App_CP_Suspend();
  static void App_CP_Resume();
  static void App_CP_End();
} atApp_CP;

App_CP::App_CP()
{
  _Pend_User = *App_CP_Pend;
  _Start_User = *App_CP_Start;
  _Restart_User = *App_CP_Restart;
  _Execute_User = *App_CP_Execute;
  _Suspend_User = *App_CP_Suspend;
  _Resume_User = *App_CP_Resume;
  _End_User = *App_CP_End;
  ID_Application = 1;
  Name_Application = (char *)"CP Application";
}

App_CP::~App_CP() {}
void App_CP::App_CP_Pend() {}
void App_CP::App_CP_Start() {}
void App_CP::App_CP_Restart() {}

void App_CP::App_CP_Execute()
{
  const bool wifi_connected =
      atApp_Wifi.is_Connected && WiFi.status() == WL_CONNECTED;

  if (!wifi_connected)
  {
    atApp_Publisher.network_ready = false;
    atApp_Subscriber.network_ready = false;
    return;
  }

  strncpy(atApp_Publisher.ssid_To_Connect,
          atApp_Wifi.ssid_To_Connect,
          sizeof(atApp_Publisher.ssid_To_Connect) - 1);
  atApp_Publisher.ssid_To_Connect[sizeof(atApp_Publisher.ssid_To_Connect) - 1] = '\0';
  strncpy(atApp_Publisher.password_To_Connect,
          atApp_Wifi.password_To_Connect,
          sizeof(atApp_Publisher.password_To_Connect) - 1);
  atApp_Publisher.password_To_Connect[sizeof(atApp_Publisher.password_To_Connect) - 1] = '\0';

  // Publisher owns the shared micro-ROS Wi-Fi transport/context.
  atApp_Publisher.network_ready = true;
  atApp_Subscriber.network_ready = true;

  // Subscriber only receives the Bool command. The control plane decides when
  // Blink_led may apply it to GPIO 2.
  if (atApp_Subscriber.has_received_led_command)
  {
    atApp_Blink_led.led_state = atApp_Subscriber.received_led_state;
  }
}

void App_CP::App_CP_Suspend() {}
void App_CP::App_CP_Resume() {}
void App_CP::App_CP_End()
{
  atApp_Publisher.network_ready = false;
  atApp_Subscriber.network_ready = false;
}

void atApp_CP_Task_Func(void *parameter)
{
  while (true)
  {
    atApp_CP.Run_Application(APP_RUN_MODE_AUTO);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#endif