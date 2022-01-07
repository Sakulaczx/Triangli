/***************************************************
  ����LQR����ƽ�����������ι̼�Դ��

  1. FOC����32, 33, 25, 22    22Ϊenable��
  2. AS5600���������� SDA-23 SCL-5  MPU6050���ᴫ���� SDA-19 SCL-18��
  3. ��wifi��λ�����������룺TA+�Ƕȣ��Ϳ����޸�ƽ��Ƕȣ�
  4. ��ʹ���Լ��ĵ��ʱ����һ���ǵ��޸�Ĭ�ϼ��������� BLDCMotor(5) �е�ֵ������Ϊ�Լ��ļ��������֣���������/2��
  5. ����Ĭ�����õĹ����ѹΪ 12V,��������ѹ������ǵ��޸� voltage_power_supply , voltage_limit �����е�ֵ��
  6. Ĭ��PID��Եĵ���� GB2204 ��ʹ���Լ��ĵ����Ҫ�޸�PID����������ʵ�ָ���Ч��

  ���̼����� https://gitee.com/coll45/foc �޸ģ�ԭ���ߣ�45coll

  ����ĿGithub repositories��

  Last review/edit by һҶ֪���ĪЦ��: 2022/01/06
 ****************************************************/

#include <SimpleFOC.h>
#include "triangli.h"
#include "network.h"

Network wifi;
Triangli triangli;

MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
TwoWire I2Ctwo = TwoWire(1);

//�������
BLDCMotor motor = BLDCMotor(5);
BLDCDriver3PWM driver = BLDCDriver3PWM(32, 33, 25, 22);

// instantiate the commander
void setup() {

    Serial.begin(115200);

    /* Initial Triangli parameter*/
    triangli.init();
    Serial.println("kalman mpu6050 init");

    // wifi��ʼ��
    wifi.init();
    
    I2Ctwo.begin(23, 5, 400000);   //SDA,SCL
    sensor.init(&I2Ctwo);
  
    //����motor�����봫��������
    motor.linkSensor(&sensor);

    //�����ѹ���� [V]
    driver.voltage_power_supply = 12;
    driver.init();

    //���ӵ����driver����
    motor.linkDriver(&driver);

    //FOCģ��ѡ��
    motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

    //�˶�����ģʽ����
    motor.controller = MotionControlType::velocity;
        //�ٶ�PI������
    motor.PID_velocity.P = 0.5;
    motor.PID_velocity.I = 20;

    //��������Ƶ��
    motor.voltage_limit = 12;

    //�ٶȵ�ͨ�˲�ʱ�䳣��
    motor.LPF_velocity.Tf = 0.01;

    //��������ٶ�����
    motor.velocity_limit = 40;
    //motor.useMonitoring(Serial);

    //��ʼ�����
    motor.init();

    //��ʼ�� FOC
    motor.initFOC();

    Serial.println(F("Motor ready."));
}

void loop() {
    motor.loopFOC();

    triangli.process();
}

