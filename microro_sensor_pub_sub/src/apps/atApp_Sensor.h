/**
@file

*/
/*
  Application example t

  Library:: 

  This version is

  Copyright:: 2021 nguyentrinhtuan1996@gmail.com
*/

#ifndef _Application_atApp_Sensor_
#define _Application_atApp_Sensor_
/* _____PROJECT INCLUDES____________________________________________________ */
#include "App.h"
#include "../obj/atObj_Sensor.h"

/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
TaskHandle_t Task_atApp_Sensor;  
void atApp_Sensor_Task_Func(void *parameter);
///////////////////////////////////////////////Testing part//
/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */

/**
 * This Application class is the application to manage the 
 */
class App_Sensor : public Application
{
public:
  	App_Sensor();
 	~App_Sensor();
protected:
private:
  	static void  App_Sensor_Pend();
	static void  App_Sensor_Start();
	static void  App_Sensor_Restart();
	static void  App_Sensor_Execute();
	static void  App_Sensor_Suspend();
	static void  App_Sensor_Resume();	  
	static void  App_Sensor_End();

	unsigned long last_time = 0;
} atApp_Sensor ;
/**
 * This function will be automaticaly called when a object is created by this class
 */
App_Sensor::App_Sensor(/* args */)
{
  	_Pend_User 	     = *App_Sensor_Pend;
	_Start_User 	 = *App_Sensor_Start;
	_Restart_User 	 = *App_Sensor_Restart;
	_Execute_User 	 = *App_Sensor_Execute;
	_Suspend_User	 = *App_Sensor_Suspend;
	_Resume_User	 = *App_Sensor_Resume;
	_End_User	     = *App_Sensor_End;

	// change the ID of application
	ID_Application = 1;
	// change the application name
	Name_Application = (char*)"Sensor Application";
	// change the ID of SNM
}
/**
 * This function will be automaticaly called when the object of class is delete
 */
App_Sensor::~App_Sensor()
{
	
}
/**
 * Pend to start is the first task of this application it will do prerequisite condition. In the debit mode, task will send information of application to terminal to start the application.
 */
void  App_Sensor::App_Sensor_Pend()
{
    // atService_XYZ.Debug();
}
/**
 * This start function will init some critical function 
 */
void  App_Sensor::App_Sensor_Start()
{
	atObject_Sensor.temperature = 25.0;
	atObject_Sensor.humidity = 50.0;
}  
/**
 * Restart function of SNM  app
 */
void  App_Sensor::App_Sensor_Restart()
{

}
/**
 * Execute fuction of SNM app
 */
void  App_Sensor::App_Sensor_Execute()
{
	unsigned long current_time = millis();
	if(current_time - atApp_Sensor.last_time >= 3000)
	{
		atObject_Sensor.temperature = random(20, 30);
		atObject_Sensor.humidity = random(40, 60);

		atApp_Sensor.last_time = current_time;
	}
    if(atApp_Sensor.User_Mode == APP_USER_MODE_DEBUG)
    {
		Serial.printf("Temp: %.2f, Hum: %.2f \n",atObject_Sensor.temperature,atObject_Sensor.humidity);
    }   
}
void  App_Sensor::App_Sensor_Suspend(){}
void  App_Sensor::App_Sensor_Resume(){}	  
void  App_Sensor::App_Sensor_End(){}
void atApp_Sensor_Task_Func(void *parameter)
{
  while (1)
  {
    atApp_Sensor.Run_Application(APP_RUN_MODE_AUTO);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#endif