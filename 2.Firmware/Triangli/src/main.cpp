/***************************************************
  基于LQR的自平衡莱洛三角形固件源码

  1. FOC引脚32, 33, 25, 22    22为enable；
  2. AS5600霍尔传感器 SDA-23 SCL-5  MPU6050六轴传感器 SDA-19 SCL-18；
  3. 在wifi上位机窗口中输入：TA+角度，就可以修改平衡角度；
  4. 在使用自己的电机时，请一定记得修改默认极对数，即 BLDCMotor(5) 中的值，设置为自己的极对数数字，磁铁数量/2；
  5. 程序默认设置的供电电压为 12V,用其他电压供电请记得修改 voltage_power_supply , voltage_limit 变量中的值；
  6. 默认PID针对的电机是 GB2204 ，使用自己的电机需要修改PID参数，才能实现更好效果

  本固件基于 https://gitee.com/coll45/foc 修改，原作者：45coll

  本项目Github repositories：

  Last review/edit by 一叶知秋君莫笑我: 2022/01/06
 ****************************************************/

#include <SimpleFOC.h>
#include "triangli.h"
#include "network.h"

Network wifi;
Triangli triangli;

MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
TwoWire I2Ctwo = TwoWire(1);

//电机参数
BLDCMotor motor = BLDCMotor(5);
BLDCDriver3PWM driver = BLDCDriver3PWM(32, 33, 25, 22);

// instantiate the commander
void setup() {

    Serial.begin(115200);

    /* Initial Triangli parameter*/
    triangli.init();
    Serial.println("kalman mpu6050 init");

    // wifi初始化
    wifi.init();
    
    I2Ctwo.begin(23, 5, 400000);   //SDA,SCL
    sensor.init(&I2Ctwo);
  
    //连接motor对象与传感器对象
    motor.linkSensor(&sensor);

    //供电电压设置 [V]
    driver.voltage_power_supply = 12;
    driver.init();

    //连接电机和driver对象
    motor.linkDriver(&driver);

    //FOC模型选择
    motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

    //运动控制模式设置
    motor.controller = MotionControlType::velocity;
        //速度PI环设置
    motor.PID_velocity.P = 0.5;
    motor.PID_velocity.I = 20;

    //最大电机限制电机
    motor.voltage_limit = 12;

    //速度低通滤波时间常数
    motor.LPF_velocity.Tf = 0.01;

    //设置最大速度限制
    motor.velocity_limit = 40;
    //motor.useMonitoring(Serial);

    //初始化电机
    motor.init();

    //初始化 FOC
    motor.initFOC();

    Serial.println(F("Motor ready."));
}

void loop() {
    motor.loopFOC();

    triangli.process();
}

