#pragma once

#include <Servo.h>

#define linkesRadPin 10
#define rechtesRadPin 9
#define UltraschallsensorServoPin 12 

#define linkesRadStop  90
#define rechtesRadStop 90
#define turn_speed_offest 3

class DriveController {
private:
    Servo linkesRad;
    Servo rechtesRad;
    Servo kopf;
public:
    DriveController();
    void stop();
    void drive_forward(int delay_time);
    void drive_backward(int delay_time);
    void turn_left(int delay_time);
    void turn_right(int delay_time);
    Servo* get_head();
};