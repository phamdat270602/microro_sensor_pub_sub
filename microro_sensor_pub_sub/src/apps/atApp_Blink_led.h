/**
@file

*/
/*
  Application example t

  Library:: 

  This version is

  Copyright:: 2021 nguyentrinhtuan1996@gmail.com
*/

#ifndef _Application_atApp_Blink_led_
#define _Application_atApp_Blink_led_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
// #include "../services/atService_XYZ.h"
/* _____DEFINITIONS__________________________________________________________ */
# define LED_PIN 2
/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_Blink_led;  
void atApp_Blink_led_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the 
 */
class App_Blink_led : public Application
{
public:
  	App_Blink_led();
 	~App_Blink_led();

    volatile bool led_state = false;
protected:
private:
  	static void  App_Blink_led_Pend();
	static void  App_Blink_led_Start();
	static void  App_Blink_led_Restart();
	static void  App_Blink_led_Execute();
	static void  App_Blink_led_Suspend();
	static void  App_Blink_led_Resume();	  
	static void  App_Blink_led_End();
} atApp_Blink_led ;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_Blink_led::App_Blink_led(/* args */)
{
  	_Pend_User 	     = *App_Blink_led_Pend;
	_Start_User 	 = *App_Blink_led_Start;
	_Restart_User 	 = *App_Blink_led_Restart;
	_Execute_User 	 = *App_Blink_led_Execute;
	_Suspend_User	 = *App_Blink_led_Suspend;
	_Resume_User	 = *App_Blink_led_Resume;
	_End_User	     = *App_Blink_led_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char*)"Blink_led Application";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_Blink_led::~App_Blink_led()
{
	
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void  App_Blink_led::App_Blink_led_Pend()
{
    // atService_XYZ.Debug();
}
/**
 * This start function will init some critical function 
 */
void  App_Blink_led::App_Blink_led_Start()
{
    pinMode(LED_PIN, OUTPUT);
	// init atXYZ Service in the fist running time
	// atService_XYZ.Run_Service();
}  
/**
 * Restart function of SNM  app
 */
void  App_Blink_led::App_Blink_led_Restart()
{

}
/**
 * Execute fuction of SNM app
 */
void  App_Blink_led::App_Blink_led_Execute()
{	
	digitalWrite(LED_PIN, atApp_Blink_led.led_state ? HIGH : LOW);
    
	// atService_XYZ.Run_Service();
    if(atApp_Blink_led.User_Mode == APP_USER_MODE_DEBUG)
    {
		
    }   
}
void  App_Blink_led::App_Blink_led_Suspend(){}
void  App_Blink_led::App_Blink_led_Resume(){}	  
void  App_Blink_led::App_Blink_led_End(){}
void atApp_Blink_led_Task_Func(void *parameter)
{
  while (1)
  {
    atApp_Blink_led.Run_Application(APP_RUN_MODE_AUTO);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#endif