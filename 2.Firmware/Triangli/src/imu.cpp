#include "imu.h"

void IMU::init()
{
    Wire.begin(IMU_I2C_SDA, IMU_I2C_SCL,400000);//Wire.begin(IMU_I2C_SDA);
    //Wire.setClock(400000);
    unsigned long timeout = 5000;
    unsigned long preMillis = millis();
    mpu = MPU6050(0x68);
    while (!mpu.testConnection());
    
    if(!mpu.testConnection())
    {
        Serial.print(F("Unable to connect to MPU6050.\n"));
        return ;
    }

    Serial.print(F("Initialization MPU6050 now,  don't move.\n"));
    mpu.initialize();
    Serial.print(F("Initialization MPU6050 success.\n"));
}

void IMU::update()
{
	mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

	// Serial.print(gx);
	// Serial.print(" ");
	// Serial.print(gy);
	// Serial.print(" ");
	// Serial.print(gz);
	// Serial.println(" ");
}

int16_t IMU::getAccelX()
{
	return ax;
}

int16_t IMU::getAccelY()
{
	return ay;
}

int16_t IMU::getAccelZ()
{
	return az;
}

int16_t IMU::getGyroX()
{
	return gx;
}

int16_t IMU::getGyroY()
{
	return gy;
}

int16_t IMU::getGyroZ()
{
	return gz;
}

/* mpu6050加速度转换为角度
            acc2rotation(ax, ay)
            acc2rotation(az, ay) */
double IMU::acc2rotation(double x, double y)
{
    if (y < 0) {
        return atan(x / y) / 1.570796 * 90 + 180;
    } else if (x < 0) {
        return (atan(x / y) / 1.570796 * 90 + 360);
    } else {
        return (atan(x / y) / 1.570796 * 90);
    }
}