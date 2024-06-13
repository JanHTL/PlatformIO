#include <Arduino.h>
#include <Servo.h>
#include "drive_functions.hpp"

DriveController::DriveController() 
{
  this->linkesRad.attach(linkesRadPin); //10
  this->rechtesRad.attach(rechtesRadPin); //9
  this->kopf.attach(UltraschallsensorServoPin);
}

void DriveController::stop()
{
  this->linkesRad.write(linkesRadStop);
  this->rechtesRad.write(rechtesRadStop);
}

void DriveController::drive_forward(int delay_time) 
{
  this->linkesRad.write(linkesRadStop+3);
  this->rechtesRad.write(rechtesRadStop-4);
  delay(delay_time);
}

void DriveController::drive_backward(int delay_time) 
{
  this->linkesRad.write(linkesRadStop-3);  
  this->rechtesRad.write(rechtesRadStop+3); 
  delay(delay_time);
}

void DriveController::turn_left(int delay_time)
{
  this->linkesRad.write(linkesRadStop-turn_speed_offest);
  this->rechtesRad.write(rechtesRadStop-turn_speed_offest);
  delay(delay_time);
}

void DriveController::turn_right(int delay_time)
{
  this->linkesRad.write(linkesRadStop+turn_speed_offest);
  this->rechtesRad.write(rechtesRadStop+turn_speed_offest);
  delay(delay_time);
}

Servo* DriveController::get_head() 
{
    return &this->kopf;
}