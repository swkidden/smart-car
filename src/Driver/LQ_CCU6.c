/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
 ��ƽ    ̨�������������ܿƼ�TC264DA���İ�
 ����    д��ZYF/chiusir
 ��E-mail  ��chiusir@163.com
 ������汾��V1.1 ��Ȩ���У���λʹ��������ϵ��Ȩ
 �������¡�2020��10��28��
 �������Ϣ�ο����е�ַ��
 ����    վ��http:// www.lqist.cn
 ���Ա����̡�http:// longqiu.taobao.com
 ------------------------------------------------
 ��dev.env.��AURIX Development Studio1.2.2�����ϰ汾
 ��Target �� TC264DA/TC264D
 ��Crystal�� 20.000Mhz
 ��SYS PLL�� 200MHz
 ________________________________________________________________
 ����iLLD_1_0_1_11_0�ײ����,

 ʹ�����̵�ʱ�򣬽������û�пո��Ӣ��·����
 ����CIFΪTC264DA�����⣬�����Ĵ������TC264D
 ����Ĭ�ϳ�ʼ����EMEM��512K������û�ʹ��TC264D��ע�͵�EMEM_InitConfig()��ʼ��������
 ������\Libraries\iLLD\TC26B\Tricore\Cpu\CStart\IfxCpu_CStart0.c��164�����ҡ�
 ________________________________________________________________

 QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
 *  ��    ע��TC264 ������CCU6ģ��  ÿ��ģ��������������ʱ��  ������ʱ���ж�
 QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

#include "LQ_CCU6.h"

#include <CompilerTasking.h>

#include "../APP/LQ_GPIO_LED.h"
#include "LQ_GPT12_ENC.h"
#include "LQ_ImageProcess.h"
#include "LQ_MotorServo.h"
#include "LQ_PID.h"
#include "Zhou_config.h"
volatile sint16 ECPULSE1 = 0;   // ������1
volatile sint16 ECPULSE2 = 0;   // ������2
volatile sint32 RAllPulse = 0;  // ������1�ۼ�ֵ����̣�
// extern IfxCpu_spinLock mutexCpu0motorIsOk;
extern sint16 g_sSteeringError;  //ת�����
extern uint8_t AKMQZ;
extern uint8_t Servo_P;
extern uint8_t Servo_D;
extern sint16 g_sSteeringError_last;
volatile float Target_Speed = initspeed;  // �ٶ�ȫ�ֱ���
volatile float L_Real_Speed = 0;
volatile float R_Real_Speed = 0;
IFX_INTERRUPT(CCU60_CH0_IRQHandler, CCU60_VECTABNUM, CCU60_CH0_PRIORITY);
IFX_INTERRUPT(CCU60_CH1_IRQHandler, CCU60_VECTABNUM, CCU60_CH1_PRIORITY);
IFX_INTERRUPT(CCU61_CH0_IRQHandler, CCU61_VECTABNUM, CCU61_CH0_PRIORITY);
IFX_INTERRUPT(CCU61_CH1_IRQHandler, CCU61_VECTABNUM, CCU61_CH1_PRIORITY);

/** CCU6�ж�CPU��� */
const uint8 Ccu6IrqVectabNum[2] = {CCU60_VECTABNUM, CCU61_VECTABNUM};

/** CCU6�ж����ȼ� */
const uint8 Ccu6IrqPriority[4] = {CCU60_CH0_PRIORITY, CCU60_CH1_PRIORITY, CCU61_CH0_PRIORITY, CCU61_CH1_PRIORITY};

/** CCU6�жϷ�������ַ */
const void *Ccu6IrqFuncPointer[4] = {&CCU60_CH0_IRQHandler, &CCU60_CH1_IRQHandler, &CCU61_CH0_IRQHandler, &CCU61_CH1_IRQHandler};

extern pid_param_t LSpeed_PID;  //�����ⲿ����������PID��δ��PID��
extern pid_param_t RSpeed_PID;  //�����ⲿ����������PID��δ��PID��
extern sint16 MotorDuty1;       //�����ⲿ����������PWM
extern sint16 MotorDuty2;       //�����ⲿ����������PWM
/***********************************************************************************************/
/********************************CCU6�ⲿ�ж�  ������******************************************/
/***********************************************************************************************/

/*************************************************************************
 *  �������ƣ�void CCU60_CH0_IRQHandler(void)
 *  ����˵����
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2020��3��30��
 *  ��    ע��CCU60_CH0ʹ�õ��жϷ�����
 *************************************************************************/
void CCU60_CH0_IRQHandler(void) {
  /* ����CPU�ж�  �����жϲ���Ƕ�� */
  //   IfxCpu_enableInterrupts();
  // ����жϱ�־
  IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_t12PeriodMatch);
  /* �û����� */
  //Ԫ����Ϣ����
  ServoDuty = (g_sSteeringError * Servo_P + (g_sSteeringError - g_sSteeringError_last) * Servo_D) / 10;  //ƫ��Ŵ�
  g_sSteeringError_last = g_sSteeringError;
  if (ServoDuty > 80) ServoDuty = 80;       //ƫ���޷� 0.766
  if (ServoDuty < -80) ServoDuty = -80;     //ƫ���޷�
  ServoCtrl(Servo_Center_Mid - ServoDuty);  //������
}

/*************************************************************************
 *  �������ƣ�void CCU60_CH1_IRQHandler(void)
 *  ����˵����
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2020��3��30��
 *  ��    ע��CCU60_CH1ʹ�õ��жϷ�����
 *************************************************************************/
void CCU60_CH1_IRQHandler(void) {
  /* ����CPU�ж�  �����жϲ���Ƕ�� */
  IfxCpu_enableInterrupts();
  // ����жϱ�־
  IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_t13PeriodMatch);
  /* ��ȡ������ֵ */

  static volatile float Target_Speed_L = 150;
  static volatile float Target_Speed_R = 150;
  Target_Speed_L = Target_Speed;
  Target_Speed_R = Target_Speed;                 //û�д�ǵ�ʱ��������Ŀ���ٶȾ�����Ŀ���ٶ�
  ECPULSE1 = ENC_GetCounter(ENC2_InPut_P33_7);   // ���ֵ�� ĸ���ϱ�����1��С��ǰ��Ϊ��ֵ
  ECPULSE2 = -ENC_GetCounter(ENC4_InPut_P02_8);  // ���ֵ�� ĸ���ϱ�����2��С��ǰ��Ϊ��ֵ
  /* �ٽ��� */                                   //���û��Ҫ���д˳���ٽ�������Ҫ ˫��ͬʱ����ͬһ�ڴ�ʱ���ڴ����Ԫ�����л�
                                                 // if (IfxCpu_setSpinLock(&mutexCpu0motorIsOk,2)) {
  RAllPulse += ECPULSE2;                         //��̥תһȦΪ1200���� 6.5cmֱ�� һȦ1200����ĳ���Ϊ20.4cm
                                                 //   IfxCpu_resetSpinLock(&mutexCpu0motorIsOk);
                                                 //  }
  /* �ٽ��� */
  // g_sSteeringError_front
  if (AKMQZ == 0) {
    AKMQZ = 40;
  }
  Target_Speed_L = Target_Speed * (1 + ZHOUCHANG * Diffspeed * ServoDuty / AKMQZ / AKMQZ);  // 20��40cm 35��20cm
  Target_Speed_R = Target_Speed * (1 - ZHOUCHANG * Diffspeed * ServoDuty / AKMQZ / AKMQZ);

  //���ٴ������ٱ�������ò�Ҫ�޸� ������ٲ�����  ServoDuty���ֵΪ80
  // if (ServoDuty > 0)  //��ת
  // {
  //   Target_Speed_R = Target_Speed - ServoDuty / 8* Decoefficient- Target_Speed/300*ServoDuty / 8* Diffspeed ;  //��Ȧ
  //   Target_Speed_L = Target_Speed - ServoDuty / 8 * Decoefficient;              //��Ȧ
  // } else                                                                        //��ת
  // {
  //   Target_Speed_R = Target_Speed + ServoDuty / 8 * Decoefficient;              //��Ȧ
  //   Target_Speed_L = Target_Speed + ServoDuty / 8  * Decoefficient+Target_Speed/300*ServoDuty / 8* Diffspeed;  //��Ȧ
  // }
  R_Real_Speed = (20.4 * ECPULSE1 / 1200 * 1000 / 5);  // 1000ms/5ms�ж�ʱ��
  L_Real_Speed = (20.4 * ECPULSE2 / 1200 * 1000 / 5);  // 1000ms/5ms�ж�ʱ��
  MotorDuty2 = PidIncCtrl(&LSpeed_PID, (Target_Speed_L - L_Real_Speed));
  MotorDuty1 = PidIncCtrl(&RSpeed_PID, (Target_Speed_R - R_Real_Speed));
  MotorCtrl(MotorDuty1, MotorDuty2);
}

/*************************************************************************
 *  �������ƣ�void CCU61_CH0_IRQHandler(void)
 *  ����˵����
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2020��3��30��
 *  ��    ע��CCU61_CH0ʹ�õ��жϷ�����
 *************************************************************************/
void CCU61_CH0_IRQHandlerXXXXXXXXXXXXXXX(void) {
  /* ����CPU�ж�  �����жϲ���Ƕ�� */
  IfxCpu_enableInterrupts();

  // ����жϱ�־
  IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU61, IfxCcu6_InterruptSource_t12PeriodMatch);

  /* �û����� */
  /* ��ȡ������ֵ */
}

void CCU61_CH0_IRQHandler(void) {
  /* ����CPU�ж�  �����жϲ���Ƕ�� */
  IfxCpu_enableInterrupts();

  // ����жϱ�־
  IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU61, IfxCcu6_InterruptSource_t12PeriodMatch);

  /* �û����� */
  /* ��ȡ������ֵ */
  ECPULSE1 = ENC_GetCounter(ENC2_InPut_P33_7);  // ���� ĸ���ϱ�����1��С��ǰ��Ϊ��ֵ
  ECPULSE2 = ENC_GetCounter(ENC4_InPut_P02_8);  // �ҵ�� ĸ���ϱ�����2��С��ǰ��Ϊ��ֵ
  RAllPulse += ECPULSE2;                        //
}
/*************************************************************************
 *  �������ƣ�void CCU61_CH1_IRQHandler(void)
 *  ����˵����
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2020��3��30��
 *  ��    ע��CCU61_CH1ʹ�õ��жϷ�����
 *************************************************************************/
void CCU61_CH1_IRQHandler(void) {
  /* ����CPU�ж�  �����жϲ���Ƕ�� */
  IfxCpu_enableInterrupts();

  // ����жϱ�־
  IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU61, IfxCcu6_InterruptSource_t13PeriodMatch);

  /* �û����� */
  LED_Ctrl(LED0, RVS);  // ��ƽ��ת,LED��˸
}

/*************************************************************************
 *  �������ƣ�CCU6_InitConfig CCU6
 *  ����˵������ʱ�������жϳ�ʼ��
 *  ����˵����ccu6    �� ccu6ģ��            CCU60 �� CCU61
 *  ����˵����channel �� ccu6ģ��ͨ��  CCU6_Channel0 �� CCU6_Channel1
 *  ����˵����us      �� ccu6ģ��  �ж�����ʱ��  ��λus
 *  �������أ���
 *  �޸�ʱ�䣺2020��3��30��
 *  ��    ע��    CCU6_InitConfig(CCU60, CCU6_Channel0, 100);  // 100us����һ���ж�
 *************************************************************************/
void CCU6_InitConfig(CCU6_t ccu6, CCU6_Channel_t channel, uint32 us) {
  IfxCcu6_Timer_Config timerConfig;

  Ifx_CCU6 *module = IfxCcu6_getAddress((IfxCcu6_Index)ccu6);

  uint8 Index = ccu6 * 2 + channel;

  uint32 period = 0;

  uint64 clk = 0;

  /* �ر��ж� */
  boolean interrupt_state = disableInterrupts();

  IfxCcu6_Timer_initModuleConfig(&timerConfig, module);

  clk = IfxScuCcu_getSpbFrequency();

  /* ����ʱ��Ƶ��  */
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
      timerConfig.base.t12Period = period;  // ���ö�ʱ�ж�
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

  IfxCpu_Irq_installInterruptHandler((void *)Ccu6IrqFuncPointer[Index], Ccu6IrqPriority[Index]);  // �����жϺ������жϺ�

  restoreInterrupts(interrupt_state);

  IfxCcu6_Timer_start(&Ccu6Timer);
}

/*************************************************************************
 *  �������ƣ�CCU6_DisableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel)
 *  ����˵����ֹͣCCU6ͨ���ж�
 *  ����˵����ccu6    �� ccu6ģ��            CCU60 �� CCU61
 *  ����˵����channel �� ccu6ģ��ͨ��  CCU6_Channel0 �� CCU6_Channel1
 *  �������أ���
 *  �޸�ʱ�䣺2020��5��6��
 *  ��    ע��
 *************************************************************************/
void CCU6_DisableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel) {
  Ifx_CCU6 *module = IfxCcu6_getAddress((IfxCcu6_Index)ccu6);
  IfxCcu6_clearInterruptStatusFlag(module, (IfxCcu6_InterruptSource)(7 + channel * 2));
  IfxCcu6_disableInterrupt(module, (IfxCcu6_InterruptSource)(7 + channel * 2));
}

/*************************************************************************
 *  �������ƣ�CCU6_DisableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel)
 *  ����˵����ʹ��CCU6ͨ���ж�
 *  ����˵����ccu6    �� ccu6ģ��            CCU60 �� CCU61
 *  ����˵����channel �� ccu6ģ��ͨ��  CCU6_Channel0 �� CCU6_Channel1
 *  �������أ���
 *  �޸�ʱ�䣺2020��5��6��
 *  ��    ע��
 *************************************************************************/
void CCU6_EnableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel) {
  Ifx_CCU6 *module = IfxCcu6_getAddress((IfxCcu6_Index)ccu6);
  IfxCcu6_clearInterruptStatusFlag(module, (IfxCcu6_InterruptSource)(7 + channel * 2));
  IfxCcu6_enableInterrupt(module, (IfxCcu6_InterruptSource)(7 + channel * 2));
}
