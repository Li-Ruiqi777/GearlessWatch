#include "mpu6050.h"
#include "i2c.h"

/**
 * @brief 初始化MPU6050
 * @param
 * @return 返回值:0,成功
 * 				其他,错误代码
 */
uint8_t MPU_Init(void)
{
	__IO uint8_t addr;
	__IO uint8_t ret;
	ret = MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80); // 复位MPU6050
	delay_ms(100);
	ret = MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00); // 唤醒MPU6050
	MPU_Set_Gyro_Fsr(3);													 // 陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);													 // 加速度传感器,±2g
	MPU_Set_Rate(50);															 // 设置采样率50Hz
	ret = MPU_Write_Byte(MPU_INT_EN_REG, 0X00);		 // 关闭所有中断
	ret = MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00); // I2C主模式关闭
	ret = MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);	 // 关闭FIFO
	ret = MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80); // INT引脚低电平有效
	addr = MPU_Read_Byte(MPU_DEVICE_ID_REG);
	if (addr == MPU_ADDR) // 器件ID正确
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01); // 设置CLKSEL,PLL X轴为参考
		MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00); // 加速度与陀螺仪都工作
		MPU_Set_Rate(50);												 // 设置采样率为50Hz
	}
	else
		return 1;
	return 0;
}
/**
 * @brief 设置MPU6050陀螺仪传感器满量程范围
 * @param fsr 0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
 * @return 0,设置成功、其他,设置失败
 */
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG, fsr << 3); // 设置陀螺仪满量程范围
}

/**
 * @brief 设置MPU6050加速度传感器满量程范围
 * @param fsr 0,±2g;1,±4g;2,±8g;3,±16g
 * @return 0,设置成功、其他,设置失败
 */
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG, fsr << 3); // 设置加速度传感器满量程范围
}

/**
 * @brief 设置MPU6050的数字低通滤波器
 * @param lpf 数字低通滤波频率(Hz)
 * @return 0,设置成功、其他,设置失败
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
	return MPU_Write_Byte(MPU_CFG_REG, data); // 设置数字低通滤波器
}

/**
 * @brief 设置MPU6050的采样率(假定Fs=1KHz)
 * @param rate 4~1000(Hz)
 * @return 0,设置成功、其他,设置失败
 */
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if (rate > 1000)
		rate = 1000;
	if (rate < 4)
		rate = 4;
	data = 1000 / rate - 1;
	data = MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data); // 设置数字低通滤波器
	return MPU_Set_LPF(rate / 2);											// 自动设置LPF为采样率的一半
}

/**
 * @brief 得到温度值
 * @param
 * @return 当前摄氏度
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
 * @brief 得到陀螺仪值(单位为deg/s)(带符号)
 * @param IMU
 * @return 0,成功、其他,错误代码
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
 * @brief 得到加速度值(单位为m/s^2)(带符号)
 * @param IMU
 * @return 0,成功、其他,错误代码
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
// IIC连续写
// addr:器件地址
// reg:寄存器地址
// len:写入长度
// buf:数据区
// 返回值:0,正常
//     其他,错误代码
uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t sta;
	sta = HAL_I2C_Mem_Write(&hi2c1, addr << 1, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 100);
	if (sta == HAL_OK)
		return 0;
	else
		return 1;
}
// IIC连续读
// addr:器件地址
// reg:要读取的寄存器地址
// len:要读取的长度
// buf:读取到的数据存储区
// 返回值:0,正常
//     其他,错误代码
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t sta;
	sta = HAL_I2C_Mem_Read(&hi2c1, addr << 1, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 100);
	if (sta == HAL_OK)
		return 0;
	else
		return 1;
}
// IIC写一个字节
// reg:寄存器地址
// data:数据
// 返回值:0,正常
//     其他,错误代码
uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data)
{
	uint8_t sta;
	sta = HAL_I2C_Mem_Write(&hi2c1, MPU_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if (sta != HAL_OK)
		return 1;
	else
		return 0;
}

// IIC读一个字节
// reg:寄存器地址
// 返回值:读到的数据
uint8_t MPU_Read_Byte(uint8_t reg)
{
	uint8_t dat, sta;

	sta = HAL_I2C_Mem_Read(&hi2c1, MPU_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, &dat, 1, 100);
	if (sta != HAL_OK)
		return 1;
	else
		return dat;
}
