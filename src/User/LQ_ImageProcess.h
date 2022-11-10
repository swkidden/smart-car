/*
 * @LastEditors: Dark white
 * @LastEditTime: 2022-07-19 22:49:17
 * @FilePath: \wansai2\src\User\LQ_ImageProcess.h
 * @Description: 
 * 
 * Copyright (c) 2022 by Dark white, All Rights Reserved. 
 */
/*
 * @LastEditors: Dark white
 * @LastEditTime: 2022-07-11 09:41:47
 * @FilePath: \LQ_TC264DA_CameraC1224\src\User\LQ_ImageProcess.h
 * @Description:
 *
 * Copyright (c) 2022 by Dark white, All Rights Reserved.
 */
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨�������������ܿƼ�TC264DA���İ�
����    д��zyf/chiusir
��E-mail  ��chiusir@163.com
�������汾��V1.0
�������¡�2020��4��10��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://longqiu.taobao.com
------------------------------------------------
��dev.env.��Hightec4.9.3/Tasking6.3�����ϰ汾
��Target �� TC264DA
��Crystal�� 20.000Mhz
��SYS PLL�� 200MHz
����iLLD_1_0_1_11_0�ײ����
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#ifndef SRC_APPSW_TRICORE_USER_LQ_IMAGEPROCESS_H_
#define SRC_APPSW_TRICORE_USER_LQ_IMAGEPROCESS_H_
#include <Platform_Types.h>
#include <stdint.h>
#include <stdio.h>

#include "LQ_CAMERA.h"
#include "include.h"

typedef enum {
  NO = 0,
  FlagRoundabout_R,
  FlagRoundabout_R_1,
  FlagRoundabout_R_2,
  FlagRoundabout_R_3,
  FlagRoundabout_R_4,
  FlagRoundabout_R_5,
  FlagRoundabout_L,
  FlagRoundabout_L_1,
  FlagRoundabout_L_2,
  FlagRoundabout_L_3,
  FlagRoundabout_L_4,
  FlagRoundabout_L_5,
} FlagRoundabout;

typedef enum {
  Y_NO = 0,
  FlagFork_0,
  FlagFork_1,
  FlagFork_2,
  FlagFork_3,
  FlagFork_4,
} FlagFork;
typedef enum { no_start = 0, start, over } game_status;

extern uint8_t UpdowmSide[2][LCDW];
extern uint8_t ImageSide[LCDH][2];
extern uint8_t g_ucFlagW;  
extern uint8_t leftdown_y ;//拐点坐标
extern uint8_t leftdown_x ; 
extern uint8_t rightdown_y ;
extern uint8_t rightdown_x ;
extern uint8_t xianhuan_L ;
extern uint8_t xianhuan_R ;
void CameraCar(void);  //��⺯��

//���ұ��߻����ж�
uint8_t RoundaboutGetArc(uint8_t imageSide[LCDH][2], uint8_t status, uint8_t num, uint8_t* index);

//���ߴ���
void RoadNoSideProcess(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[LCDH][2], uint8_t mode, uint8_t lineIndex);

//��ʾ
void TFT_Show_Camera_Info(void);

//��ȡ���±���
uint8_t UpdownSideGet(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[2][LCDW]);

//��ȡת�۵����
uint8_t TurnNumSideGet(void);

//��ȡ���ұ���
uint8_t ImageGetSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[LCDH][2]);

//���»�ȡ��/�ұ���
void RoundaboutGetSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageSide[LCDH][2], uint8_t status);

//���»�ȡ��/�±���
void Roundabout_Get_UpDowmSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageSide[2][LCDW], uint8_t status);

//���ߴ���
void ImageAddingLine(uint8_t imageSide[LCDH][2], uint8_t status, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY);
/**
 * @description: 直道长度计算
 * @return {*}
 */
void StraightLine(uint8_t imageOut[2][LCDW], uint8_t arry);

#endif /* SRC_APPSW_TRICORE_USER_LQ_IMAGEPROCESS_H_ */
