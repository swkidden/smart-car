/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨�������������ܿƼ�TC264DA���İ�
����    д��ZYF/chiusir
��E-mail  ��chiusir@163.com
�������汾��V1.1 ��Ȩ���У���λʹ��������ϵ��Ȩ
�������¡�2020��10��28��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://longqiu.taobao.com
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
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
*  ��    ע��TC264 ������CCU6ģ��  ÿ��ģ��������������ʱ��  ������ʱ���ж�
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

#ifndef SRC_APPSW_TRICORE_DRIVER_LQ_CCU6_H_
#define SRC_APPSW_TRICORE_DRIVER_LQ_CCU6_H_

#include "Cpu/Std/Platform_Types.h"
#include "stdint.h"
#include <Bsp.h>
#include <IfxCcu6.h>
#include <IfxCcu6_cfg.h>
#include <IfxCcu6_reg.h>
#include <IfxCcu6_regdef.h>
#include <IfxCcu6_Timer.h>
#include <IfxCpu.h>
#include <IfxCpu_Irq.h>
#include <IfxScuCcu.h>


extern volatile sint16 ECPULSE1;          //�ٶ�ȫ�ֱ���
extern volatile sint16 ECPULSE2;          //�ٶ�ȫ�ֱ���
extern volatile sint32 RAllPulse;         //�ٶ�ȫ�ֱ���

extern volatile float Target_Speed;          // �ٶ�ȫ�ֱ���
//extern volatile sint16 Target_Speed2;          // �ٶ�ȫ�ֱ���

/**
 * 	CCU6ģ��ö��
 */
typedef enum
{
	CCU60,
	CCU61
}CCU6_t;

/**
 * 	CCU6ͨ��ö��
 */
typedef enum
{
	CCU6_Channel0,
	CCU6_Channel1,
}CCU6_Channel_t;

/** CCU6��ʱ���ж� CCU60  channel0 �жϷ��������ȼ�   ��Χ��1-255   ����Խ�� ���ȼ�Խ��  ע�����ȼ���Ҫ�ظ� */
#define  CCU60_CH0_PRIORITY    10

/** CCU6��ʱ���ж� CCU60  channel1 �жϷ��������ȼ�   ��Χ��1-255   ����Խ�� ���ȼ�Խ��  ע�����ȼ���Ҫ�ظ� */
#define  CCU60_CH1_PRIORITY    81

/** CCU6��ʱ���ж� CCU60   �жϹ��ĸ��ں˹����� ��Χ��0��CPU0   1��CPU1   3��DMA*/
#define  CCU60_VECTABNUM       1



/** CCU6��ʱ���ж� CCU61  channel0 �жϷ��������ȼ�   ��Χ��1-255   ����Խ�� ���ȼ�Խ��  ע�����ȼ���Ҫ�ظ� */
#define  CCU61_CH0_PRIORITY    82

/** CCU6��ʱ���ж� CCU61  channel1 �жϷ��������ȼ�   ��Χ��1-255   ����Խ�� ���ȼ�Խ��  ע�����ȼ���Ҫ�ظ� */
#define  CCU61_CH1_PRIORITY    83

/** CCU6��ʱ���ж� CCU61  �жϹ��ĸ��ں˹����� ��Χ��0��CPU0   1��CPU1   3��DMA*/
#define  CCU61_VECTABNUM       0


/*************************************************************************
*  �������ƣ�CCU6_InitConfig CCU6
*  ����˵������ʱ�������жϳ�ʼ��
*  ����˵����ccu6    �� ccu6ģ��            CCU60 �� CCU61
*  ����˵����channel �� ccu6ģ��ͨ��  CCU6_Channel0 �� CCU6_Channel1
*  ����˵����us      �� ccu6ģ��  �ж�����ʱ��  ��λus
*  �������أ���
*  �޸�ʱ�䣺2020��3��30��
*  ��    ע��
*************************************************************************/
void CCU6_InitConfig(CCU6_t ccu6, CCU6_Channel_t channel, unsigned long us);

void CCU6_DisableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel);

void CCU6_EnableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel);

#endif /* SRC_APPSW_TRICORE_DRIVER_LQ_CCU6_H_ */
