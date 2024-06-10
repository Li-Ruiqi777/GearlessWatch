#ifndef _INV_MPU_H_
#define _INV_MPU_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#include "stdint.h"
#include "mpu6050.h"

// 定义输出速度
#define DEFAULT_MPU_HZ (100) // 100Hz
#define MY_MPU_HZ (10)
#define INV_X_GYRO (0x40)
#define INV_Y_GYRO (0x20)
#define INV_Z_GYRO (0x10)
#define INV_XYZ_GYRO (INV_X_GYRO | INV_Y_GYRO | INV_Z_GYRO)
#define INV_XYZ_ACCEL (0x08)
#define INV_XYZ_COMPASS (0x01)

// 移植官方MSP430DMP驱动过来
struct int_param_s
{
    void (*cb)(void);
    uint16_t pin;
    uint8_t lp_exit;
    uint8_t active_low;
};

#define MPU_INT_STATUS_DATA_READY (0x0001)
#define MPU_INT_STATUS_DMP (0x0002)
#define MPU_INT_STATUS_PLL_READY (0x0004)
#define MPU_INT_STATUS_I2C_MST (0x0008)
#define MPU_INT_STATUS_FIFO_OVERFLOW (0x0010)
#define MPU_INT_STATUS_ZMOT (0x0020)
#define MPU_INT_STATUS_MOT (0x0040)
#define MPU_INT_STATUS_FREE_FALL (0x0080)
#define MPU_INT_STATUS_DMP_0 (0x0100)
#define MPU_INT_STATUS_DMP_1 (0x0200)
#define MPU_INT_STATUS_DMP_2 (0x0400)
#define MPU_INT_STATUS_DMP_3 (0x0800)
#define MPU_INT_STATUS_DMP_4 (0x1000)
#define MPU_INT_STATUS_DMP_5 (0x2000)

/* Set up APIs */
int mpu_init(void);
int mpu_init_slave(void);
int mpu_set_bypass(uint8_t bypass_on);

/* Configuration APIs */
int mpu_lp_accel_mode(uint8_t rate);
int mpu_lp_motion_interrupt(uint16_t thresh, uint8_t time,
                            uint8_t lpa_freq);
int mpu_set_int_level(uint8_t active_low);
int mpu_set_int_latched(uint8_t enable);

int mpu_set_dmp_state(uint8_t enable);
int mpu_get_dmp_state(uint8_t *enabled);

int mpu_get_lpf(uint16_t *lpf);
int mpu_set_lpf(uint16_t lpf);

int mpu_get_gyro_fsr(uint16_t *fsr);
int mpu_set_gyro_fsr(uint16_t fsr);

int mpu_get_accel_fsr(uint8_t *fsr);
int mpu_set_accel_fsr(uint8_t fsr);

int mpu_get_compass_fsr(uint16_t *fsr);

int mpu_get_gyro_sens(float *sens);
int mpu_get_accel_sens(uint16_t *sens);

int mpu_get_sample_rate(uint16_t *rate);
int mpu_set_sample_rate(uint16_t rate);
int mpu_get_compass_sample_rate(uint16_t *rate);
int mpu_set_compass_sample_rate(uint16_t rate);

int mpu_get_fifo_config(uint8_t *sensors);
int mpu_configure_fifo(uint8_t sensors);

int mpu_get_power_state(uint8_t *power_on);
int mpu_set_sensors(uint8_t sensors);

int mpu_set_accel_bias(const long *accel_bias);

/* Data getter/setter APIs */
int mpu_get_gyro_reg(short *data, unsigned long *timestamp);
int mpu_get_accel_reg(short *data, unsigned long *timestamp);
int mpu_get_compass_reg(short *data, unsigned long *timestamp);
int mpu_get_temperature(long *data, unsigned long *timestamp);

int mpu_get_int_status(short *status);
int mpu_read_fifo(short *gyro, short *accel, unsigned long *timestamp,
                  uint8_t *sensors, uint8_t *more);
int mpu_read_fifo_stream(uint16_t length, uint8_t *data,
                         uint8_t *more);
int mpu_reset_fifo(void);

int mpu_write_mem(uint16_t mem_addr, uint16_t length,
                  uint8_t *data);
int mpu_read_mem(uint16_t mem_addr, uint16_t length,
                 uint8_t *data);
int mpu_load_firmware(uint16_t length, const uint8_t *firmware,
                      uint16_t start_addr, uint16_t sample_rate);

int mpu_reg_dump(void);
int mpu_read_reg(uint8_t reg, uint8_t *data);
int mpu_run_self_test(long *gyro, long *accel);
int mpu_register_tap_cb(void (*func)(uint8_t, uint8_t));

// 自行添加的一些函数
void mget_ms(unsigned long *time);
uint16_t inv_row_2_scale(const signed char *row);
uint16_t inv_orientation_matrix_to_scalar(const signed char *mtx);
uint8_t run_self_test(void);
uint8_t mpu_dmp_init(void);
uint8_t mpu_dmp_get_data(IMU_info *IMU);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _INV_MPU_H_ */
