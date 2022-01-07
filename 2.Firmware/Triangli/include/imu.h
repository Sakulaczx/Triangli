#ifndef IMU_H
#define IMU_H

#include <MPU6050.h>

#define IMU_I2C_SDA 19 
#define IMU_I2C_SCL 18

class IMU
{
private:
    MPU6050 mpu;
    int flag;
    long last_update_time;
    int16_t ax, ay, az;
	int16_t gx, gy, gz;

public:
    void init();
    void update();

    int16_t getAccelX();
	int16_t getAccelY();
	int16_t getAccelZ();

	int16_t getGyroX();
	int16_t getGyroY();
	int16_t getGyroZ();

    double acc2rotation(double x, double y);
};

#endif