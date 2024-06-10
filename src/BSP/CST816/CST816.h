#ifndef __CST816_H
#define __CST816_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "main.h"

/****************************************IIC��������*******************************/
#define CST816_ADDR 0x15 // д��ַ0x2A		����ַ0x2B

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

/*************************************�Ĵ�����������********************************/
typedef enum
{
	Down = 0,
	Up,
} State;

typedef struct
{
	uint16_t X_Pos; // X����
	uint16_t Y_Pos; // Y����
	State TP_sta;		// ��¼����״̬
} CST816_Info;

//
// CST816�Ĵ���
#define GestureID 0x01		 // ���ƼĴ���
#define FingerNum 0x02		 // ��ָ����
#define XposH 0x03				 // x����λ
#define XposL 0x04				 // x�Ͱ�λ
#define YposH 0x05				 // y����λ
#define YposL 0x06				 // y�Ͱ�λ
#define ChipID 0xA7				 // оƬ�ͺ�
#define MotionMask 0xEC		 // ��������
#define AutoSleepTime 0xF9 // �Զ�����
#define IrqCrl 0xFA				 // �жϿ���
#define AutoReset 0xFB		 // ����������
#define LongPressTime 0xFC // ��������
#define DisAutoSleep 0xFE	 // ʹ�ܵ͹���ģʽ

//

// MotionMask
#define EnConLR 1 << 2 // �������һ���
#define EnConUD 1 << 1 // �������»���
#define EnDClick 1		 // ˫��

// IRQCtrl								//�жϵ͵�ƽ
#define EnTest 1 << 7		// �жϲ���
#define EnToucj 1 << 6	// ���������ж�
#define EnChange 1 << 5 // ����״̬�仯�����ж�
#define EnMotion 1 << 4 // ��⵽���ƴ����ж�
#define OnceWLP 1				// ��⵽���������ж�

// AutoSleep
#define ENABLE 1
#define FALSE 0

extern __IO CST816_Info CST816_Instance;

void CST816_Init(void);
uint8_t CST816_Get_ChipID(void);
void CST816_Get_XY(void);			// �õ���ǰ����������
uint8_t CST816_Get_Sta(void); // ���´���״̬

void CST816_Set_MotionMask(uint8_t Motion);
void CST816_Set_IrqCtrl(uint8_t IRQCtrl);
void CST816_Set_AutoRST(uint8_t time);			// ���ö೤ʱ���޴����Զ���λ��Ĭ��5s,д0����ù���
void CST816_Set_LongPressRST(uint8_t time); // ���ó����೤ʱ�临λ��Ĭ��Ϊ10s��д0���ù���
void CST816_Set_AutoSleep(uint8_t en);			// �Ƿ�ʹ���Զ�����͹���ģʽ

#ifdef __cplusplus
}
#endif

#endif