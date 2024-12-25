#include "CST816.h" 

#define TOUCH_OFFSET_Y 0
#define REVERSE 1

CST816_Info	CST816_Instance;

iic_bus_t CST816_dev =
{
	.IIC_SDA_PORT = GPIOB,
	.IIC_SCL_PORT = GPIOB,
	.IIC_SDA_PIN = GPIO_PIN_7,
	.IIC_SCL_PIN = GPIO_PIN_6,
};

/*
*********************************************************************************************************
*	函 数 名: CST816_GPIO_Init
*	功能说明: CST816 GPIO口初始化
*	形    参：none
*	返 回 值: none
*********************************************************************************************************
*/
void CST816_GPIO_Init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/* 初始化复位引脚 */
	GPIO_InitStructure.Pin = TOUCH_RST_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(TOUCH_RST_PORT, &GPIO_InitStructure);
	
	HAL_GPIO_WritePin(TOUCH_RST_PORT,TOUCH_RST_PIN,GPIO_PIN_SET);
	
	/* 初始化中断引脚,不用中断的话屏蔽掉 */
	
	/* 初始化I2C引脚 */
	IICInit(&CST816_dev);
	
	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	//IICStop(&CST816_dev);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Init
*	功能说明: CST816初始化
*	形    参：none
*	返 回 值: none
*********************************************************************************************************
*/
void CST816_Init(void)
{
	CST816_GPIO_Init();
	CST816_Config_AutoSleepTime(5);
}


/*
*********************************************************************************************************
*	函 数 名: CST816_IIC_ReadREG
*	功能说明: 读取触摸屏单个寄存器的数据
*	形    参：reg：寄存器地址
*	返 回 值: 返回寄存器存储的数据
*********************************************************************************************************
*/
uint8_t CST816_IIC_ReadREG(uint8_t addr)
{
	return IIC_Read_One_Byte(&CST816_dev,Device_Addr,addr);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_IIC_WriteREG
*	功能说明: 向触摸屏的寄存器写入数据
*	形    参：addr：寄存器地址
*						dat:	写入的数据
*	返 回 值: 返回寄存器存储的数据
*********************************************************************************************************
*/
void CST816_IIC_WriteREG(uint8_t addr, uint8_t dat)
{
	IIC_Write_One_Byte(&CST816_dev,Device_Addr,addr,dat);
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_RESET
*	功能说明: 触摸屏复位
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_RESET(void)
{
	TOUCH_RST_0;
	HAL_Delay(10);
	TOUCH_RST_1;
	HAL_Delay(100);
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_READ_X
*	功能说明: 读取触摸屏在触摸时的坐标值
*	形    参：无
*	返 回 值: 无 （数据存储在CST816_Instance结构体中）
*********************************************************************************************************
*/
void CST816_Get_XY_AXIS(void)
{
	uint8_t DAT[4];
	IIC_Read_Multi_Byte(&CST816_dev,Device_Addr,XposH,4,DAT);
	CST816_Instance.X_Pos=((DAT[0]&0x0F)<<8)|DAT[1];//(temp[0]&0X0F)<<4|
	CST816_Instance.Y_Pos=((DAT[2]&0x0F)<<8)|DAT[3] + TOUCH_OFFSET_Y;//(temp[2]&0X0F)<<4|
	#if REVERSE //翻转
		CST816_Instance.X_Pos = 239 - CST816_Instance.X_Pos;
		CST816_Instance.Y_Pos = 279 - CST816_Instance.Y_Pos;
	#endif
//	int x_temp = CST816_Instance.X_Pos;
//	int y_temp = CST816_Instance.Y_Pos;
//	#if REVERSE //翻转
//		CST816_Instance.X_Pos = y_temp;
//		CST816_Instance.Y_Pos = 239 - x_temp;
//	#endif
}


/*
*********************************************************************************************************
*	函 数 名: CST816_Get_FingerNum
*	功能说明: 读取触摸屏的手指触摸个数,0xFF为睡眠
*	形    参：无
*	返 回 值: 返回芯片ID
*********************************************************************************************************
*/
uint8_t CST816_Get_FingerNum(void)
{
	return CST816_IIC_ReadREG(FingerNum);
}


/*
*********************************************************************************************************
*	函 数 名: CST816_Get_ChipID
*	功能说明: 读取触摸屏的芯片ID
*	形    参：无
*	返 回 值: 返回芯片ID
*********************************************************************************************************
*/
uint8_t CST816_Get_ChipID(void)
{
	return CST816_IIC_ReadREG(ChipID);
}


/*
*********************************************************************************************************
*	函 数 名: CST816_Config_MotionMask
*	功能说明: 使能连续动作（连续左右滑动，连续上下滑动，双击）
*	形    参：mode：模式(5种)
*	返 回 值: 无
*	注    意：使能连续动作会增加响应时间
*********************************************************************************************************
*/
void CST816_Config_MotionMask(uint8_t mode)
{
	CST816_IIC_WriteREG(MotionMask,mode);
}


/*
*********************************************************************************************************
*	函 数 名: CST816_Config_AutoSleepTime
*	功能说明: 规定time内无触摸，自动进入低功耗模式
*	形    参：time：时间(s)
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Config_AutoSleepTime(uint8_t time)
{
	CST816_IIC_WriteREG(AutoSleepTime,time);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Sleep
*	功能说明: 进入睡眠，无触摸唤醒功能
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Sleep(void)
{
	CST816_IIC_WriteREG(SleepMode,0x03);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Wakeup
*	功能说明: 唤醒
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Wakeup(void)
{
	CST816_RESET();
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Config_MotionSlAngle
*	功能说明: 手势检测滑动分区角度控制。Angle=tan(c)*10 c为以x轴正方向为基准的角度。
*	形    参：x_right_y_up_angle：角度值
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Config_MotionSlAngle(uint8_t x_right_y_up_angle)
{
	CST816_IIC_WriteREG(MotionSlAngle,x_right_y_up_angle);
}


/*
*********************************************************************************************************
*	函 数 名: CST816_Config_NorScanPer
*	功能说明: 正常快速检测周期配置函数。
*						此值会影响到LpAutoWakeTime和AutoSleepTime。 
*						单位10ms，可选值：1～30。默认值为1。
*	形    参：Period：周期值
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Config_NorScanPer(uint8_t Period)
{
	if(Period >= 30)
		Period = 30;
	CST816_IIC_WriteREG(NorScanPer,Period);
}


/*
*********************************************************************************************************
*	函 数 名: CST816_Config_IrqPluseWidth
*	功能说明: 中断低脉冲输出宽度配置函数
*	形    参：Period：周期值
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Config_IrqPluseWidth(uint8_t Width)
{
	if(Width >= 200)
		Width = 200;
	CST816_IIC_WriteREG(IrqPluseWidth,Width);
}


/*
*********************************************************************************************************
*	函 数 名: CST816_Config_NorScanPer
*	功能说明: 低功耗扫描唤醒门限配置函数。越小越灵敏。默认值48
*	形    参：TH：门限值
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Config_LpScanTH(uint8_t TH)
{
	CST816_IIC_WriteREG(LpScanTH,TH);
}
