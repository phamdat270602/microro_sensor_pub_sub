/**
@file
Objective for control with Esp32 Wrover.
*/
/*
  Object_Sensor.h - Arduino library for control with ESP32
  Library:: Object_Sensor
  This version is
  Copyright:: 2021 nguyentrinhtuan1996@gmail.com
*/
#ifndef Object_Sensor_h
#define Object_Sensor_h


/* _____PROJECT INCLUDES____________________________________________________ */
#include "Arduino.h"
#include "Object.h"

/* _____DEFINITIONS__________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */


/* _____GLOBAL FUNCTION______________________________________________________ */

/* _____CLASS DEFINITION_____________________________________________________ */
/**
 * This class is the application to manage the 
 */

class Object_Sensor :public Object
{
public:
    static void   Object_Cont_Start();
    static void   Object_Cont_Execute();    
    static void   Object_Cont_End();
    
    uint8_t Get_Fault_State();
    Object_Sensor(/* args */);
    ~Object_Sensor();

    float temperature;
    float humidity;

protected: 
    
private:
    /* data */ 
} atObject_Sensor;
/**
 * @brief This function code is executed one time when class is init
 * 
 */
Object_Sensor::Object_Sensor(/* args */)
{
     _Start_User      = *Object_Cont_Start;
     _Execute_User    = *Object_Cont_Execute;
     _End_User        = *Object_Cont_End;
}
/**
 * @brief This function code is executed one time when class is deinit
 * 
 */
Object_Sensor::~Object_Sensor()
{
}
/**
 * This start function will init some critical function 
 */
void  Object_Sensor::Object_Cont_Start()
{

} 
/**
 * Execute fuction of EMM app
 */
void  Object_Sensor:: Object_Cont_Execute()
{   

}    
void  Object_Sensor:: Object_Cont_End(){}

#endif