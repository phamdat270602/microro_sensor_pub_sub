#ifndef _Application_atApp_Publisher_
#define _Application_atApp_Publisher_

#include "App.h"
#include "../obj/atObj_Sensor.h"

#include <micro_ros_platformio.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/int32.h>
#include <rmw_microros/rmw_microros.h>
#include <rclc/executor.h>

#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/string.h>
#include <rosidl_runtime_c/string_functions.h>

TaskHandle_t Task_atApp_Publisher;
void atApp_Publisher_Task_Func(void *parameter);

class App_Publisher : public Application
{
public:
  App_Publisher();
  ~App_Publisher();

  IPAddress agent_ip{192, 168, 100, 240};
  size_t agent_port = 8888;

  char ssid_To_Connect[100] = "";
  char password_To_Connect[100] = "";

  // ===== Shared micro-ROS objects (owned by publisher) =====
  rcl_publisher_t publisher;
  rcl_node_t node;
  rclc_support_t support;
  rcl_allocator_t allocator;

  std_msgs__msg__String msg;

  volatile bool network_ready = false;

  // Transport is configured once. Support (context) is initialized once and
  // then reused by the subscriber as well.
  bool is_microros_transport_configured = false;
  volatile bool is_support_initialized = false;
  bool is_microros_publisher_config = false;

  int32_t counter = 0;

  // Serializes every rmw / transport touch between publisher and subscriber.
  SemaphoreHandle_t microros_mutex = NULL;

  bool configure_microros_transport();
  bool initialize_microros_support();      // NEW – creates the shared context
  bool initialize_microros_publisher();    // node + publisher only
  bool is_agent_reachable();

private:
  static void App_Publisher_Pend();
  static void App_Publisher_Start();
  static void App_Publisher_Restart();
  static void App_Publisher_Execute();
  static void App_Publisher_Suspend();
  static void App_Publisher_Resume();
  static void App_Publisher_End();

} atApp_Publisher;

App_Publisher::App_Publisher()
{
  _Pend_User    = *App_Publisher_Pend;
  _Start_User   = *App_Publisher_Start;
  _Restart_User = *App_Publisher_Restart;
  _Execute_User = *App_Publisher_Execute;
  _Suspend_User = *App_Publisher_Suspend;
  _Resume_User  = *App_Publisher_Resume;
  _End_User     = *App_Publisher_End;

  ID_Application = 1;
  Name_Application = (char *)"Publisher Application";
}

App_Publisher::~App_Publisher() {}

void App_Publisher::App_Publisher_Pend() {}

void App_Publisher::App_Publisher_Start()
{
  atApp_Publisher.microros_mutex = xSemaphoreCreateMutex();
  if (atApp_Publisher.microros_mutex == NULL)
  {
    Serial.println("ERROR: could not create micro-ROS mutex.");
  }
}

void App_Publisher::App_Publisher_Restart() {}

void App_Publisher::App_Publisher_Execute()
{
  if (!atApp_Publisher.network_ready)
  {
    return;
  }

  if (!atApp_Publisher.is_microros_transport_configured)
  {
    if (!atApp_Publisher.configure_microros_transport())
    {
      return;
    }
  }

  // 1) Shared support/context (used by publisher AND subscriber)
  if (!atApp_Publisher.is_support_initialized)
  {
    if (!atApp_Publisher.is_agent_reachable())
    {
      Serial.println("Publisher: agent not reachable; waiting.");
      return;
    }
    if (!atApp_Publisher.initialize_microros_support())
    {
      Serial.println("Publisher: support init failed; retrying.");
      return;
    }
    Serial.println("micro-ROS support/context initialized.");
  }

  // 2) Publisher node + publisher on the shared support
  if (!atApp_Publisher.is_microros_publisher_config)
  {
    if (!atApp_Publisher.is_agent_reachable())
    {
      return;
    }
    if (!atApp_Publisher.initialize_microros_publisher())
    {
      Serial.println("micro-ROS publisher initialization failed; retrying.");
      return;
    }
    Serial.println("micro-ROS publisher initialized.");
  }

  // 3) Publish
  if (!atApp_Publisher.is_agent_reachable())
  {
    Serial.println("Publisher: agent not reachable; publishing paused.");
    return;
  }

  float temperature = atObject_Sensor.temperature;
  float humidity    = atObject_Sensor.humidity;

  if (temperature == 0.0f && humidity == 0.0f)
  {
    temperature = 25.0f;
    humidity    = 50.0f;
  }

  String sensor_data = String(atApp_Publisher.counter) +
                       "_Temperature: " + String(temperature) +
                       "; Humidity: " + String(humidity);

  rosidl_runtime_c__String__assign(&atApp_Publisher.msg.data, sensor_data.c_str());

  if (atApp_Publisher.microros_mutex == NULL ||
      xSemaphoreTake(atApp_Publisher.microros_mutex, pdMS_TO_TICKS(100)) != pdTRUE)
  {
    Serial.println("micro-ROS transport is busy; publisher will retry.");
    return;
  }

  rcl_ret_t rc = rcl_publish(&atApp_Publisher.publisher,
                             &atApp_Publisher.msg, NULL);
  xSemaphoreGive(atApp_Publisher.microros_mutex);

  if (rc == RCL_RET_OK)
  {
    Serial.print("Published: ");
    Serial.println(atApp_Publisher.msg.data.data);
    atApp_Publisher.counter++;
  }
  else
  {
    Serial.print("Publish failed, error code: ");
    Serial.println((int)rc);
  }
}

void App_Publisher::App_Publisher_Suspend() {}
void App_Publisher::App_Publisher_Resume()  {}
void App_Publisher::App_Publisher_End()     {}

bool App_Publisher::configure_microros_transport()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    return false;
  }

  set_microros_wifi_transports(
      atApp_Publisher.ssid_To_Connect,
      atApp_Publisher.password_To_Connect,
      atApp_Publisher.agent_ip,
      atApp_Publisher.agent_port);
  atApp_Publisher.is_microros_transport_configured = true;
  Serial.println("micro-ROS Wi-Fi transport configured.");
  return true;
}

// NEW – creates the shared context that both apps will use.
bool App_Publisher::initialize_microros_support()
{
  if (atApp_Publisher.microros_mutex == NULL ||
      xSemaphoreTake(atApp_Publisher.microros_mutex, pdMS_TO_TICKS(100)) != pdTRUE)
  {
    return false;
  }

  atApp_Publisher.allocator = rcl_get_default_allocator();

  if (rclc_support_init(&atApp_Publisher.support, 0, NULL,
                        &atApp_Publisher.allocator) != RCL_RET_OK)
  {
    Serial.println("ERROR: micro-ROS support init failed");
    Serial.println(rcl_get_error_string().str); 
    xSemaphoreGive(atApp_Publisher.microros_mutex);
    return false;
  }

  atApp_Publisher.is_support_initialized = true;
  xSemaphoreGive(atApp_Publisher.microros_mutex);
  return true;
}

// Node + publisher only – assumes support is already initialized.
bool App_Publisher::initialize_microros_publisher()
{
  if (atApp_Publisher.microros_mutex == NULL ||
      xSemaphoreTake(atApp_Publisher.microros_mutex, pdMS_TO_TICKS(100)) != pdTRUE)
  {
    return false;
  }

  if (rclc_node_init_default(&atApp_Publisher.node,
                             "esp32_publisher", "",
                             &atApp_Publisher.support) != RCL_RET_OK)
  {
    Serial.println("ERROR: node initialization failed");
    xSemaphoreGive(atApp_Publisher.microros_mutex);
    return false;
  }

  if (rclc_publisher_init_default(
          &atApp_Publisher.publisher,
          &atApp_Publisher.node,
          ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
          "/esp32_counter") != RCL_RET_OK)
  {
    Serial.println("ERROR: publisher initialization failed");
    rcl_node_fini(&atApp_Publisher.node);
    xSemaphoreGive(atApp_Publisher.microros_mutex);
    return false;
  }

  atApp_Publisher.is_microros_publisher_config = true;
  xSemaphoreGive(atApp_Publisher.microros_mutex);
  return true;
}

bool App_Publisher::is_agent_reachable()
{
  if (WiFi.status() != WL_CONNECTED || atApp_Publisher.microros_mutex == NULL ||
      xSemaphoreTake(atApp_Publisher.microros_mutex, pdMS_TO_TICKS(700)) != pdTRUE)
  {
    return false;
  }

  const bool ok = rmw_uros_ping_agent(200, 3) == RMW_RET_OK;
  xSemaphoreGive(atApp_Publisher.microros_mutex);
  return ok;
}

void atApp_Publisher_Task_Func(void *parameter)
{
  while (1)
  {
    atApp_Publisher.Run_Application(APP_RUN_MODE_AUTO);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#endif