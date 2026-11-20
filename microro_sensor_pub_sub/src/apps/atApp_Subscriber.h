#ifndef _Application_atApp_Subscriber_
#define _Application_atApp_Subscriber_

#include "App.h"
// Subscriber never touches Blink_led. CP is the only writer of led_state.
// Subscriber also never touches the micro-ROS transport/support; it reuses
// the publisher's shared context + mutex.
#include "atApp_Publisher.h"

#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/bool.h>

#define LED_CONTROL_TOPIC "/led_control"

TaskHandle_t Task_atApp_Subscriber;
void atApp_Subscriber_Task_Func(void *parameter);

class App_Subscriber : public Application
{
public:
  App_Subscriber();
  ~App_Subscriber();

  volatile bool network_ready = false;
  bool is_microros_subscriber_configured = false;

  // Latest command from /led_control. Consumed by atApp_CP.
  volatile bool has_received_led_command = false;
  volatile bool received_led_state = false;

  // Subscriber-local ROS objects (share publisher's support/context).
  rcl_subscription_t subscription;
  std_msgs__msg__Bool msg;
  rclc_executor_t executor;
  rcl_node_t node;

  bool initialize_microros_subscriber();

private:
  static void App_Subscriber_Pend();
  static void App_Subscriber_Start();
  static void App_Subscriber_Restart();
  static void App_Subscriber_Execute();
  static void App_Subscriber_Suspend();
  static void App_Subscriber_Resume();
  static void App_Subscriber_End();
  static void App_Subscriber_BoolCallback(const std_msgs__msg__Bool *msg,
                                          void *context);
} atApp_Subscriber;

App_Subscriber::App_Subscriber()
{
  _Pend_User    = *App_Subscriber_Pend;
  _Start_User   = *App_Subscriber_Start;
  _Restart_User = *App_Subscriber_Restart;
  _Execute_User = *App_Subscriber_Execute;
  _Suspend_User = *App_Subscriber_Suspend;
  _Resume_User  = *App_Subscriber_Resume;
  _End_User     = *App_Subscriber_End;

  ID_Application = 2;
  Name_Application = (char *)"Subscriber Application";
}

App_Subscriber::~App_Subscriber() {}
void App_Subscriber::App_Subscriber_Pend() {}

void App_Subscriber::App_Subscriber_Start()
{
  // Init happens in Execute once publisher's shared support is ready.
}

void App_Subscriber::App_Subscriber_Restart() {}

void App_Subscriber::App_Subscriber_Execute()
{
  if (!atApp_Subscriber.network_ready)
  {
    return;
  }

  // Wait for the publisher to create the shared support / context.
  if (!atApp_Publisher.is_support_initialized)
  {
    return;
  }

  if (!atApp_Subscriber.is_microros_subscriber_configured)
  {
    if (!atApp_Subscriber.initialize_microros_subscriber())
    {
      Serial.println("ERROR: micro-ROS subscriber initialization failed.");
      return;
    }
    Serial.println("micro-ROS Bool subscriber ready on " LED_CONTROL_TOPIC ".");
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    atApp_Subscriber.network_ready = false;
    return;
  }

  // Serialize transport access with the publisher.
  if (atApp_Publisher.microros_mutex == NULL ||
      xSemaphoreTake(atApp_Publisher.microros_mutex, pdMS_TO_TICKS(100)) != pdTRUE)
  {
    return;
  }
  rcl_ret_t rc = rclc_executor_spin_some(&atApp_Subscriber.executor,
                                         RCL_MS_TO_NS(20));
  xSemaphoreGive(atApp_Publisher.microros_mutex);

  if (rc != RCL_RET_OK)
  {
    Serial.println("ERROR: micro-ROS subscriber executor spin failed.");
  }
}

void App_Subscriber::App_Subscriber_Suspend() {}
void App_Subscriber::App_Subscriber_Resume()  {}
void App_Subscriber::App_Subscriber_End()     { atApp_Subscriber.network_ready = false; }

void App_Subscriber::App_Subscriber_BoolCallback(const std_msgs__msg__Bool *msg,
                                                 void *context)
{
  App_Subscriber *self = static_cast<App_Subscriber *>(context);
  if (self == nullptr)
  {
    self = &atApp_Subscriber;
  }

  // Only record the command. CP decides when Blink_led applies it.
  self->received_led_state       = msg->data;
  self->has_received_led_command = true;
}

// Build the subscriber on the publisher's shared support/context.
bool App_Subscriber::initialize_microros_subscriber()
{
  if (atApp_Publisher.microros_mutex == NULL ||
      xSemaphoreTake(atApp_Publisher.microros_mutex, pdMS_TO_TICKS(100)) != pdTRUE)
  {
    return false;
  }

  // ---- USE THE PUBLISHER'S NODE (do NOT create a second node) ----
  rcl_node_t *shared_node = &atApp_Publisher.node;

  if (rclc_subscription_init_default(
          &atApp_Subscriber.subscription,
          shared_node,
          ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
          LED_CONTROL_TOPIC) != RCL_RET_OK)
  {
    Serial.print("ERROR: Bool subscription initialization failed: ");
    Serial.println(rcl_get_error_string().str);
    xSemaphoreGive(atApp_Publisher.microros_mutex);
    return false;
  }

  if (rclc_executor_init(&atApp_Subscriber.executor,
                         &atApp_Publisher.support.context,
                         1,
                         &atApp_Publisher.allocator) != RCL_RET_OK)
  {
    Serial.print("ERROR: micro-ROS executor initialization failed: ");
    Serial.println(rcl_get_error_string().str);
    rcl_subscription_fini(&atApp_Subscriber.subscription, shared_node);
    xSemaphoreGive(atApp_Publisher.microros_mutex);
    return false;
  }

  if (rclc_executor_add_subscription_with_context(
          &atApp_Subscriber.executor,
          &atApp_Subscriber.subscription,
          &atApp_Subscriber.msg,
          (rclc_subscription_callback_with_context_t)
              atApp_Subscriber.App_Subscriber_BoolCallback,
          &atApp_Subscriber,
          ON_NEW_DATA) != RCL_RET_OK)
  {
    Serial.print("ERROR: Bool subscription executor registration failed: ");
    Serial.println(rcl_get_error_string().str);
    rclc_executor_fini(&atApp_Subscriber.executor);
    rcl_subscription_fini(&atApp_Subscriber.subscription, shared_node);
    xSemaphoreGive(atApp_Publisher.microros_mutex);
    return false;
  }

  atApp_Subscriber.is_microros_subscriber_configured = true;
  xSemaphoreGive(atApp_Publisher.microros_mutex);
  return true;
}

void atApp_Subscriber_Task_Func(void *parameter)
{
  while (true)
  {
    atApp_Subscriber.Run_Application(APP_RUN_MODE_AUTO);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#endif