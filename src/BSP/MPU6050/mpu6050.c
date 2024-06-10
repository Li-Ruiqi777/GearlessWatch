#include "mpu6050.h"
#include "i2c.h"

/**
 * @brief ��ʼ��MPU6050
 * @param
 * @return ����ֵ:0,�ɹ�
 * 				����,�������
 */
uint8_t MPU_Init(void)
{
	__IO uint8_t addr;
	__IO uint8_t ret;
	ret = MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80); // ��λMPU6050
	delay_ms(100);
	ret = MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00); // ����MPU6050
	MPU_Set_Gyro_Fsr(3);													 // �����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(0);													 // ���ٶȴ�����,��2g
	MPU_Set_Rate(50);															 // ���ò�����50Hz
	ret = MPU_Write_Byte(MPU_INT_EN_REG, 0X00);		 // �ر������ж�
	ret = MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00); // I2C��ģʽ�ر�
	ret = MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);	 // �ر�FIFO
	ret = MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80); // INT���ŵ͵�ƽ��Ч
	addr = MPU_Read_Byte(MPU_DEVICE_ID_REG);
	if (addr == MPU_ADDR) // ����ID��ȷ
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01); // ����CLKSEL,PLL X��Ϊ�ο�
		MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00); // ���ٶ��������Ƕ�����
		MPU_Set_Rate(50);												 // ���ò�����Ϊ50Hz
	}
	else
		return 1;
	return 0;
}
/**
 * @brief ����MPU6050�����Ǵ����������̷�Χ
 * @param fsr 0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
 * @return 0,���óɹ�������,����ʧ��
 */
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG, fsr << 3); // ���������������̷�Χ
}

/**
 * @brief ����MPU6050���ٶȴ����������̷�Χ
 * @param fsr 0,��2g;1,��4g;2,��8g;3,��16g
 * @return 0,���óɹ�������,����ʧ��
 */
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG, fsr << 3); // ���ü��ٶȴ����������̷�Χ
}

/**
 * @brief ����MPU6050�����ֵ�ͨ�˲���
 * @param lpf ���ֵ�ͨ�˲�Ƶ��(Hz)
 * @return 0,���óɹ�������,����ʧ��
 */
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data = 0;
	if (lpf >= 188)
		data = 1;
	else if (lpf >= 98)
		data = 2;
	else if (lpf >= 42)
		data = 3;
	else if (lpf >= 20)
		data = 4;
	else if (lpf >= 10)
		data = 5;
	else
		data = 6;
	return MPU_Write_Byte(MPU_CFG_REG, data); // �������ֵ�ͨ�˲���
}

/**
 * @brief ����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
 * @param rate 4~1000(Hz)
 * @return 0,���óɹ�������,����ʧ��
 */
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if (rate > 1000)
		rate = 1000;
	if (rate < 4)
		rate = 4;
	data = 1000 / rate - 1;
	data = MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data); // �������ֵ�ͨ�˲���
	return MPU_Set_LPF(rate / 2);											// �Զ�����LPFΪ�����ʵ�һ��
}

/**
 * @brief �õ��¶�ֵ
 * @param
 * @return ��ǰ���϶�
 */
float MPU_Get_Temperature(void)
{
	uint8_t buf[2];
	short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf);
	raw = ((uint16_t)buf[0] << 8) | buf[1];
	temp = 36.53 + ((double)raw) / 340;
	return temp;
}

/**
 * @brief �õ�������ֵ(��λΪdeg/s)(������)
 * @param IMU
 * @return 0,�ɹ�������,�������
 */
uint8_t MPU_Get_Gyroscope(IMU_info *IMU)
{
	uint8_t buf[6], res;
	res = MPU_Read_Len(MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
	if (res == 0)
	{
		IMU->gx = ((int16_t)((uint16_t)buf[0] << 8) | buf[1]) * 0.061; // 2000 / 32768
		IMU->gy = ((int16_t)((uint16_t)buf[2] << 8) | buf[3]) * 0.061;
		IMU->gz = ((int16_t)((uint16_t)buf[4] << 8) | buf[5]) * 0.061;
	}
	return res;
}

/**
 * @brief �õ����ٶ�ֵ(��λΪm/s^2)(������)
 * @param IMU
 * @return 0,�ɹ�������,�������
 */
uint8_t MPU_Get_Accelerometer(IMU_info *IMU)
{
	uint8_t buf[6], res;
	res = MPU_Read_Len(MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
	if (res == 0)
	{
		IMU->ax = ((int16_t)((uint16_t)buf[0] << 8) | buf[1]) * 0.000598; // 2 * 9.8 / 32768
		IMU->ay = ((int16_t)((uint16_t)buf[2] << 8) | buf[3]) * 0.000598;
		IMU->az = ((int16_t)((uint16_t)buf[4] << 8) | buf[5]) * 0.000598;
	}
	return res;
}
// IIC����д
// addr:������ַ
// reg:�Ĵ�����ַ
// len:д�볤��
// buf:������
// ����ֵ:0,����
//     ����,�������
uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t sta;
	sta = HAL_I2C_Mem_Write(&hi2c1, addr << 1, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 100);
	if (sta == HAL_OK)
		return 0;
	else
		return 1;
}
// IIC������
// addr:������ַ
// reg:Ҫ��ȡ�ļĴ�����ַ
// len:Ҫ��ȡ�ĳ���
// buf:��ȡ�������ݴ洢��
// ����ֵ:0,����
//     ����,�������
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t sta;
	sta = HAL_I2C_Mem_Read(&hi2c1, addr << 1, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 100);
	if (sta == HAL_OK)
		return 0;
	else
		return 1;
}
// IICдһ���ֽ�
// reg:�Ĵ�����ַ
// data:����
// ����ֵ:0,����
//     ����,�������
uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data)
{
	uint8_t sta;
	sta = HAL_I2C_Mem_Write(&hi2c1, MPU_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if (sta != HAL_OK)
		return 1;
	else
		return 0;
}

// IIC��һ���ֽ�
// reg:�Ĵ�����ַ
// ����ֵ:����������
uint8_t MPU_Read_Byte(uint8_t reg)
{
	uint8_t dat, sta;

	sta = HAL_I2C_Mem_Read(&hi2c1, MPU_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, &dat, 1, 100);
	if (sta != HAL_OK)
		return 1;
	else
		return dat;
}
