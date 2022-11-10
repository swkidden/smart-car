/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
 【平    台】北京龙邱智能科技TC264DA核心板
 【编    写】ZYF/chiusir
 【E-mail  】chiusir@163.com
 【软件版本】V1.1 版权所有，单位使用请先联系授权
 【最后更新】2020年10月28日
 【相关信息参考下列地址】
 【网    站】http:// www.lqist.cn
 【淘宝店铺】http:// longqiu.taobao.com
 ------------------------------------------------
 【dev.env.】AURIX Development Studio1.2.2及以上版本
 【Target 】 TC264DA/TC264D
 【Crystal】 20.000Mhz
 【SYS PLL】 200MHz
 ________________________________________________________________
 基于iLLD_1_0_1_11_0底层程序,

 使用例程的时候，建议采用没有空格的英文路径，
 除了CIF为TC264DA独有外，其它的代码兼容TC264D
 本库默认初始化了EMEM：512K，如果用户使用TC264D，注释掉EMEM_InitConfig()初始化函数。
 工程下\Libraries\iLLD\TC26B\Tricore\Cpu\CStart\IfxCpu_CStart0.c第164行左右。
 ________________________________________________________________

 QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
 *  备    注：TC264 有两个CCU6模块  每个模块有两个独立定时器  触发定时器中断
 QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

#include "LQ_CCU6.h"

#include <CompilerTasking.h>

#include "../APP/LQ_GPIO_LED.h"
#include "LQ_GPT12_ENC.h"
#include "LQ_ImageProcess.h"
#include "LQ_MotorServo.h"
#include "LQ_PID.h"
#include "Zhou_config.h"
volatile sint16 ECPULSE1 = 0;   // 编码器1
volatile sint16 ECPULSE2 = 0;   // 编码器2
volatile sint32 RAllPulse = 0;  // 编码器1累计值（里程）
// extern IfxCpu_spinLock mutexCpu0motorIsOk;
extern sint16 g_sSteeringError;  //转向误差
extern uint8_t AKMQZ;
extern uint8_t Servo_P;
extern uint8_t Servo_D;
extern sint16 g_sSteeringError_last;
volatile float Target_Speed = initspeed;  // 速度全局变量
volatile float L_Real_Speed = 0;
volatile float R_Real_Speed = 0;
IFX_INTERRUPT(CCU60_CH0_IRQHandler, CCU60_VECTABNUM, CCU60_CH0_PRIORITY);
IFX_INTERRUPT(CCU60_CH1_IRQHandler, CCU60_VECTABNUM, CCU60_CH1_PRIORITY);
IFX_INTERRUPT(CCU61_CH0_IRQHandler, CCU61_VECTABNUM, CCU61_CH0_PRIORITY);
IFX_INTERRUPT(CCU61_CH1_IRQHandler, CCU61_VECTABNUM, CCU61_CH1_PRIORITY);

/** CCU6中断CPU标号 */
const uint8 Ccu6IrqVectabNum[2] = {CCU60_VECTABNUM, CCU61_VECTABNUM};

/** CCU6中断优先级 */
const uint8 Ccu6IrqPriority[4] = {CCU60_CH0_PRIORITY, CCU60_CH1_PRIORITY, CCU61_CH0_PRIORITY, CCU61_CH1_PRIORITY};

/** CCU6中断服务函数地址 */
const void *Ccu6IrqFuncPointer[4] = {&CCU60_CH0_IRQHandler, &CCU60_CH1_IRQHandler, &CCU61_CH0_IRQHandler, &CCU61_CH1_IRQHandler};

extern pid_param_t LSpeed_PID;  //声明外部变量：左轮PID（未用PID）
extern pid_param_t RSpeed_PID;  //声明外部变量：右轮PID（未用PID）
extern sint16 MotorDuty1;       //声明外部变量：右轮PWM
extern sint16 MotorDuty2;       //声明外部变量：左轮PWM
/***********************************************************************************************/
/********************************CCU6外部中断  服务函数******************************************/
/***********************************************************************************************/

/*************************************************************************
 *  函数名称：void CCU60_CH0_IRQHandler(void)
 *  功能说明：
 *  参数说明：无
 *  函数返回：无
 *  修改时间：2020年3月30日
 *  备    注：CCU60_CH0使用的中断服务函数
 *************************************************************************/
void CCU60_CH0_IRQHandler(void) {
  /* 开启CPU中断  否则中断不可嵌套 */
  //   IfxCpu_enableInterrupts();
  // 清除中断标志
  IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_t12PeriodMatch);
  /* 用户代码 */
  //元素信息处理
  ServoDuty = (g_sSteeringError * Servo_P + (g_sSteeringError - g_sSteeringError_last) * Servo_D) / 10;  //偏差放大
  g_sSteeringError_last = g_sSteeringError;
  if (ServoDuty > 80) ServoDuty = 80;       //偏差限幅 0.766
  if (ServoDuty < -80) ServoDuty = -80;     //偏差限幅
  ServoCtrl(Servo_Center_Mid - ServoDuty);  //舵机打角
}

/*************************************************************************
 *  函数名称：void CCU60_CH1_IRQHandler(void)
 *  功能说明：
 *  参数说明：无
 *  函数返回：无
 *  修改时间：2020年3月30日
 *  备    注：CCU60_CH1使用的中断服务函数
 *************************************************************************/
void CCU60_CH1_IRQHandler(void) {
  /* 开启CPU中断  否则中断不可嵌套 */
  IfxCpu_enableInterrupts();
  // 清除中断标志
  IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_t13PeriodMatch);
  /* 获取编码器值 */

  static volatile float Target_Speed_L = 150;
  static volatile float Target_Speed_R = 150;
  Target_Speed_L = Target_Speed;
  Target_Speed_R = Target_Speed;                 //没有打角的时候左右轮目标速度就是总目标速度
  ECPULSE1 = ENC_GetCounter(ENC2_InPut_P33_7);   // 右轮电机 母板上编码器1，小车前进为正值
  ECPULSE2 = -ENC_GetCounter(ENC4_InPut_P02_8);  // 左轮电机 母板上编码器2，小车前进为负值
  /* 临界区 */                                   //如果没有要求读写顺序临界区不重要 双核同时访问同一内存时由内存管理单元进行切换
                                                 // if (IfxCpu_setSpinLock(&mutexCpu0motorIsOk,2)) {
  RAllPulse += ECPULSE2;                         //轮胎转一圈为1200脉冲 6.5cm直径 一圈1200脉冲的长度为20.4cm
                                                 //   IfxCpu_resetSpinLock(&mutexCpu0motorIsOk);
                                                 //  }
  /* 临界区 */
  // g_sSteeringError_front
  if (AKMQZ == 0) {
    AKMQZ = 40;
  }
  Target_Speed_L = Target_Speed * (1 + ZHOUCHANG * Diffspeed * ServoDuty / AKMQZ / AKMQZ);  // 20行40cm 35行20cm
  Target_Speed_R = Target_Speed * (1 - ZHOUCHANG * Diffspeed * ServoDuty / AKMQZ / AKMQZ);

  //差速处理，差速比例，最好不要修改 弯道差速并减速  ServoDuty最大值为80
  // if (ServoDuty > 0)  //右转
  // {
  //   Target_Speed_R = Target_Speed - ServoDuty / 8* Decoefficient- Target_Speed/300*ServoDuty / 8* Diffspeed ;  //内圈
  //   Target_Speed_L = Target_Speed - ServoDuty / 8 * Decoefficient;              //外圈
  // } else                                                                        //左转
  // {
  //   Target_Speed_R = Target_Speed + ServoDuty / 8 * Decoefficient;              //外圈
  //   Target_Speed_L = Target_Speed + ServoDuty / 8  * Decoefficient+Target_Speed/300*ServoDuty / 8* Diffspeed;  //内圈
  // }
  R_Real_Speed = (20.4 * ECPULSE1 / 1200 * 1000 / 5);  // 1000ms/5ms中断时间
  L_Real_Speed = (20.4 * ECPULSE2 / 1200 * 1000 / 5);  // 1000ms/5ms中断时间
  MotorDuty2 = PidIncCtrl(&LSpeed_PID, (Target_Speed_L - L_Real_Speed));
  MotorDuty1 = PidIncCtrl(&RSpeed_PID, (Target_Speed_R - R_Real_Speed));
  MotorCtrl(MotorDuty1, MotorDuty2);
}

/*************************************************************************
 *  函数名称：void CCU61_CH0_IRQHandler(void)
 *  功能说明：
 *  参数说明：无
 *  函数返回：无
 *  修改时间：2020年3月30日
 *  备    注：CCU61_CH0使用的中断服务函数
 *************************************************************************/
void CCU61_CH0_IRQHandlerXXXXXXXXXXXXXXX(void) {
  /* 开启CPU中断  否则中断不可嵌套 */
  IfxCpu_enableInterrupts();

  // 清除中断标志
  IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU61, IfxCcu6_InterruptSource_t12PeriodMatch);

  /* 用户代码 */
  /* 获取编码器值 */
}

void CCU61_CH0_IRQHandler(void) {
  /* 开启CPU中断  否则中断不可嵌套 */
  IfxCpu_enableInterrupts();

  // 清除中断标志
  IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU61, IfxCcu6_InterruptSource_t12PeriodMatch);

  /* 用户代码 */
  /* 获取编码器值 */
  ECPULSE1 = ENC_GetCounter(ENC2_InPut_P33_7);  // 左电机 母板上编码器1，小车前进为负值
  ECPULSE2 = ENC_GetCounter(ENC4_InPut_P02_8);  // 右电机 母板上编码器2，小车前进为正值
  RAllPulse += ECPULSE2;                        //
}
/*************************************************************************
 *  函数名称：void CCU61_CH1_IRQHandler(void)
 *  功能说明：
 *  参数说明：无
 *  函数返回：无
 *  修改时间：2020年3月30日
 *  备    注：CCU61_CH1使用的中断服务函数
 *************************************************************************/
void CCU61_CH1_IRQHandler(void) {
  /* 开启CPU中断  否则中断不可嵌套 */
  IfxCpu_enableInterrupts();

  // 清除中断标志
  IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU61, IfxCcu6_InterruptSource_t13PeriodMatch);

  /* 用户代码 */
  LED_Ctrl(LED0, RVS);  // 电平翻转,LED闪烁
}

/*************************************************************************
 *  函数名称：CCU6_InitConfig CCU6
 *  功能说明：定时器周期中断初始化
 *  参数说明：ccu6    ： ccu6模块            CCU60 、 CCU61
 *  参数说明：channel ： ccu6模块通道  CCU6_Channel0 、 CCU6_Channel1
 *  参数说明：us      ： ccu6模块  中断周期时间  单位us
 *  函数返回：无
 *  修改时间：2020年3月30日
 *  备    注：    CCU6_InitConfig(CCU60, CCU6_Channel0, 100);  // 100us进入一次中断
 *************************************************************************/
void CCU6_InitConfig(CCU6_t ccu6, CCU6_Channel_t channel, uint32 us) {
  IfxCcu6_Timer_Config timerConfig;

  Ifx_CCU6 *module = IfxCcu6_getAddress((IfxCcu6_Index)ccu6);

  uint8 Index = ccu6 * 2 + channel;

  uint32 period = 0;

  uint64 clk = 0;

  /* 关闭中断 */
  boolean interrupt_state = disableInterrupts();

  IfxCcu6_Timer_initModuleConfig(&timerConfig, module);

  clk = IfxScuCcu_getSpbFrequency();

  /* 设置时钟频率  */
  uint8 i = 0;
  while (i++ < 16) {
    period = (uint32)(clk * us / 1000000);
    if (period < 0xffff) {
      break;
    } else {
      clk = clk / 2;
    }
  }
  switch (channel) {
    case CCU6_Channel0:
      timerConfig.timer = IfxCcu6_TimerId_t12;
      timerConfig.interrupt1.source = IfxCcu6_InterruptSource_t12PeriodMatch;
      timerConfig.interrupt1.serviceRequest = IfxCcu6_ServiceRequest_1;
      timerConfig.base.t12Frequency = (float)clk;
      timerConfig.base.t12Period = period;  // 设置定时中断
      timerConfig.clock.t12countingInputMode = IfxCcu6_CountingInputMode_internal;
      timerConfig.timer12.counterValue = 0;
      timerConfig.interrupt1.typeOfService = Ccu6IrqVectabNum[ccu6];
      timerConfig.interrupt1.priority = Ccu6IrqPriority[Index];
      break;

    case CCU6_Channel1:
      timerConfig.timer = IfxCcu6_TimerId_t13;
      timerConfig.interrupt2.source = IfxCcu6_InterruptSource_t13PeriodMatch;
      timerConfig.interrupt2.serviceRequest = IfxCcu6_ServiceRequest_2;
      timerConfig.base.t13Frequency = (float)clk;
      timerConfig.base.t13Period = period;
      timerConfig.clock.t13countingInputMode = IfxCcu6_CountingInputMode_internal;
      timerConfig.timer13.counterValue = 0;
      timerConfig.interrupt2.typeOfService = Ccu6IrqVectabNum[ccu6];
      timerConfig.interrupt2.priority = Ccu6IrqPriority[Index];
      break;
  }

  timerConfig.trigger.t13InSyncWithT12 = FALSE;

  IfxCcu6_Timer Ccu6Timer;

  IfxCcu6_Timer_initModule(&Ccu6Timer, &timerConfig);

  IfxCpu_Irq_installInterruptHandler((void *)Ccu6IrqFuncPointer[Index], Ccu6IrqPriority[Index]);  // 配置中断函数和中断号

  restoreInterrupts(interrupt_state);

  IfxCcu6_Timer_start(&Ccu6Timer);
}

/*************************************************************************
 *  函数名称：CCU6_DisableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel)
 *  功能说明：停止CCU6通道中断
 *  参数说明：ccu6    ： ccu6模块            CCU60 、 CCU61
 *  参数说明：channel ： ccu6模块通道  CCU6_Channel0 、 CCU6_Channel1
 *  函数返回：无
 *  修改时间：2020年5月6日
 *  备    注：
 *************************************************************************/
void CCU6_DisableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel) {
  Ifx_CCU6 *module = IfxCcu6_getAddress((IfxCcu6_Index)ccu6);
  IfxCcu6_clearInterruptStatusFlag(module, (IfxCcu6_InterruptSource)(7 + channel * 2));
  IfxCcu6_disableInterrupt(module, (IfxCcu6_InterruptSource)(7 + channel * 2));
}

/*************************************************************************
 *  函数名称：CCU6_DisableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel)
 *  功能说明：使能CCU6通道中断
 *  参数说明：ccu6    ： ccu6模块            CCU60 、 CCU61
 *  参数说明：channel ： ccu6模块通道  CCU6_Channel0 、 CCU6_Channel1
 *  函数返回：无
 *  修改时间：2020年5月6日
 *  备    注：
 *************************************************************************/
void CCU6_EnableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel) {
  Ifx_CCU6 *module = IfxCcu6_getAddress((IfxCcu6_Index)ccu6);
  IfxCcu6_clearInterruptStatusFlag(module, (IfxCcu6_InterruptSource)(7 + channel * 2));
  IfxCcu6_enableInterrupt(module, (IfxCcu6_InterruptSource)(7 + channel * 2));
}
