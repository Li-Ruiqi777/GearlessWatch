#include "main.h"
#include "CST816.h"
#include "math.h"

void Delay_us(uint32_t nus)
{
  uint32_t Delay = nus * 2;
  do
  {
    __NOP();//延时1/84M(s)
  } while (Delay--);
}

void CST816_SDA_IN(void)
{
  TP_SDA_GPIO_Port->MODER &= ~(GPIO_MODER_MODER0 << (3 * 2U));            // 先把对应Pin的2bit置0,其他Pin都不变
  TP_SDA_GPIO_Port->MODER |= ((GPIO_MODE_INPUT & GPIO_MODE) << (3 * 2U)); // 把对应Pin的2bit赋值成传入的值
}
void CST816_SDA_OUT(void)
{
  TP_SDA_GPIO_Port->MODER &= ~(GPIO_MODER_MODER0 << (3 * 2U));                // 先把对应Pin的2bit置0,其他Pin都不变
  TP_SDA_GPIO_Port->MODER |= ((GPIO_MODE_OUTPUT_PP & GPIO_MODE) << (3 * 2U)); // 把对应Pin的2bit赋值成传入的值
}

__IO CST816_Info CST816_Instance; // 创建CST816实例。

void CST816_IIC_Start(void)
{
  CST816_SDA_OUT();
  CST816_SDA_Set();
  CST816_SCL_Set();
  Delay_us(4);
  CST816_SDA_Clr();
  Delay_us(4);
  CST816_SCL_Clr();
}

void CST816_IIC_Stop(void)
{
  CST816_SDA_OUT();

  CST816_SCL_Clr();
  CST816_SDA_Clr();
  Delay_us(4);
  CST816_SCL_Set();
  CST816_SDA_Set();
  Delay_us(4);
}

void CST816_IIC_ACK(void)
{
  CST816_SDA_OUT();

  CST816_SCL_Clr();
  CST816_SDA_Clr();
  Delay_us(2);
  CST816_SCL_Set();
  Delay_us(2);
  CST816_SCL_Clr();
}
void CST816_IIC_NACK(void)
{
  CST816_SDA_OUT();
  CST816_SCL_Clr();

  CST816_SDA_Set();
  Delay_us(2);
  CST816_SCL_Set();
  Delay_us(2);
  CST816_SCL_Clr();
}

uint8_t CST816_IIC_Wait_ACK(void)
{
  uint8_t t = 0;

  CST816_SDA_IN();
  CST816_SDA_Set();
  Delay_us(1);
  CST816_SCL_Set();
  Delay_us(1);
  while (CST816_SDA_Get())
  {
    t++;
    if (t > 250)
    {
      CST816_IIC_Stop();
      return 1;
    }
  }
  CST816_SCL_Clr();
  return 0;
}

void CST816_IIC_SendByte(uint8_t byte)
{
  uint8_t BitCnt;
  CST816_SDA_OUT();
  CST816_SCL_Clr();
  for (BitCnt = 0; BitCnt < 8; BitCnt++) // 要传送的数据长度为8位
  {
    //		Delay_us(5);
    if (byte & 0x80)
      CST816_SDA_Set(); // 判断发送位
    else
      CST816_SDA_Clr();
    byte <<= 1;
    Delay_us(2);
    CST816_SCL_Set();
    Delay_us(2);
    CST816_SCL_Clr();
    Delay_us(2);
  }
}
uint8_t CST816_IIC_RecvByte(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  uint8_t retc;
  uint8_t BitCnt;
  retc = 0;
  CST816_SDA_IN(); // 置数据线为输入方式
  for (BitCnt = 0; BitCnt < 8; BitCnt++)
  {
    CST816_SCL_Clr();
    Delay_us(2);
    CST816_SCL_Set(); // 置时钟线为高使数据线上数据有效
    retc = retc << 1;
    if (CST816_SDA_Get())
      retc++; // 读数据位,接收的数据位放入retc中
    Delay_us(1);
  }
  return (retc);
}

void CST816_IIC_WriteREG(uint8_t reg, uint8_t date)
{
  CST816_IIC_Start();
  CST816_IIC_SendByte(0x2A);
  CST816_IIC_Wait_ACK();
  CST816_IIC_SendByte(reg);
  CST816_IIC_Wait_ACK();
  CST816_IIC_SendByte(date);
  CST816_IIC_Wait_ACK();
  CST816_IIC_Stop();
  Delay_us(10);
}

uint8_t CST816_IIC_ReadREG(uint8_t reg)
{
  uint8_t date;
  CST816_IIC_Start();
  CST816_IIC_SendByte(0x2A);
  CST816_IIC_Wait_ACK();
  CST816_IIC_SendByte(reg);
  CST816_IIC_Wait_ACK();

  CST816_IIC_Start();
  CST816_IIC_SendByte(0x2B);
  CST816_IIC_Wait_ACK();
  date = CST816_IIC_RecvByte();
  CST816_IIC_ACK();
  CST816_IIC_Stop();

  return date;
}

void CST816_Init(void)
{
}
uint8_t CST816_Get_ChipID()
{
  return CST816_IIC_ReadREG(ChipID);
}

void CST816_Get_XY(void)
{
  uint8_t temp[4] = {0, 0, 0, 0};
  uint16_t x, y;

  CST816_IIC_Start();
  CST816_IIC_SendByte(0x2A);
  CST816_IIC_Wait_ACK();
  CST816_IIC_SendByte(0x03);
  CST816_IIC_Wait_ACK();

  /* 这里必须连读4bit,这芯片有个bug,如果你用iic读一次数据并结束此次iic通信,
      它所有寄存器的值都会被清空,因此要连续读
  */
  CST816_IIC_Start();
  CST816_IIC_SendByte(0x2B);
  CST816_IIC_Wait_ACK();
  temp[0] = CST816_IIC_RecvByte();
  CST816_IIC_ACK();
  temp[1] = CST816_IIC_RecvByte();
  CST816_IIC_ACK();
  temp[2] = CST816_IIC_RecvByte();
  CST816_IIC_ACK();
  temp[3] = CST816_IIC_RecvByte();
  CST816_IIC_ACK();
  CST816_IIC_Stop();

  x = (uint16_t)((temp[0] & 0x0F) << 8) | temp[1]; //(temp[0]&0X0F)<<4|
  y = (uint16_t)((temp[2] & 0x0F) << 8) | temp[3]; //(temp[2]&0X0F)<<4|
  if (x < 240 && y < 280)
  {
    CST816_Instance.X_Pos = x;
    CST816_Instance.Y_Pos = y;
  }
}

uint8_t CST816_Get_Sta(void)
{
  uint8_t sta;

  CST816_IIC_Start();
  CST816_IIC_SendByte(0x2A);
  CST816_IIC_Wait_ACK();
  CST816_IIC_SendByte(0x02);
  CST816_IIC_Wait_ACK();
  CST816_IIC_Start();

  CST816_IIC_SendByte(0x2B);
  CST816_IIC_Wait_ACK();
  sta = CST816_IIC_RecvByte();
  CST816_IIC_ACK();
  CST816_IIC_Stop();

  /* 没手指放上去的话，sta=0或255 */
  if (sta != 255 & sta != 0)
  {
    CST816_Instance.TP_sta = Down;
    return 1;
  }
  else
  {
    CST816_Instance.TP_sta = Up;
    return 0;
  }
}

void CST816_Set_MotionMask(uint8_t Motion)
{
  CST816_IIC_WriteREG(MotionMask, Motion);
}

void CST816_Set_IrqCtrl(uint8_t IRQCtrl)
{
  CST816_IIC_WriteREG(IrqCrl, IRQCtrl);
}

void CST816_Set_AutoSleep(uint8_t en) // 是否使能自动进入低功耗模式
{
  if (en == FALSE)
  {
    CST816_IIC_WriteREG(DisAutoSleep, FALSE);
  }
  else
  {
    CST816_IIC_WriteREG(DisAutoSleep, ENABLE);
    CST816_IIC_WriteREG(AutoSleepTime, en);
  }
}
void CST816_Set_AutoRST(uint8_t time) // 设置多长时间无触摸自动复位，默认5s,写0则禁用功能
{
  CST816_IIC_WriteREG(AutoReset, time);
}
void CST816_Set_LongPressRST(uint8_t time) // 设置长按多长时间复位，默认为10s，写0禁用功能
{
  CST816_IIC_WriteREG(LongPressTime, time);
}
