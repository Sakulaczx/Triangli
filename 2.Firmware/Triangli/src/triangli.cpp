#include "triangli.h"
#include <AsyncUDP.h>

extern BLDCMotor motor;
Command comm;

//倒立摆参数
float LQR_K1_1 = 4;  //摇摆到平衡
float LQR_K1_2 = 1.5;   //
float LQR_K1_3 = 0.30; //

float LQR_K2_1 = 3.49;   //平衡态
float LQR_K2_2 = 0.26;   //
float LQR_K2_3 = 0.15; //

float LQR_K3_1 = 10;   //摇摆到平衡
float LQR_K3_2 = 1.7;   //
float LQR_K3_3 = 1.75; //

float LQR_K4_1 = 2.4;   //摇摆到平衡
float LQR_K4_2 = 1.5;   //
float LQR_K4_3 = 1.42; //

float target_velocity = 0;
float target_angle = 89.3;
float target_voltage = 0;
float swing_up_voltage = 1.8;
float swing_up_angle = 20;
float v_i_1 = 20;
float v_p_1 = 0.5;
float v_i_2 = 10;
float v_p_2 = 0.2;
bool wifi_flag = 0;
bool Motor_enable_flag = 0;
float pendulum_angle = 0;
double gyroZangle; // Angle calculate using the gyro only
double compAngleZ; // Calculated angle using a complementary filter
double kalAngleZ;  // Calculated angle using a Kalman filter
double gyroZrate;

char buf[255];
extern AsyncUDP udp;

void do_vp1(char* cmd) { comm.scalar(&v_p_1, cmd); EEPROM.writeFloat(12, v_p_1);}
void do_vi1(char* cmd) { comm.scalar(&v_i_1, cmd);EEPROM.writeFloat(16, v_i_1); }
void do_vp2(char* cmd) { comm.scalar(&v_p_2, cmd); EEPROM.writeFloat(20, v_p_2);}
void do_vi2(char* cmd) { comm.scalar(&v_i_2, cmd);EEPROM.writeFloat(24, v_i_2); }
void do_tv(char* cmd)  { comm.scalar(&target_velocity, cmd); }
void do_K31(char* cmd) { comm.scalar(&LQR_K3_1, cmd); }
void do_K32(char* cmd) { comm.scalar(&LQR_K3_2, cmd); }
void do_K33(char* cmd) { comm.scalar(&LQR_K3_3, cmd); }
void do_K41(char* cmd) { comm.scalar(&LQR_K4_1, cmd); }
void do_K42(char* cmd) { comm.scalar(&LQR_K4_2, cmd); }
void do_K43(char* cmd) { comm.scalar(&LQR_K4_3, cmd); }

void do_TA(char* cmd) { comm.scalar(&target_angle, cmd);EEPROM.writeFloat(0, target_angle); }
void do_SV(char* cmd) { comm.scalar(&swing_up_voltage, cmd); EEPROM.writeFloat(4, swing_up_voltage); }
void do_SA(char* cmd) { comm.scalar(&swing_up_angle, cmd);EEPROM.writeFloat(8, swing_up_angle); }
void do_START(char* cmd) {  wifi_flag = !wifi_flag; }

void do_MOTOR(char* cmd)
{  
    if(Motor_enable_flag)
        digitalWrite(22,HIGH);
    else 
        digitalWrite(22,LOW);
    Motor_enable_flag = !Motor_enable_flag;
}

void wifi_print(char * s,double num)
{
    char str[255];
    char n[255];

    sprintf(n, "%.2f",num);
    strcpy(str,s);
    strcat(str, n);
    strcat(buf+strlen(buf), str);
    strcat(buf, ",\0");
}

// LQR stabilization controller functions
// calculating the voltage that needs to be set to the motor in order to stabilize the pendulum
float Triangli::controllerLQR(float p_angle, float p_vel, float m_vel)
{
    // if angle controllable
    // calculate the control law
    // LQR controller u = k*x
    //  - k = [40, 7, 0.3]
    //  - k = [13.3, 21, 0.3]
    //  - x = [pendulum angle, pendulum velocity, motor velocity]'

    if (abs(p_angle) > 2.5) {
        last_unstable_time = millis();
        if(stable) {
        target_angle = EEPROM.readFloat(0);
        stable = 0;
        }
    }
    if ((millis() - last_unstable_time) > 1000 && !stable) {
        target_angle = target_angle+p_angle;
        stable = 1;
    }

    float u;

    if (!stable) {
        motor.PID_velocity.P = v_p_1;
        motor.PID_velocity.I = v_i_1;
        u = LQR_K3_1 * p_angle + LQR_K3_2 * p_vel + LQR_K3_3 * m_vel;
    } else {
        motor.PID_velocity.P = v_p_2;
        motor.PID_velocity.I = v_i_2;
        //u = LQR_K1 * p_angle + LQR_K2 * p_vel + LQR_K3 * m_vel;
        u = LQR_K4_1 * p_angle + LQR_K4_2 * p_vel + LQR_K4_3 * m_vel;
    }

    return u;
}

void Triangli::process()
{
    mpu.update();

    accX = mpu.getAccelX();
    accY = mpu.getAccelY();
    accZ = mpu.getAccelZ();   
    gyroX = mpu.getGyroX();
    gyroY = mpu.getGyroY();
    gyroZ = mpu.getGyroZ();
     
    double dt = (double)(micros() - timer) / 1000000; // Calculate delta time
    timer = micros();
    
    double pitch = mpu.acc2rotation(accX, accY);
    gyroZrate = gyroZ / 131.0; // Convert to deg/s
    if(abs(pitch - last_pitch) > 100)
        kalmanZ.setAngle(pitch);
    
    kalAngleZ = kalmanZ.getAngle(pitch, gyroZrate + gyroZ_OFF, dt);
    last_pitch = pitch;
    gyroZangle += (gyroZrate + gyroZ_OFF) * dt;
    compAngleZ = 0.93 * (compAngleZ + (gyroZrate + gyroZ_OFF) * dt) + 0.07 * pitch;

    // Reset the gyro angle when it has drifted too much
    if (gyroZangle < -180 || gyroZangle > 180)
        gyroZangle = kalAngleZ;
      
    pendulum_angle = constrainAngle(fmod(kalAngleZ, 120) - target_angle);

    if (abs(pendulum_angle) < swing_up_angle) { // if angle small enough stabilize 0.5~30°,1.5~90°
    
        target_velocity = controllerLQR(pendulum_angle, gyroZrate, motor.shaftVelocity());
        if (abs(target_velocity) > 140)
            target_velocity = _sign(target_velocity) * 140;
            motor.controller = MotionControlType::velocity;
            motor.move(target_velocity);
    } else {    // else do swing-up
        // sets 1.5V to the motor in order to swing up
        motor.controller = MotionControlType::torque;
        target_voltage = -_sign(gyroZrate) * swing_up_voltage;
        motor.move(target_voltage);
    }

    if (wifi_flag) {
        memset(buf, 0, strlen(buf));  
        
        wifi_print("v", motor.shaftVelocity());
        wifi_print("vq", motor.voltage.q);
        wifi_print("p", pendulum_angle);
        wifi_print("t", target_angle);
        wifi_print("k", kalAngleZ);
        wifi_print("g", gyroZrate);
        udp.writeTo((const unsigned char*)buf, strlen(buf), IPAddress(192,168,4,2), 2333); //广播数据
    }
}

void Triangli::init()
{
    int k, j;
    j = 0;

    if (!EEPROM.begin(1000)) {
        Serial.println("Failed to initialise EEPROM");
        Serial.println("Restarting...");
        delay(1000);
        ESP.restart();
    }

    for (k = 0; k <= 24; k = k + 4) {
        float nan = EEPROM.readFloat(k);
        if (isnan(nan)) {
            j = 1;
            Serial.println("frist write");
            EEPROM.writeFloat(0, target_angle);     delay(10);EEPROM.commit();
            EEPROM.writeFloat(4, swing_up_voltage); delay(10);EEPROM.commit();
            EEPROM.writeFloat(8, swing_up_angle);   delay(10);EEPROM.commit();
            EEPROM.writeFloat(12, v_p_1);           delay(10);EEPROM.commit();
            EEPROM.writeFloat(16, v_i_1);           delay(10);EEPROM.commit();
            EEPROM.writeFloat(20, v_p_2);           delay(10);EEPROM.commit();
            EEPROM.writeFloat(24, v_i_2);           delay(10);EEPROM.commit();
        }
    }

    if (j == 0) {
        target_angle     = EEPROM.readFloat(0);
        swing_up_voltage = EEPROM.readFloat(4);
        swing_up_angle   = EEPROM.readFloat(8);  
        v_p_1 = EEPROM.readFloat(12);
        v_i_1 = EEPROM.readFloat(16);
        v_p_2 = EEPROM.readFloat(20);
        v_i_2 = EEPROM.readFloat(24);
        motor.PID_velocity.P = v_p_1;
        motor.PID_velocity.I = v_i_1;
    }

    comm.add("TA",do_TA);
    comm.add("START",do_START);
    comm.add("MOTOR",do_MOTOR);
    comm.add("SV",do_SV);
    comm.add("SA",do_SA);

    comm.add("VP1",do_vp1);
    comm.add("VI1",do_vi1);
    comm.add("VP2",do_vp2);
    comm.add("VI2",do_vi2);
    comm.add("TV",do_tv);
    comm.add("K31",do_K31);
    comm.add("K32",do_K32);
    comm.add("K33",do_K33);
    comm.add("K41",do_K41);
    comm.add("K42",do_K42);
    comm.add("K43",do_K43);

    mpu.init();
    mpu.update();

    delay(100); // Wait for sensor to stabilize

    accX = mpu.getAccelX();
    accY =  mpu.getAccelY();
    accZ = mpu.getAccelZ() ;
    double pitch = mpu.acc2rotation(accX, accY);

    kalmanZ.setAngle(pitch);
    gyroZangle = pitch;
}

// function constraining the angle in between -60~60
float Triangli::constrainAngle(float x)
{
    float a = 0;

    if(x < 0) {
        a = 120+x;
        if(a<abs(x))
            return a;
    }

    return x;
}