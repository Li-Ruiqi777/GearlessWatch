#ifndef __CST816_H
#define __CST816_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "main.h"

/****************************************IIC驱动部分*******************************/
#define CST816_ADDR 0x15 // 写地址0x2A		读地址0x2B

#define CST816_SCL_Clr() HAL_GPIO_WritePin(TP_SCL_GPIO_Port, TP_SCL_Pin, GPIO_PIN_RESET) // SCL=SCLK
#define CST816_SCL_Set() HAL_GPIO_WritePin(TP_SCL_GPIO_Port, TP_SCL_Pin, GPIO_PIN_SET)

#define CST816_SDA_Get() HAL_GPIO_ReadPin(TP_SDA_GPIO_Port, TP_SDA_Pin)
#define CST816_SDA_Clr() HAL_GPIO_WritePin(TP_SDA_GPIO_Port, TP_SDA_Pin, GPIO_PIN_RESET)
#define CST816_SDA_Set() HAL_GPIO_WritePin(TP_SDA_GPIO_Port, TP_SDA_Pin, GPIO_PIN_SET)

void Delay_us(uint32_t nus);
void CST816_SDA_IN(void);
void CST816_SDA_OUT(void);
void CST816_IIC_ACK(void);
void CST816_IIC_NACK(void);
uint8_t CST816_IIC_Wait_ACK(void);
void CST816_IIC_Start(void);
void CST816_IIC_Stop(void);
void CST816_IIC_SendByte(uint8_t byte);
void CST816_IIC_WriteREG(uint8_t reg, uint8_t date);
uint8_t CST816_IIC_ReadREG(uint8_t reg);
uint8_t CST816_IIC_RecvByte(void);

/*************************************寄存器操作部分********************************/
typedef enum
{
	Down = 0,
	Up,
} State;

typedef struct
{
	uint16_t X_Pos; // X坐标
	uint16_t Y_Pos; // Y坐标
	State TP_sta;		// 记录触摸状态
} CST816_Info;

//
// CST816寄存器
#define GestureID 0x01		 // 手势寄存器
#define FingerNum 0x02		 // 手指数量
#define XposH 0x03				 // x高四位
#define XposL 0x04				 // x低八位
#define YposH 0x05				 // y高四位
#define YposL 0x06				 // y低八位
#define ChipID 0xA7				 // 芯片型号
#define MotionMask 0xEC		 // 触发动作
#define AutoSleepTime 0xF9 // 自动休眠
#define IrqCrl 0xFA				 // 中断控制
#define AutoReset 0xFB		 // 无手势休眠
#define LongPressTime 0xFC // 长按休眠
#define DisAutoSleep 0xFE	 // 使能低功耗模式

//

// MotionMask
#define EnConLR 1 << 2 // 连续左右滑动
#define EnConUD 1 << 1 // 连续上下滑动
#define EnDClick 1		 // 双击

// IRQCtrl								//中断低电平
#define EnTest 1 << 7		// 中断测试
#define EnToucj 1 << 6	// 触摸触发中断
#define EnChange 1 << 5 // 触摸状态变化触发中断
#define EnMotion 1 << 4 // 检测到手势触发中断
#define OnceWLP 1				// 检测到长按触发中断

// AutoSleep
#define ENABLE 1
#define FALSE 0

extern __IO CST816_Info CST816_Instance;

void CST816_Init(void);
uint8_t CST816_Get_ChipID(void);
void CST816_Get_XY(void);			// 得到当前触摸的坐标
uint8_t CST816_Get_Sta(void); // 更新触摸状态

void CST816_Set_MotionMask(uint8_t Motion);
void CST816_Set_IrqCtrl(uint8_t IRQCtrl);
void CST816_Set_AutoRST(uint8_t time);			// 设置多长时间无触摸自动复位，默认5s,写0则禁用功能
void CST816_Set_LongPressRST(uint8_t time); // 设置长按多长时间复位，默认为10s，写0禁用功能
void CST816_Set_AutoSleep(uint8_t en);			// 是否使能自动进入低功耗模式

#ifdef __cplusplus
}
#endif

#endif