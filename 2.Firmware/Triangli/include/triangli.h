#ifndef TRIANGLI_H
#define TRIANGLI_H

#include"EEPROM.h"
#include "Kalman.h"  // Source: https://github.com/TKJElectronics/KalmanFilter
#include "Command.h"
#include "imu.h"
#include <SimpleFOC.h>

#define gyroZ_OFF -0.19

class Triangli
{
private:
    uint32_t last_unstable_time;
    bool stable = 0;
    
    double accX, accY, accZ;
    double gyroX, gyroY, gyroZ;

    uint32_t timer;
    double last_pitch;

    Kalman kalmanZ;
    IMU mpu;
public:
    float controllerLQR(float p_angle, float p_vel, float m_vel);
    void process();
    void init();

    float constrainAngle(float x);
};

#endif