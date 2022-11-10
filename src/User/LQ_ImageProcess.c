/*
 * @LastEditors: Dark white
 * @LastEditTime: 2022-07-22 16:01:22
 * @FilePath: \jingsu\src\User\LQ_ImageProcess.c
 * @Description:
 *
 * Copyright (c) 2022 by Dark white, All Rights Reserved.
 */
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨�������������ܿƼ�TC264DA���İ�
����    д��zyf/chiusir
��E-mail  ��chiusir@163.com
������汾��V1.0
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

#include "LQ_ImageProcess.h"

#include <Platform_Types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../APP/LQ_CAMERA.h"
#include "../APP/LQ_GPIO_KEY.h"
#include "../APP/LQ_GPIO_LED.h"
#include "../APP/LQ_TFT18.h"
#include "../Driver/LQ_ADC.h"
#include "../Driver/LQ_CCU6.h"
#include "../Driver/LQ_STM.h"
#include "LQ_GPIO.h"
#include "LQ_GPT12_ENC.h"
#include "LQ_Inductor.h"
#include "LQ_MotorServo.h"
#include "LQ_PID.h"
#include "LQ_STM.h"
#include "LQ_UART.h"
#include "Zhou_config.h"
#include "guaidian.h"
uint8_t leftdown_turn_flag = 0;   //����ת�۵��־
uint8_t rightdown_turn_flag = 0;  //����ת�۵��־
uint8_t leftup_turn_flag = 0;     //����ת�۵��־
uint8_t rightup_turn_flag = 0;    //����ת�۵��־
uint8_t xianhuan_L = 0;           //�߻��е�������
uint8_t xianhuan_R = 0;           //�߻��е�������
uint8_t leftdown_y = 0;           //�յ�����
uint8_t leftdown_x = 0;
uint8_t rightdown_y = 0;
uint8_t rightdown_x = 0;
uint8_t ImageSide[LCDH][2];    //���ұ������� 0�� 1��
uint8_t UpdowmSide[2][LCDW];   //���±������� 0�� 1��
uint8_t up_turn[10];           //�ϱ���ת�۵����� ����������
uint8_t left_turn[10];         //�����ת�۵����� ����������
uint8_t right_turn[10];        //�ұ���ת�۵����� ����������
uint8_t num_left_lost = 0;     //��ඪ�߸���
uint8_t num_right_lost = 0;    //�Ҳඪ�߸���
uint16_t Aver_left_lost = 0;   //��ඪ�ߵ�ƽ������
uint16_t Aver_right_lost = 0;  //�Ҳඪ�ߵ�ƽ������
uint8_t num_all_lost = 0;      //����ͬ�ж��߸���
uint8_t num_up_turn = 0;       //�ϱ���ת�۵����
uint8_t num_left_turn = 0;     //�����ת�۵����
uint8_t num_right_turn = 0;    //�ұ���ת�۵����
uint8_t anleg_up_turn = 0;     //ת�۵�Ƕ�
uint8_t anleg_left_turn = 0;
uint8_t anleg_right_turn = 0;
uint8_t num_black_white = 0;  //�ڰ�������
game_status game = no_start;  //����״̬
uint8_t count = 0;            //�����ߴ���
uint8_t straightline = 0;     //ֱ������
uint8_t threerightguaidian_x = 93;
uint8_t threerightguaidian_y = 0;
uint8_t threeleftguaidian_x = 93;
uint8_t threeleftguaidian_y = 0;
/**  @brief    ת�����  */
sint16 g_sSteeringError = 0;
uint8_t AKMQZ = 0;  //�����о��복��������ڰ�����ת��
sint16 g_sSteeringError_last = 0;
/**  @brief    ���߱�־λ  */
uint8_t g_ucIsNoSide = 0;
/**  @brief    ������־λ  */
FlagRoundabout g_ucFlagRoundabout = NO;

/**  @brief    ʮ�ֱ�־λ  */
uint8_t g_ucFlagCross = 0;

/**  @brief    �����߱�־λ  */
uint8_t g_ucFlagZebra = 0;

/**  @brief    Y�Ͳ�ڱ�־λ  */
FlagFork g_ucFlagFork = Y_NO;
uint8_t g_ucForkNum = 0;

/**  @brief    T�Ͳ�ڱ�־λ  */
uint8_t g_ucFlagT = 0;
/* �����־λ */
uint8_t g_ucFlagW = 0;       // 1Ϊ����� 2Ϊ�����
pid_param_t BalDirgyro_PID;  // ����PID

float Servo_P = 16;
float Servo_D = 10;
char txt[30];
uint32_t Time = 0;
uint32_t LastTime = 0;
uint16_t Fps = 0;
/*!
 * @brief    ������
 *
 * @param
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2020/6/28 ������
 */
int Get_angle(uint8 ax, uint8 ay, uint8 bx, uint8 by, uint8 cx, uint8 cy) {
  int abx = ax - bx;
  int aby = ay - by;
  int cbx = cx - bx;
  int cby = cy - by;
  int ab_muti_cb = abx * cbx + aby * cby;
  int dist_ab = sqrt(abx * abx + aby * aby);
  int dist_cb = sqrt(cbx * cbx + cby * cby);
  int cosvalue = ab_muti_cb / (dist_ab * dist_cb);
  return (int)(acos(cosvalue) * 180 / 3.14);
}
void TFTSPI_BinRoadSide(uint8_t imageOut[LCDH][2]) {
  uint8_t i = 0;

  for (i = 0; i < ROAD_START_ROW; i++) {
    TFTSPI_Draw_Dot(imageOut[i][0], i, u16RED);

    TFTSPI_Draw_Dot(imageOut[i][1], i, u16GREEN);
  }
}

void TFTSPI_BinRoad_UpdownSide(uint8_t imageOut[2][LCDW]) {
  uint8_t i = 0;

  for (i = 0; i < LCDW; i++) {
    TFTSPI_Draw_Dot(i, imageOut[0][i], u16YELLOW);

    // TFTSPI_Draw_Dot(i, imageOut[1][i], u16ORANGE);
  }
}
/**
 * @description: ���ϱ���ת�۵�
 * @param {uint8_t} up_turn
 * @return {*}
 */
void TFTSPI_BinRoad_Upturn(uint8_t up_turn[]) {
  if (num_up_turn < 10)
    for (uint8_t i = 0; i < num_up_turn; i++) {
      TFTSPI_Draw_Circle(up_turn[i], UpdowmSide[0][up_turn[i]], 5, u16WHITE);
    }
}
/**
 * @description: �������ת�۵�
 * @param {uint8_t} up_turn
 * @return {*}
 */
void TFTSPI_BinRoad_Leftturn(uint8_t left_turn[]) {
  if (num_left_turn < 10)
    for (uint8_t i = 0; i < num_left_turn; i++) {
      TFTSPI_Draw_Circle(ImageSide[left_turn[i]][0], left_turn[i], 5, u16WHITE);
    }
}
/**
 * @description: ���ұ���ת�۵�
 * @param {uint8_t} up_turn
 * @return {*}
 */
void TFTSPI_BinRoad_Rightturn(uint8_t right_turn[]) {
  if (num_right_turn < 10)
    for (uint8_t i = 0; i < num_right_turn; i++) {
      TFTSPI_Draw_Circle(ImageSide[right_turn[i]][1], right_turn[i], 5, u16WHITE);
    }
}
// /**
//  * @description: �ж��Ƿ���ֱ��
//  * @param {int} *
//  * @param {int} coordinatesSize�����
//  * @param {int} *coordinatesColSize
//  * @return {*}
//  */
// bool checkStraightLine(int *coordinates, int coordinatesSize, int *coordinatesColSize) {
//    for(uint8_t i=0;i<60;i++){
//     uint8_t x1 =
//    }

//   for (int i = 2; i < coordinatesSize; i++) {
//     int x1 = coordinates[i - 2][0];
//     int y1 = coordinates[i - 2][1];
//     int x2 = coordinates[i - 1][0];
//     int y2 = coordinates[i - 1][1];
//     int x3 = coordinates[i][0];
//     int y3 = coordinates[i][1];

//     if ((y2 - y1) * (x3 - x2) != (y3 - y2) * (x2 - x1)) {
//       return false;
//     }
//   }
//   return true;
// }

/*!
 * @brief    �ж��ϱ����Ƿ񵥵�
 * @param    X1 :��ʼX��
 * @param    X2 :��ֹX��              X1 < X2
 * @param    imageIn �� ��������
 *
 * @return   0��������or���� 1������������ 2�������ݼ�
 *
 * @note
 *
 * @see
 *
 * @date     2021/11/30 ���ڶ�
 */
uint8_t RoadUpSide_Mono(uint8_t X1, uint8_t X2, uint8_t imageIn[2][LCDW]) {
  uint8_t i = 0, num = 0;

  for (i = X1; i < X2 - 1; i++) {
    if (imageIn[0][i] >= imageIn[0][i + 1])
      num++;
    else
      num = 0;
    if (num >= (X2 - X1) * 4 / 5) return 1;
  }

  for (i = X1; i < X2 - 1; i++) {
    if (imageIn[0][i] <= imageIn[0][i + 1])
      num++;
    else
      num = 0;
    if (num >= (X2 - X1) * 4 / 5) return 2;
  }
  return 0;
}
/**
 * @description: �����ж���������
 * @param {uint8_t} imageOut
 * @return {*}
 */
void StraightLine(uint8_t imageOut[2][LCDW], uint8_t arry) {
  uint16_t temp = 0;
  for (uint8_t i = 47 - arry; i <= 47 + arry; i++) {
    temp += (60 - imageOut[0][i]);
  }
  straightline = temp / ((2 * arry) + 1);
}
/*!
 * @brief    �ж��Ƿ���ֱ��
 *
 * @param    image �� ��ֵͼ����Ϣ
 *
 * @return   0������ֱ���� 1��ֱ��
 *
 * @note     ˼·�����߱��߶�����
 *
 * @see
 *
 * @date     2020/6/23 ���ڶ�
 */
uint8_t RoadIsStraight(uint8_t imageSide[LCDH][2]) {
  uint8_t i = 0;
  uint8_t leftState = 0, rightState = 0;

  /* ������Ƿ񵥵� */
  for (i = ROAD_START_ROW - 1; i > ROAD_END_ROW; i--) {
    if (imageSide[i][0] + 5 < imageSide[i + 1][0]) {
      leftState = 1;
      break;
    }
  }

  /* �ұ����Ƿ񵥵� */
  for (i = ROAD_START_ROW - 1; i > ROAD_END_ROW; i--) {
    if (imageSide[i][1] - 5 > imageSide[i + 1][1]) {
      rightState = 1;
      break;
    }
  }

  if (leftState == 1 && rightState == 1) {
    return 1;
  }

  return 0;
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�TProcess
*  ����˵����T�ֲ��ߴ���
*  ����˵��:
*               imageUp         �����±���
*               imageInput      :��Ե��ֵ��ͼ������
*               imageSide       �����ұ���
*               *flag           :��־λ
*  �������أ���
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע���жϱ�־λ��ֵ�����߸������۵�����ȵ� ���Խ����ŵ������ϣ��۲�ͼ�������ֵ����ʵ�������������
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t TProcess(uint8_t imageInput[LCDH][LCDW], uint8_t imageUp[2][LCDW], uint8_t imageSide[LCDH][2], uint8_t *flag) {
  switch (*flag) {
    case 1:
      Target_Speed = T_speed_Z;
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //��������                                                                       //��T�� �ӻ����ڶ��׶Σ������߽׶Σ�����
      if (T_2) {                                 //�������ͬʱ���������ߵ����ͳ�T PS:t�ֳ�ʶ��ͬ����
        *flag = 2;
        RAllPulse = 0;
      } else if (RAllPulse >= 30000) {
        *flag = 0;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //��������
      }
      break;
    case 2:  //����T��
      Target_Speed = T_speed_C;
      Servo_P = Servo_P_chut;
      Servo_D = Servo_D_chut;
      if (R_T_2) {
        *flag = 0;  // �л�״̬ ��T�����
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);
      }
      break;

    case 3:  //��T��
      Target_Speed = T_speed_Z;
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //��������                                                                       //��T�� �ӻ����ڶ��׶Σ������߽׶Σ�����
      if (T_2) {
        *flag = 4;
        RAllPulse = 0;
      } else if (RAllPulse >= 30000) {
        *flag = 0;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //��������
      }
      break;
    case 4:  //����T��
      Target_Speed = T_speed_C;
      Servo_P = Servo_P_chut;
      Servo_D = Servo_D_chut;
      if (L_T_2) {
        *flag = 0;  //�л�״̬ ��T�����
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);
      }
      break;
  }
  return 0;
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�RoadIsZebra
*  ����˵���������߼�⺯��
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע���жϱ�־λ��ֵ�����߸������۵�����ȵ� ���Խ����ŵ������ϣ��۲�ͼ�������ֵ����ʵ�������������
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t RoadIsZebra(uint8_t imageSide[LCDH][2], uint8_t *flag) {
  uint8_t j, i, num_black_white = 0, count1 = 0, diuL = 0, diuR = 0, budiuL = 0, budiuR = 0;
  //�ÿ�������10�н����жϷ�ֹ������Ϊ�����ܷ�
  if (ImageSide[ROAD_MAIN_ROW - 5 - 5][0] <= 5 && ImageSide[ROAD_MAIN_ROW - 5 - 5][1] <= 47) {  //һ�߶���һ�����м�ͻ��
    if (ImageSide[ROAD_MAIN_ROW - 10 - 5][1] >= 47 && ImageSide[ROAD_MAIN_ROW - 5][1] >= 47 && ImageSide[ROAD_MAIN_ROW][1] >= 47) {
      for (i = ROAD_MAIN_ROW - 10 - 5; i < ROAD_MAIN_ROW - 5; i++) {
        for (j = 20; j < 70; j++) {
          if (Bin_Image[i][j] == 1 && Bin_Image[i][j - 1] == 0) {
            num_black_white++;
            // printf("%d\n", num_black_white);
            if (num_black_white > 55) {
              RAllPulse = 0;  //�������ۼ�ֵ����
              return 1;       //�󳵿�
            }
          }
        }
      }
    }
  }
  if (ImageSide[ROAD_MAIN_ROW - 5 - 5][1] >= 90 && ImageSide[ROAD_MAIN_ROW - 5 - 5][0] >= 47) {  //һ�߶���һ�����м�ͻ��
    if (ImageSide[ROAD_MAIN_ROW - 10 - 5][0] <= 47 && ImageSide[ROAD_MAIN_ROW - 5][0] <= 47 && ImageSide[ROAD_MAIN_ROW][0] <= 47) {
      for (i = ROAD_MAIN_ROW - 10 - 5; i < ROAD_MAIN_ROW - 5; i++) {
        for (j = 20; j < 70; j++) {
          if (Bin_Image[i][j] == 1 && Bin_Image[i][j - 1] == 0) {
            num_black_white++;
            // printf("%d\n", num_black_white);
            if (num_black_white > 55) {
              RAllPulse = 0;  //�������ۼ�ֵ����
              return 2;       //�ҳ���
            }
          }
        }
      }
    }
  }
  // for (i = 40; i > 20; i--) {
  //   for (j = 20; j < 70; j++) {
  //     if (Bin_Image[i][j] == 1 && Bin_Image[i][j - 1] == 0) {
  //       num_black_white++;
  //     }
  //     if (Bin_Image[i][j] == 0 && Bin_Image[i][j - 1] == 1) {
  //       count1++;
  //     }
  //   }
  //   if (ImageSide[i][0] <= 2) {
  //     diuL++;
  //   } else {
  //     budiuL++;
  //   }
  //   if (ImageSide[i][1] >= 90 <= 2) {
  //     diuR++;
  //   } else {
  //     budiuR++;
  //   }
  // }
  // if (num_black_white > 55 && count1 > 55 && diuL > 10 && budiuL > 18) {
  //   RAllPulse = 0;  //�������ۼ�ֵ����
  //   return 1;
  // }
  // if (num_black_white > 55 && count1 > 55 && diuR > 10 && budiuL > 18) {
  //   RAllPulse = 0;  //�������ۼ�ֵ����
  //   return 2;
  // }
  return 0;
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�RoadIsRoundabout
*  ����˵����������⺯��
*  ����˵��:
*               Upimage        �����±���
*               imageInput     :��Ե��ֵ��ͼ������
*               image          �����ұ���
*  �������أ�0
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע���жϱ�־λ��ֵ�����߸������۵�����ȵ� ���Խ����ŵ������ϣ��۲�ͼ�������ֵ����ʵ�������������
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t RoadIsRoundabout(uint8_t Upimage[2][LCDW], uint8_t imageInput[LCDH][LCDW], uint8_t image[LCDH][2]) {
  //�һ����жϣ����룩
  //�ϱ��������ұ��������Сֵ�ϵĲ�ֵ����30 �� �����û�ж��� �� �ұ��߶��� �� ������۵���Ϊ0 �� �ϱ����۵������ڵ���3 �� �ұ����۵�������1 �� �ж�Ϊ�һ���
  if (R_Round_0) {
    RAllPulse = 0;
    g_ucFlagRoundabout = FlagRoundabout_R;
    return 0;
  }
  /* �󻷵��жϣ����룩 */
  if (L_Round_0) {
    RAllPulse = 0;
    g_ucFlagRoundabout = FlagRoundabout_L;
    return 0;
  }
  return 0;
}
/*!
 * @brief    ��ȡ��������
 *
 * @param    imageInput �� ��ֵͼ����Ϣ
 * @param    imageOut   �� ��������
 * @param    status     �� 1���󻷵�(����)  2���һ���(����)
 *
 * @return
 *
 * @note     ˼·������һ�߱����ϸ񵥵�������һ�߱��ߣ���ȡ��һ����
 *
 * @see
 *
 * @date     2020/6/23 ���ڶ�
 */
void RoundaboutGetSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageSide[LCDH][2], uint8_t status) {
  uint8_t i = 0, j = 0;

  switch (status) {
      /* �󻷵� */
    case 1: {
      /* ����ȷ����߽� */
      for (i = ROAD_START_ROW; i > ROAD_END_ROW; i--) {
        for (j = LCDW / 2; j > 0; j--) {
          if (!imageInput[i][j]) {
            imageSide[i][0] = j;
            break;
          }
        }
      }
      break;
    }

    case 2: {
      /* ����ȷ���ұ߽� */
      for (i = ROAD_START_ROW; i > ROAD_END_ROW; i--) {
        for (j = LCDW / 2; j < LCDW; j++) {
          if (!imageInput[i][j]) {
            imageSide[i][1] = j;
            break;
          }
        }
      }
      break;
    }
  }
}

void Roundabout_Get_UpDowmSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageSide[2][LCDW], uint8_t status) {
  uint8_t i = 0, j = 0;

  switch (status) {
    case 1: {
      /* ����ȷ���ϱ߽� */
      for (i = 159; i > 0; i--) {
        for (j = LCDH / 2; j > 0; j--) {
          if (!imageInput[j][i]) {
            imageSide[0][i] = j;
            break;
          }
        }
      }
      break;
    }

    case 2: {
      /* ����ȷ���±߽� */
      for (i = 159; i > 0; i--) {
        for (j = LCDH / 2; j < LCDH; j++) {
          if (!imageInput[j][i]) {
            imageSide[1][i] = j;
            break;
          }
        }
      }
      break;
    }
  }
}
/*!
 * @brief    �жϱ����Ƿ���ڻ���
 *
 * @param    imageInput �� ��ֵͼ����Ϣ
 * @param    imageOut   �� ��������
 * @param    status     �� 1�������  2���ұ���
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2020/6/23 ���ڶ�
 */
uint8_t RoundaboutGetArc(uint8_t imageSide[LCDH][2], uint8_t status, uint8_t num, uint8_t *index) {
  int i = 0;
  uint8_t inc = 0, dec = 0, n = 0;
  switch (status) {
    case 1:
      for (i = ROAD_START_ROW - 1; i > ROAD_END_ROW; i--) {
        if (imageSide[i][0] != 0 && imageSide[i + 1][0] != 0) {
          if (imageSide[i][0] == imageSide[i + 1][0]) {
            n++;
            continue;
          }
          if (imageSide[i][0] > imageSide[i + 1][0]) {
            inc++;
            inc += n;
            n = 0;
          } else {
            dec++;
            dec += n;
            n = 0;
          }

          /* �л��� */
          if (inc > num && dec > num) {
            *index = i + num;
            return 1;
          }
        } else {
          inc = 0;
          dec = 0;
          n = 0;
        }
      }

      break;

    case 2:
      for (i = ROAD_START_ROW - 1; i > ROAD_END_ROW; i--) {
        if (imageSide[i][1] != 159 && imageSide[i + 1][1] != 159) {
          if (imageSide[i][1] == imageSide[i + 1][1]) {
            n++;
            continue;
          }
          if (imageSide[i][1] > imageSide[i + 1][1]) {
            inc++;
            inc += n;
            n = 0;
          } else {
            dec++;
            dec += n;
            n = 0;
          }

          /* �л��� */
          if (inc > num && dec > num) {
            *index = i + num;
            return 1;
          }
        } else {
          inc = 0;
          dec = 0;
          n = 0;
        }
      }

      break;
  }

  return 0;
}

/*!
 * @brief    �жϱ����Ƿ���ڻ���
 *
 * @param    SideInput �� �ϱ�������
 * @param    num       �� ������
 * @param    index     �� ��͵�
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2021/12/01 ������
 */
uint8_t UpSideErr(uint8_t SideInput[2][LCDW], uint8_t status, uint8_t num, uint8_t *index) {
  uint8_t dec = 0, inc = 0, i;
  //�����Ƿ���ͻ��
  switch (status) {
    case 1:
      for (i = 159 - 1; i > 0; i--) {
        if (UpdowmSide[0][i] > 1 && UpdowmSide[0][i + 1] > 1) {
          if (UpdowmSide[0][i] >= UpdowmSide[0][i + 1])
            inc++;
          else
            dec++;
          /* �л��� */
          if (inc > num && dec > num) {
            *index = i + num;
            return 1;
          }
        } else {
          inc = 0;
          dec = 0;
        }
      }
      break;
    //�±���
    case 2:
      for (i = 159 - 1; i > 0; i--) {
        if (UpdowmSide[1][i] != 1 && UpdowmSide[1][i + 1] != 1) {
          if (UpdowmSide[1][i] >= UpdowmSide[1][i + 1])
            inc++;
          else
            dec++;
          /* �л��� */
          if (inc > num && dec > num) {
            *index = i + num;
            return 1;
          }
        } else {
          inc = 0;
          dec = 0;
        }
      }
      break;
  }

  return 0;
}

/*!
 * @brief    ���ߴ���
 *
 * @param    imageSide  : ����
 * @param    status     : 1������߲���   2���ұ��߲���
 * @param    startX     : ��ʼ�� ����
 * @param    startY     : ��ʼ�� ����
 * @param    endX       : ������ ����
 * @param    endY       : ������ ����
 *
 * @return
 *
 * @note     endY һ��Ҫ���� startY
 *
 * @see
 *
 * @date     2020/6/24 ������
 */
void ImageAddingLine(uint8_t imageSide[LCDH][2], uint8_t status, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY) {
  int i = 0;

  /* ֱ�� x = ky + b*/
  float k = 0.0f, b = 0.0f;
  switch (status) {
    case 1:  //����
    {
      k = (float)((float)endX - (float)startX) / (float)((float)endY - (float)startY);
      b = (float)startX - (float)startY * k;

      for (i = startY; i < endY; i++) {
        imageSide[i][0] = (uint8_t)(k * i + b);
      }
      break;
    }

    case 2:  //�Ҳ���
    {
      k = (float)((float)endX - (float)startX) / (float)((float)endY - (float)startY);
      b = (float)startX - (float)startY * k;

      for (i = startY; i < endY; i++) {
        imageSide[i][1] = (uint8_t)(k * i + b);
      }
      break;
    }
  }
}

/*!
 * @brief    Ѱ�������
 *
 * @param    imageSide   �� ��������
 * @param    status      ��1����߽�   2���ұ߽�
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2020/6/24 ������
 */
uint8_t ImageGetHop(uint8_t imageSide[LCDH][2], uint8_t state, uint8_t *x, uint8_t *y) {
  int i = 0;
  uint8_t px = 0, py = 0;
  uint8_t count = 0;
  switch (state) {
    case 1:
      /* Ѱ������� */
      for (i = ROAD_START_ROW - 1; i > ROAD_END_ROW; i--) {
        if (imageSide[i][0] == 0 && i > (ROAD_END_ROW + 5)) {
          count++;

          if (count > 5) {
            if (imageSide[i - 1][0] > (imageSide[i][0] + 20)) {
              py = i - 1;
              px = imageSide[py][0];
              break;
            }
            if (imageSide[i - 2][0] > (imageSide[i - 1][0] + 20)) {
              py = i - 2;
              px = imageSide[py][0];
              break;
            }
            if (imageSide[i - 3][0] > (imageSide[i - 2][0] + 20)) {
              py = i - 3;
              px = imageSide[py][0];
              break;
            }
            if (imageSide[i - 4][0] > (imageSide[i - 3][0] + 20)) {
              py = i - 4;
              px = imageSide[py][0];
              break;
            }
          }

        } else {
          count = 0;
        }
      }

      if (py != 0) {
        *x = px;
        *y = py;
        return 1;
      }

      break;

    case 2:
      /* Ѱ������� */
      for (i = ROAD_START_ROW - 1; i > ROAD_END_ROW; i--) {
        if (imageSide[i][1] == 159 && i > (ROAD_END_ROW + 5)) {
          count++;

          if (count > 5) {
            if (imageSide[i - 1][1] < (imageSide[i][1] - 20)) {
              py = i - 1;
              px = imageSide[py][1];
              break;
            }
            if (imageSide[i - 2][1] < (imageSide[i - 1][1] - 20)) {
              py = i - 2;
              px = imageSide[py][1];
              break;
            }
            if (imageSide[i - 3][1] < (imageSide[i - 2][1] - 20)) {
              py = i - 3;
              px = imageSide[py][1];
              break;
            }
            if (imageSide[i - 4][1] < (imageSide[i - 3][1] - 20)) {
              py = i - 4;
              px = imageSide[py][1];
              break;
            }
          }

        } else {
          count = 0;
        }
      }

      if (py != 0) {
        *x = px;
        *y = py;
        return 1;
      }

      break;
  }
  return 0;
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�RoundaboutProcess
*  ����˵�����������ߴ���
*  ����˵��:
*               UpdowmSide          �����±���
*               imageInput          :��Ե��ֵ��ͼ������
*               imageSide           �����ұ���
*  �������أ���
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע���жϱ�־λ��ֵ�����߸������۵�����ȵ� ���Խ����ŵ������ϣ��۲�ͼ�������ֵ����ʵ�������������
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void RoundaboutProcess(uint8_t imageInput[LCDH][LCDW], uint8_t imageSide[LCDH][2], uint8_t UpdowmSide[2][LCDW]) {
  switch (g_ucFlagRoundabout) {
      /* �����һ��� ��һ�ζ��� �ڲ����߻�������ʱ�л�����һ��״̬ */
    // 0
    case NO:
      break;
    // 1���� �һ��� �� T�Ϳ� �� ����
    case FlagRoundabout_R:                       //ʶ���Ҳ⻷�� ��ʱ��δ�����ײ� ��Ϊ�ұ߶��ߣ�������Ҫ���������T��ʱ��������ͬ
      Target_Speed = Round_speed_S;              //�����ٶ�
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //��������                                                                       //��T�� �ӻ����ڶ��׶Σ������߽׶Σ�����
      if ((R_Round_1))                           //��������״̬2
      {
        g_ucFlagRoundabout = FlagRoundabout_R_1;  //�л�״̬
        RAllPulse = 0;                            //�������ۼ�ֵ����
      } else if (num_left_lost > 5 && RAllPulse > 3000) {
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;                   //�������
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
        return;
      } else if (R_T_1) {  //�г̴���5000 �� �ϱ����ǵ����� ���ж�Ϊ��T��
        g_ucFlagT = 1;
        RAllPulse = 0;            // T�οڱ�־λ ��Ϊ 1
        g_ucFlagRoundabout = NO;  //�������
        return;
      }

      if (RoadIsZebra(ImageSide, &g_ucFlagZebra)) {
        g_ucFlagZebra = 3;
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
      }
      break;
    /* 2�������ڵڶ��ζ���ʱ�л���һ״̬ */
    case FlagRoundabout_R_1:
      Target_Speed = Round_speed_S;
      if ((R_Round_2))                            //���߲��ǵ���������T�Ϳڡ���Ѱ���뻷��
      {                                           //��಻���� �� �Ҳඪ�߳���10�����ص� �� ���û���۵� �� �Ϸ�����1���۵� �� ��ʱ��ʼ�뻷
        RAllPulse = 0;                            //�������ۼ�ֵ����
        g_ucFlagRoundabout = FlagRoundabout_R_2;  //״̬�л�����һ�׶�
      } else if (num_left_lost > 5) {
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;                   //�������
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
        return;
      } else if (R_T_1) {  //�г̴���5000 �� �ϱ����ǵ����� ���ж�Ϊ��T��
        g_ucFlagT = 1;
        RAllPulse = 0;            // T�οڱ�־λ ��Ϊ 1
        g_ucFlagRoundabout = NO;  //�������
        return;
      } else if (RAllPulse > 10000) {              //�г̴���5000�˳�
        RAllPulse = 0;                             // T�οڱ�־λ ��Ϊ 1
        g_ucFlagRoundabout = NO;                   // t���˳�
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
        return;                                    //�������
      }
      if (RoadIsZebra(ImageSide, &g_ucFlagZebra)) {
        g_ucFlagZebra = 3;
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
      }
      break;
    // 3 �뵺�ж��Ƿ����ɹ� ����ɹ����л�����һ״̬���뵺ʱ��
    case FlagRoundabout_R_2:
      Servo_P = Servo_P_jinhuan_R;
      Servo_D = Servo_D_jinhuan_R;
      Target_Speed = Round_speed_D;               //����
      if (R_Round_3) {                            //���ߵ��������뻷�ɹ���ʼ����ѭ��
        g_ucFlagRoundabout = FlagRoundabout_R_3;  //�л�״̬
        RAllPulse = 0;                            //�������ۼ�ֵ����                                                                                    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);                                   //��������
      }
      break;
    /* 4�뻷������ѭ�� */
    case FlagRoundabout_R_3:  //����ѭ������������뵺��
      Target_Speed = Round_speed_W;
      if (R_Round_4) {  //�������ͬʱ���������ߵ����ͳ���
        g_ucFlagRoundabout = FlagRoundabout_R_4;
        RAllPulse = 0;
      }
      break;
    // 5 ִ�г�����������ת��
    case FlagRoundabout_R_4:
      Servo_P = Servo_P_chuhuan_R;
      Servo_D = Servo_D_chuhuan_R;
      Target_Speed = Round_speed_D;
      if (R_Round_5) {
        g_ucFlagRoundabout = FlagRoundabout_R_5;  //�л�״̬
        RAllPulse = 0;                            //��������
      }
      break;
    // 6������ɣ��Ҳಹ����ȷ����ֱ��
    case FlagRoundabout_R_5:
      Servo_P = Servo_P_chuhuanhou_R;
      Servo_D = Servo_D_chuhuanhou_R;
      Target_Speed = Round_speed_S;
      if (R_Round_6)  //���������
      {
        g_ucFlagRoundabout = NO;                   //�л�״̬
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
        RAllPulse = 0;
      }
      break;
      /* ***********************************************�󻷵�************************************************* */
    case FlagRoundabout_L:  //ʶ���Ҳ⻷�� ��ʱ��δ�����ײ� ��Ϊ�ұ߶��ߣ�������Ҫ���������T��ʱ��������ͬ
      Target_Speed = Round_speed_S;
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //��������
      if (L_Round_1)                             //�н�·�̴���1500֮��ʼ�ж���û���ٴγ��ֶ���
      {
        g_ucFlagRoundabout = FlagRoundabout_L_1;  //�л�״̬
        RAllPulse = 0;                            //�������ۼ�ֵ����
      } else if (num_right_lost > 5 && RAllPulse > 3000) {
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;                   //�������
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
        return;
      } else if (L_T_1) {
        RAllPulse = 0;            //�г̴���5000 �� �ϱ����ǵ����� ���ж�Ϊ��T��
        g_ucFlagT = 3;            // T�οڱ�־λ ��Ϊ 1
        g_ucFlagRoundabout = NO;  // t���˳�
        return;                   //�������
      }
      if (RoadIsZebra(ImageSide, &g_ucFlagZebra)) {
        g_ucFlagZebra = 1;
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
      }
      break;
    /* 2�������ڵڶ��ζ���ʱ�л���һ״̬ */
    case FlagRoundabout_L_1:
      Target_Speed = Round_speed_S;
      if (L_Round_2)                              //���߲��ǵ���������T�Ϳڡ���Ѱ���뻷��
      {                                           //��಻���� �� �Ҳඪ�߳���10�����ص� �� ���û���۵� �� �Ϸ�����1���۵� �� ��ʱ��ʼ�뻷
        RAllPulse = 0;                            //�������ۼ�ֵ����
        g_ucFlagRoundabout = FlagRoundabout_L_2;  //״̬�л�����һ�׶�
      } else if (num_right_lost > 5) {
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;                   //�������
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
        return;
      } else if (L_T_1) {
        RAllPulse = 0;                             //�г̴���5000 �� �ϱ����ǵ����� ���ж�Ϊ��T��
        g_ucFlagT = 3;                             // T�οڱ�־λ ��Ϊ 1
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
        g_ucFlagRoundabout = NO;                   // t���˳�
        return;                                    //�������
      } else if (RAllPulse > 10000) {              //�г̴���5000�˳�
        RAllPulse = 0;                             // T�οڱ�־λ ��Ϊ 1
        g_ucFlagRoundabout = NO;                   // t���˳�
        return;                                    //�������
      }
      if (RoadIsZebra(ImageSide, &g_ucFlagZebra)) {
        g_ucFlagZebra = 1;
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
      }
      break;
    // 3 �뵺�ж��Ƿ����ɹ� ����ɹ����л�����һ״̬���뵺ʱ��
    case FlagRoundabout_L_2:  //����
      Servo_P = Servo_P_jinhuan_L;
      Servo_D = Servo_D_jinhuan_L;
      Target_Speed = Round_speed_D;
      if (L_Round_3) {                            //���ߵ��������뻷�ɹ���ʼ����ѭ��
        g_ucFlagRoundabout = FlagRoundabout_L_3;  //�л�״̬
        RAllPulse = 0;                            //�������ۼ�ֵ����
      }
      break;
    /* 4�뻷������ѭ�� */
    case FlagRoundabout_L_3:  //����ѭ������������뵺��                             // Target_Speed = 3000;                                        //�г̳���3500���л�״̬����������ѭ��
      Target_Speed = Round_speed_W;
      if (L_Round_4) {  //�������ͬʱ���������ߵ����ͳ���
        g_ucFlagRoundabout = FlagRoundabout_L_4;
        RAllPulse = 0;
      }
      break;
    // 5 ִ�г�����������ת��
    case FlagRoundabout_L_4:
      Servo_P = Servo_P_chuhuan_L;
      Servo_D = Servo_D_chuhuan_L;
      Target_Speed = Round_speed_D;
      if (L_Round_5) {
        g_ucFlagRoundabout = FlagRoundabout_L_5;  //�л�״̬
        RAllPulse = 0;                            //��������
      }

      break;
    // 6������ɣ��Ҳಹ����ȷ����ֱ��
    case FlagRoundabout_L_5:
      Target_Speed = Round_speed_S;
      Servo_P = Servo_P_chuhuanhou_L;
      Servo_D = Servo_D_chuhuanhou_L;
      if (L_Round_6)  //���������
      {
        g_ucFlagRoundabout = NO;                   //�л�״̬
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
        RAllPulse = 0;
      }
      break;
      // 5�󻷵����һ���һ��ֻ�Ƿ������ҶԵ�
      // case FlagRoundabout_L:
      //   Target_Speed = 2000;    //����
      //   if (RAllPulse <= 3500)  //�н�·��С��3500
      //   {
      //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //��������

      //     ImageAddingLine(imageSide, 1, 47, 0, 47 - 47 / 2, 58);  //�������޸� ���ߣ�����ת
      //   } else {
      //     errU1 = RoadUpSide_Mono(24, 70, UpdowmSide);  //�ϵ��������߼�
      //     if (errU1 == 0)                               //���߲��ǵ����� ���жϻ���
      //     {
      //       if (num_left_lost > 10 && num_right_lost == 0 && num_up_turn > 1 && num_right_turn == 0) {
      //         RAllPulse = 0;
      //         g_ucFlagRoundabout = FlagRoundabout_L_1;
      //       }
      //     } else {  //��̴���5000 ������ǵ����� ���ж�Ϊ��T��
      //       if (RAllPulse > 5000) {
      //         g_ucFlagT = 3;            // T�οڱ�־λ��Ϊ
      //         g_ucFlagRoundabout = NO;  //�������
      //       }
      //     }
      //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //��������
      //   }

      /****************�������ж�***************************************************/
      //          num_black_white = 0;
      //          //ɨ���ֵ�����ݵ�30�� ���Ҷ��ٸ��ڰ׽���ı߽�
      //          for(i = LCDW-1; i > 20; i--)
      //          {
      //              if(Bin_Image[30][i] +  Bin_Image[30][i-1] == 1)
      //                  num_black_white++;
      //          }
      //          if(num_black_white >= 14)   //�ڰ׽������14���ж�Ϊ������
      //          {
      //              g_ucFlagRoundabout = NO;  //�������
      //              g_ucFlagZebra = 1;          //�����߱�־λ��1
      //              RAllPulse = 0;      //�������ۼ�ֵ����
      //            }
      //          break;
      // 6
    // case FlagRoundabout_L_1:
    //   if (RAllPulse <= 3500)  //�н�·��С��3500
    //   {
    //     Target_Speed = 2000;                                    //����
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);               //��������
    //     ImageAddingLine(imageSide, 2, 47, 0, 47 + 47 / 2, 58);  //�������޸� ��ת�뵺
    //   } else {
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);           //��������
    //     flag_up_line = RoadUpSide_Mono(5, 90, UpdowmSide);  //���� ���� ���� ����1
    //     if (flag_up_line == 1) {
    //       g_ucFlagRoundabout = FlagRoundabout_L_2;
    //       RAllPulse = 0;
    //     }
    //   }
    //   break;
    // // 7
    // case FlagRoundabout_L_2:
    //   if (RAllPulse <= 3000)  //�н�·��С��3000
    //   {
    //     Target_Speed = 2000;                                   //����
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);              //��������
    //     ImageAddingLine(imageSide, 2, 47 / 3, 0, 47 / 2, 58);  //�������޸� ��ת����
    //   } else {
    //     Target_Speed = 2000;  //����
    //     if (num_left_lost > 15 && num_right_lost == 0 && num_right_turn == 0) {
    //       g_ucFlagRoundabout = FlagRoundabout_L_3;
    //       RAllPulse = 0;
    //     }
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //��������
    //   }
    //   break;
    // // 8
    // case FlagRoundabout_L_3:
    //   if (RAllPulse <= 3500)  //�н�·��С��3500
    //   {
    //     Target_Speed = 2000;                                    //����
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);               //��������
    //     ImageAddingLine(imageSide, 1, 47, 0, 47 - 47 / 2, 58);  //�������޸� ��߲��� �˳�����
    //   } else {
    //     g_ucFlagRoundabout = NO;
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //��������
    //   }
    //   break;
    default:
      break;
  }
}

/*!
 * @brief    ��ȡʮ�ֱ���
 *
 * @param    imageInput �� ��ֵͼ����Ϣ
 * @param    imageOut   �� ��������
 *
 * @return
 *
 * @note     ˼·�����м�����������
 *
 * @see
 *
 * @date     2020/6/23 ���ڶ�
 */
void CrossGetSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageSide[LCDH][2]) {
  uint8_t i = 0, j = 0;

  for (i = ROAD_START_ROW - 1; i > ROAD_END_ROW; i--) {
    for (j = 78; j > 1; j--) {
      if (imageInput[i][j]) {
        imageSide[i][0] = j;
        break;
      }
    }

    for (j = 78; j < 159; j++) {
      if (imageInput[i][j]) {
        imageSide[i][1] = j;
        break;
      }
    }
  }
}

/*!
 * @brief    ʮ�ֲ��ߴ���
 *
 * @param    imageInput �� ��ֵͼ����Ϣ
 * @param    imageSide  �� ��������
 * @param    status     ��ʮ�ֱ�־λ   1������ʮ��    2������ʮ��   3����ʮ��
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2020/6/24 ������
 */
void CrossProcess(uint8_t imageInput[LCDH][LCDW], uint8_t imageSide[LCDH][2], uint8_t *state) {
  uint8_t pointX = 0, pointY = 0;
  uint8_t leftIndex = 0;
  static uint8_t count = 0;
  switch (*state) {
    case 1: {
      /* ���»�ȡ���� */
      CrossGetSide(imageInput, imageSide);
      /* Ѱ������� */
      if (ImageGetHop(imageSide, 1, &pointX, &pointY)) {
        /* ���� */
        ImageAddingLine(imageSide, 1, pointX, pointY, 0, ROAD_START_ROW);
      }
      leftIndex = pointY;
      pointX = 0;
      pointY = 0;
      /* Ѱ������� */
      if (ImageGetHop(imageSide, 2, &pointX, &pointY)) {
        /* ���� */
        ImageAddingLine(imageSide, 2, pointX, pointY, (LCDW - 1), ROAD_START_ROW);
      }
      if (leftIndex != 0 && pointY != 0 && leftIndex >= ROAD_MAIN_ROW && pointY >= ROAD_MAIN_ROW) {
        *state = 2;
        count = 0;
      }
      if (count++ > 20) {
        *state = 2;
        count = 0;
      }
      break;
    }
    case 2: {
      /* ��黡�� */
      if (RoundaboutGetArc(imageSide, 1, 5, &leftIndex)) {
        /* ����ȷ����߽� */
        RoundaboutGetSide(imageInput, imageSide, 1);
        if (ImageGetHop(imageSide, 1, &pointX, &pointY)) {
          /* ���� */
          ImageAddingLine(imageSide, 1, pointX, pointY, imageSide[leftIndex][0], leftIndex);
          *state = 3;
          count = 0;
        } else {
          imageSide[ROAD_MAIN_ROW][0] = LCDW / 2;
          imageSide[ROAD_MAIN_ROW][1] = LCDW - 1;
        }
      }
      break;
    }

    case 3: {
      /* ����ȷ����߽� */
      RoundaboutGetSide(imageInput, imageSide, 1);
      if (ImageGetHop(imageSide, 1, &pointX, &pointY)) {
        /* ��黡�� */
        if (RoundaboutGetArc(imageSide, 1, 5, &leftIndex)) {
          /* ���� */
          ImageAddingLine(imageSide, 1, pointX, pointY, imageSide[leftIndex][0], leftIndex);
        } else {
          /* ���� */
          ImageAddingLine(imageSide, 1, pointX, pointY, 0, ROAD_START_ROW);
        }

        if (pointY >= ROAD_MAIN_ROW) {
          *state = 0;
          count = 0;
        }
      } else {
        imageSide[ROAD_MAIN_ROW][0] = 120;
        imageSide[ROAD_MAIN_ROW][1] = LCDW - 1;
      }
      if (count++ > 10) {
        *state = 0;
        count = 0;
      }
      break;
    }
  }
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�RoadIsFork
*  ����˵����Y�ּ��
*  ����˵��:
*               imageInput      �����±���
*               imageSide       �����ұ���
*  �������أ�0
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע���жϱ�־λ��ֵ�����߸������۵�����ȵ� ���Խ����ŵ������ϣ��۲�ͼ�������ֵ����ʵ�������������
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void findthreerightdown() {
  threerightguaidian_x = 93;
  threerightguaidian_y = 0;
  for (uint8_t i = 40; i > 20; i--) {
    if (ImageSide[i][1] < threerightguaidian_x) {
      threerightguaidian_x = ImageSide[i][1];
      threerightguaidian_y = i;
    }
  }
}
void findthreeleftdown() {
  threeleftguaidian_x = 0;
  threeleftguaidian_y = 0;
  for (uint8_t i = 40; i > 20; i--) {
    if (ImageSide[i][0] > threeleftguaidian_x) {
      threeleftguaidian_x = ImageSide[i][0];
      threeleftguaidian_y = i;
    }
  }
}
#if Y_YIJING
/* ���׽�y */
uint8_t RoadIsFork(uint8_t imageInput[2][LCDW], uint8_t imageSide[LCDH][2]) {
  if (Y_0) {
    uint8_t i = 0;
    findthreerightdown();
    findthreeleftdown();
    // printf("L%d,R%d\n", threeleftguaidian_x, threerightguaidian_x);
    if (abs(up_turn[0] - (threerightguaidian_x + threeleftguaidian_x) / 2) < 20)
      if (abs(threerightguaidian_x - ImageSide[45][1]) < (45 - threerightguaidian_y) * 2 && abs(threeleftguaidian_x - ImageSide[45][0]) < (45 - threeleftguaidian_y) * 2 && ImageSide[10][1] - threerightguaidian_x > 5 && threeleftguaidian_x - ImageSide[10][0] > 5) {
        for (i = 10; i < 15; i++) {
          if (ImageSide[i][1] - ImageSide[i][0] < 70) {
            break;
          }
          if (ImageSide[55 - i][1] - ImageSide[55 - i][0] > 90) {
            break;
          }
        }
        if (i >= 13) {
          g_ucFlagFork = FlagFork_0;
          RAllPulse = 0;
        }
      }
    return 0;
  }
}
/* ���׽�y */
#else
uint8_t RoadIsFork(uint8_t imageInput[2][LCDW], uint8_t imageSide[LCDH][2]) {
  if (Y_0) {
    uint8_t i = 0;
    findthreerightdown();
    findthreeleftdown();
    // printf("L%d,R%d\n", threeleftguaidian_x, threerightguaidian_x);
    if (abs(up_turn[0] - (threerightguaidian_x + threeleftguaidian_x) / 2) < 5)
      if (abs(threerightguaidian_x - ImageSide[45][1]) < (45 - threerightguaidian_y) && abs(threeleftguaidian_x - ImageSide[45][0]) < (45 - threeleftguaidian_y) && ImageSide[10][1] - threerightguaidian_x > 10 && threeleftguaidian_x - ImageSide[10][0] > 10) {
        for (i = 10; i < 15; i++) {
          if (ImageSide[i][1] - ImageSide[i][0] < 70) {
            break;
          }
          if (ImageSide[55 - i][1] - ImageSide[55 - i][0] > 90) {
            break;
          }
        }
        if (i >= 14) {
          g_ucFlagFork = FlagFork_0;
          RAllPulse = 0;
        }
      }
    return 0;
  }
}
#endif
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�ForkProcess
*  ����˵����Y�ֲ��ߴ���
*  ����˵��:
*               imageSide      �����ұ���
*  �������أ�0
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע���жϱ�־λ��ֵ�����߸������۵�����ȵ� ���Խ����ŵ������ϣ��۲�ͼ�������ֵ����ʵ�������������
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
sint32 RAllFork = 0;
void ForkProcess(uint8_t imageSide[LCDH][2]) {
  switch (g_ucFlagFork) {
    case FlagFork_0:  // y�ͽ���
      Target_Speed = Y_speed;
      Servo_P = Servo_P_Y;
      Servo_D = Servo_D_Y;
      g_ucFlagRoundabout = NO;
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);    //��������
      if (Y_1) {                                   //���뵽y��������ѭ��
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //��������
        g_ucFlagFork = FlagFork_1;
        RAllPulse = 0;
      } else if (RAllPulse > 6000) {
        g_ucFlagFork = NO;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //��������
      }
      break;
    case FlagFork_1:  // y��������ѭ��
      if (Y_2) {
        g_ucFlagFork = FlagFork_2;  //�л�����
        RAllPulse = 0;              //������
      }
      break;
    case FlagFork_2:  //�� ����
      Target_Speed = Y_speed;
      Servo_P = Servo_P_Y;
      Servo_D = Servo_D_Y;
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);    //��������
      if (Y_3) {                                   //��y�ɹ�
        g_ucFlagFork = Y_NO;                       //��־λ�ָ�
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //��������
      } else if (RAllPulse > 6000) {
        g_ucFlagFork = NO;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //��������
      }
      break;
    default:
      break;
  }
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�ZebraProcess
*  ����˵���������߳��⴦��
*  ����˵��:
*               imageSide      �����ұ���
*               *state         :��־λ
*  �������أ���
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע���жϱ�־λ��ֵ�����߸������۵�����ȵ� ���Խ����ŵ������ϣ��۲�ͼ�������ֵ����ʵ�������������
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void ZebraProcess(uint8_t imageSide[LCDH][2], uint8_t *state) {
  // 1�� 3��

  switch (count) {
    case 1:                                                             //��һ��ʶ�𵽰�����
      if (KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 1 && *state == 1) {  //����
        *state = 0;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
      }
      if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 0 && *state == 3) {  //����
        *state = 0;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
      }
      if (RAllPulse < 1000)  //���߷�ֹ��ת����ת ���߷�������ⷽ�����
      {
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);                       //��������
      } else if (*state == 1 && num_left_lost < 5 || RAllPulse > 2000)  //�󳵿�
      {
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);                        //�رշ�����
        *state = 0;                                                      //��հ����߱�־λ �Ա��´δ���
        count++;                                                         //����������1
      } else if (*state == 3 && num_right_lost < 5 || RAllPulse > 2000)  //�ҳ���
      {
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //�رշ�����
        *state = 0;                                //��հ����߱�־λ �Ա��´δ���
        count++;                                   //����������1
      }
      break;

    case 2:                                                                    //ʶ�𵽵ڶ��� ������������
      if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 1) {                        //���µ���ģʽ
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);                              //��������
                                                                               // *state = 0;
        game = over;                                                           //�л�����״̬ �������ģʽ
        RAllPulse = 0;                                                         //�������
        count++;                                                               //������1�����п���
      } else if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 0 && *state == 3) {  //�Ҳ�
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);                              //��������
                                                                               // *state = 0;
        game = over;                                                           //�л�����״̬ �������ģʽ
        RAllPulse = 0;                                                         //�������
        count++;                                                               //������1�����п���
      } else if (KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 1 && *state == 1) {
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //��������
        game = over;                               //�л�����״̬ �������ģʽ
        RAllPulse = 0;                             //�������
        count++;                                   //������1�����п���
      } else {                                     //����
        *state = 0;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //������mie
      }
      break;
  }
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�ZebraProcess
*  ����˵���������߳��⴦��
*  ����˵��:
*               imageSide      �����ұ���
*               lineIndex      ��������
*  �������أ����ֵ
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע��
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
int16_t RoadGetSteeringError(uint8_t imageSide[LCDH][2], uint8_t lineIndex) { return imageSide[lineIndex][0] + imageSide[lineIndex][1] - LCDW; }

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�RoadIsNoSide
*  ����˵�����ж������ҷ���
*  ����˵����    imageInput  ��������ͼ������
*             imageOut    ����������[0]:��  [1]����
*             lineIndex   ��ָ����
*  �������أ�0��û�ж���   1:��߶���  2���ұ߶���  3�� ���Ҷ�����   4������
*  �޸�ʱ�䣺2022��6��24��
*  ��   ע��guo
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t RoadIsNoSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[LCDH][2], uint8_t lineIndex) {
  uint8_t state = 0;
  uint8_t i = 0;
  static uint8_t last = 59;

  imageOut[lineIndex][0] = 0;
  imageOut[lineIndex][1] = 93;
  /* �þ���С���ȽϽ����� �ж��Ƿ��� */
  for (i = last; i > 1; i--) {
    if (!imageInput[lineIndex][i]) {
      imageOut[lineIndex][0] = i;
      break;
    }
  }
  if (i == 1) state = 1;  //��߽綪��

  for (i = last; i < 93; i++) {
    if (!imageInput[lineIndex][i]) {
      imageOut[lineIndex][1] = i;
      break;
    }
  }
  if (i == 93) {
    /* ���ұ߽綪�� */
    if (state == 1) state = 3;
    /* �ұ߽綪�� */
    else
      state = 2;
  }
  if (imageOut[lineIndex][1] <= imageOut[lineIndex][0]) state = 4;
  return state;
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�UD_Road_Is_No_Side
*  ����˵�����ж������Ƿ���
*  ����˵����    imageInput  ��������ͼ������
*             imageOut    ����������[0]:��  [1]����
*             lineIndex   ��ָ����
*  �������أ�0��û�ж���   1:�ϱ߶���  2���±߶���  3�� ���¶�����   4������
*  �޸�ʱ�䣺2022��6��24��
*  ��    ע��guo
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t UD_Road_Is_No_Side(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[2][LCDW], uint8_t lineIndex) {
  uint8_t i;
  uint8_t state = 0;          //״̬
  static uint8_t last1 = 30;  //��ʼ��

  //�м��߸�ֵ���¶���
  imageOut[0][lineIndex] = 0;
  imageOut[1][lineIndex] = LCDH - 1;  // 59
  /* 30-2 */
  for (i = last1; i > 1; i--) {
    if (!imageInput[i][lineIndex]) {
      imageOut[0][lineIndex] = i;
      break;
    }
  }
  if (i == 1) /* �ϱ߽綪�� */
    state = 1;

  /* 30-59 */
  for (i = last1; i < LCDH; i++) {
    if (!imageInput[i][lineIndex])  //����Ϊ��ɫ ��ɫΪ0
    {
      imageOut[1][lineIndex] = i;
      break;
    }
  }

  if (i == LCDH) {
    /* ���±߽綼���� */
    if (state == 1) state = 3;
    /* �±߽綪�� */
    else
      state = 2;
  }
  if (imageOut[1][lineIndex] <= imageOut[0][lineIndex]) {
    state = 4;
  }
  return state;
}

/*!
 * @brief    ���ߴ���
 *
 * @param    imageInput �� ��ֵͼ����Ϣ
 * @param    imageOut   �� ��������
 * @param    mode       �� �Ǳ߶��ߣ�   1����߶���  2���ұ߶���
 * @param    lineIndex  �� ��������
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2020/6/24 ������
 */
void RoadNoSideProcess(uint8_t imageInput[2][LCDW], uint8_t imageOut[LCDH][2], uint8_t mode, uint8_t lineIndex) {
  uint8_t i = 0;  //�в���
  switch (mode) {
    case 1:  //����
      for (i = lineIndex; i > 0; i--) {
        imageOut[i][0] = imageInput[0][47 + i];
      }
      break;
    case 2:  //����
      for (i = lineIndex; i > 0; i--) {
        imageOut[i][1] = imageInput[0][47 - i];  //����Ϊ�ϱ������ϱ��߸����ұ��߲���
      }
      break;
  }
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�ImageGetSide
*  ����˵������ȡ���ұ���
*  ����˵��:            imageInput  ����Եͼ������
*                     imageOut    �����ұ�������
*  �������أ�0
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע����Ҫ��
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t ImageGetSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[LCDH][2]) {
  uint8_t i = 0, j = 0, L_arry = 10, R_arry = 10;
  // right_turn_flag = 0, left_turn_flag = 0
  //  right_turn_flag = 0;
  //  left_turn_flag = 0;
  num_left_lost = 0;   //��ඪ�߸�������
  num_right_lost = 0;  //�Ҳඪ�߸�������
  num_all_lost = 0;    //���Ҷ���������
  Aver_right_lost = 0;
  Aver_left_lost = 0;
  RoadIsNoSide(imageInput, imageOut, ROAD_START_ROW);
  g_ucFlagW = 0;
  /* �복ͷ������ Ѱ�ұ��� 59-5 */
  for (i = ROAD_START_ROW - 1; i > ROAD_END_ROW; i--) {
    imageOut[i][0] = 0;
    imageOut[i][1] = 93;

    /* ���ݱ߽��������� Ѱ�ұ߽� */
    for (j = imageOut[i + 1][0] + L_arry; j > 0; j--) {  //�����
      if (j >= 93) j = 93;
      if (j < 0) j = 0;
      if (!imageInput[i][j])  //��ɫ��0 Ϊ����
      {
        imageOut[i][0] = j;
        break;
      } else
        imageOut[i][0] = 0;
    }
    /* �ұ��� */
    for (j = imageOut[i + 1][1] - R_arry; j < 93; j++)  //�Ա��ߵ�5������Ϊ��Χ���ұ���
    {
      if (j >= 93) j = 93;
      if (j < 0) j = 0;
      if (!imageInput[i][j]) {
        imageOut[i][1] = j;
        break;
      } else
        imageOut[i][1] = 93;
    }

    if (imageOut[i][1] < imageOut[i][0]) {  //�߽������ֹ���ұ߽�Ի�
      uint8_t temp = imageOut[i][1];
      imageOut[i][1] = imageOut[i][0];
      imageOut[i][0] = temp;
    }
    // if (abs(imageOut[i][0] - imageOut[i][1]) < 5) {  //��ֹ�߽��غ�
    //   if (imageOut[i][0] > 47) {
    //     imageOut[i][0] = 0;
    //   }
    //   if (imageOut[i][1] < 47) {
    //     imageOut[i][1] = 93;
    //   }
    // }
    if (imageOut[i][0] == 0) {
      num_left_lost++;
      Aver_left_lost += i;
    }
    if (imageOut[i][1] == 93) {
      num_right_lost++;
      Aver_right_lost += i;
      if (imageOut[i][0] == 0) num_all_lost++;
    }

    /* �����߽� ������������ �����Ƿ��Ҷ��� */ /* ��ת�� */
    if (/* g_ucFlagRoundabout != FlagRoundabout_R_2 && g_ucFlagRoundabout != FlagRoundabout_R_4 && g_ucFlagRoundabout != FlagRoundabout_L_2 && */ imageOut[i][0] > (LCDW / 2 - 5) && imageOut[i][1] > (LCDW - 5)) {
      /* �Ҷ��ߴ��� */
      // RoadNoSideProcess(UpdowmSide, imageOut, 1, i);
      g_ucFlagW = 1;
      L_arry = 5;
      R_arry = 0;  //ȡ����ѯ�ұ߽�
      // if (num_left_lost) Aver_left_lost /= num_left_lost;  //����ǰ����������ƽ��
      // if (num_right_lost) Aver_right_lost /= num_right_lost;
      // return 1;
    }

    /* ����ұ߽� ������������ �����Ƿ����� */ /* ��ת�� */
    if (/* g_ucFlagRoundabout != FlagRoundabout_R_2 && g_ucFlagRoundabout != FlagRoundabout_L_2 && g_ucFlagRoundabout != FlagRoundabout_R_4 && */ imageOut[i][1] < (LCDW / 2 + 5) && imageOut[i][0] < 5) {
      /* ���ߴ��� */
      // RoadNoSideProcess(UpdowmSide, imageOut, 2, i);
      g_ucFlagW = 2;
      R_arry = 5;
      L_arry = 0;  //ȡ����ѯ��߽�
      // if (num_left_lost) Aver_left_lost /= num_left_lost;
      // if (num_right_lost) Aver_right_lost /= num_right_lost;
      // return 2;
    }
  }

  if (num_left_lost) Aver_left_lost /= num_left_lost;
  if (num_right_lost) Aver_right_lost /= num_right_lost;
  return 0;
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�UpdownSideGet
*  ����˵������ȡ�ϱ���
*  ����˵��:            imageInput  ����Եͼ������
*                     imageOut    �����±�������
*  �������أ�0
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע����Ҫ��
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t UpdownSideGet(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[2][LCDW]) {
  uint8_t i = 0, j = 0;
  uint8_t last = 55;

  imageOut[0][47] = 0;
  imageOut[1][47] = 59;
  memset(imageOut, 0, sizeof(imageOut));
  /* �����߱ȽϽ����� �ж��Ƿ��� */
  for (i = last; i >= 0; i--) {  //��last�п�ʼ�������������ҵ���һ���߽�
    if (!imageInput[i][47]) {
      imageOut[0][47] = i;
      break;
    }
  }
  if (i < 0) {
    return 0;
  }
  /* ��������Ѱ�ұ��� */
  for (i = 47 - 1; i > 0; i--) {
    imageOut[0][i] = 0;
    imageOut[1][i] = 59;

    /* ���ݱ߽��������� Ѱ�ұ߽� */
    for (j = imageOut[0][i + 1] + 5; j > 0; j--) {
      if (!imageInput[j][i]) {
        imageOut[0][i] = j;
        break;
      }
    }
  }
  /*�������� Ѱ�ұ���*/
  for (i = 47 + 1; i < 93; i++) {
    imageOut[0][i] = 0;
    imageOut[1][i] = 59;

    /* ���ݱ߽��������� Ѱ�ұ߽� */
    for (j = imageOut[0][i - 1] + 5; j > 0; j--) {
      if (!imageInput[j][i]) {
        imageOut[0][i] = j;
        break;
      }
    }
  }
  return 0;
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�TurnNumSideGet
*  ����˵������ȡ���ߵ�ת�۵����
*  ����˵��:��
*  �������أ�0
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע����Ҫ�������
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t TurnNumSideGet(void) {
  char i;
  char err_front = 0, err_back = 0, err_front1 = 0, err_back1 = 0;

  num_up_turn = 0;
  num_left_turn = 0;
  num_right_turn = 0;
  memset(up_turn, 0, sizeof(up_turn));
  memset(left_turn, 0, sizeof(left_turn));
  memset(right_turn, 0, sizeof(right_turn));
  //�����ϱ����ж��ٸ��۵� 10-80
  for (i = 10; i < 80; i++) {
    err_front = UpdowmSide[0][i - 4] - UpdowmSide[0][i];  //��ߵ�y-�˵��y
    err_front1 = UpdowmSide[0][i - 2] - UpdowmSide[0][i];
    while (UpdowmSide[0][i] == UpdowmSide[0][i + 1]) {
      i++;
    }
    err_back = UpdowmSide[0][i] - UpdowmSide[0][i + 4];  //�˵��y-�ұߵ�y
    err_back1 = UpdowmSide[0][i] - UpdowmSide[0][i + 2];
    if (err_front * err_back < 0 && err_front1 * err_back1 < 0 && UpdowmSide[0][i] > 5) {
      num_up_turn++;                                        //ĳ�������ߵ�y������������С����
      if (num_up_turn <= 10) up_turn[num_up_turn - 1] = i;  // x���
    }
    /* ****************   ********************        */
    /*                  *  */  //ʾ��ͼbyzhou
  }
  //����������ж��ٸ��۵� ��55-20
  for (i = 55; i > 20; i--) {
    err_front = ImageSide[i + 1][0] - ImageSide[i][0];

    while (ImageSide[i][0] == ImageSide[i - 1][0]) {
      i--;
    }
    err_back = ImageSide[i][0] - ImageSide[i - 1][0];
    if (err_front * err_back < 0) {
      num_left_turn++;
      if (num_left_turn <= 10 && i < 40) left_turn[num_left_turn - 1] = i;  // y���
    }
  }
  //�����ұ����ж��ٸ��۵� ��55-20
  for (i = 55; i > 20; i--) {
    err_front = ImageSide[i + 1][1] - ImageSide[i][1];

    while (ImageSide[i][1] == ImageSide[i - 1][1]) {
      i--;
    }
    err_back = ImageSide[i][1] - ImageSide[i - 1][1];

    if (err_front * err_back < 0) {
      num_right_turn++;
      if (num_right_turn <= 10 && i < 40) right_turn[num_right_turn - 1] = i;  // y���}
    }
  }
  return 0;
}
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�ImagePortFilter
*  ����˵��������һ�����
*  ����˵��:        imageInput      :��Ե��ֵ������
*                 imageOut        ���������
*  �������أ���
*  �޸�ʱ�䣺2022/6/23 ������
*  ��   ע����Ҫ��
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void ImagePortFilter(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[LCDH][LCDW]) {
  uint8_t temp = 0;

  for (int i = 1; i < LCDH - 1; i++) {
    for (int j = 1; j < LCDW - 1; j++) {
      temp = imageInput[i - 1][j - 1] + imageInput[i - 1][j] + imageInput[i - 1][j + 1] + imageInput[i][j - 1] + imageInput[i][j] + imageInput[i][j + 1] + imageInput[i + 1][j - 1] + imageInput[i + 1][j] + imageInput[i + 1][j + 1];

      /* ������5�����Ǳ��� �����õ� ���Ե��������Ż��˲�Ч�� */
      if (temp > 4) {
        imageOut[i][j] = 1;
      } else {
        imageOut[i][j] = 0;
      }
    }
  }
}

uint8_t spinodalX;
uint8_t spinodalY;
int drv_err;  // 20-30����ֵƽ��ֵ
int drv_err2;
int drv_err3;
void TFT_Show_Camera_Info(void) {
  //����K2������Ļ���淽��۲�ͼ��
  //    if(KEY_Read(KEY2)==0)
  //    {while(!KEY_Read(KEY2));
  //        while(1){
  //            if(KEY_Read(KEY2) == 0)     //�ٰ�K2������Ļ
  //            {
  //                while(!KEY_Read(KEY2));
  //                break;
  //            }
  //        }
  //    }
  //
  if (KEY_Read(KEY2) == 0) {
    TFTSPI_Fill_Area(0, 0, 93, 59, u16BLACK);  // ��������ɫ��
    TFTSPI_BinRoadSide(ImageSide);             //���ұ���
    TFTSPI_BinRoad_Leftturn(left_turn);        //��ת�۵�
    TFTSPI_BinRoad_Rightturn(right_turn);
    TFTSPI_Draw_Line(0, ROAD_MAIN_ROW, 94, ROAD_MAIN_ROW, u16BLUE);  //������
    TFTSPI_Draw_Line(0, CURVE_ROW, 94, CURVE_ROW, u16RED);           //���������
  } else if (KEY_Read(KEY1) == 0)                                    //����K0 ��ʾ����ͼ��
  {
    TFTSPI_Fill_Area(0, 0, 93, 59, u16BLACK);  // ��������ɫ��
    TFTSPI_BinRoad_UpdownSide(UpdowmSide);     //���±���                   //        TFTSPI_Draw_Line(0, ROAD_MAIN_ROW, 93, ROAD_MAIN_ROW, u16RED);  //��������ʾ
    TFTSPI_BinRoad_Upturn(up_turn);            //        TFTSPI_Draw_Line(47, 0, 47, 60, u16RED);                        //��ʾ����
  } else if (KEY_Read(KEY0) == 0)              //����K0 ��ʾ����ͼ��
  {
    // TFTSPI_Road(0, 0, LCDH, LCDW, (uint8_t *)Image_Use);  //ԭͼ����ʾ     ���Եͼ��ͬʱֻ�ɿ���һ��
    TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *)Edge_arr);  //��ʾ��Ե��ȡͼ��
                                                                  //        TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *) Bin_Image);  //��ʾ��Ե��ȡͼ��
    TFTSPI_BinRoadSide(ImageSide);                                //���ұ���
    TFTSPI_BinRoad_UpdownSide(UpdowmSide);                        //���±���
                                                                  //        TFTSPI_Draw_Line(0, ROAD_MAIN_ROW, 93, ROAD_MAIN_ROW, u16RED);  //��������ʾ
                                                                  //        TFTSPI_Draw_Line(47, 0, 47, 60, u16RED);                        //��ʾ����
  } else                                                          //������K0��ֻ��ʾ �� �� �� ����
  {
    // TFTSPI_Fill_Area(0, 0, 93, 59, u16BLACK);   // ��������ɫ��
    TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *)Bin_Image);  //��ʾ��Ե��ȡͼ��
    TFTSPI_BinRoadSide(ImageSide);                                 //���ұ���
    TFTSPI_BinRoad_UpdownSide(UpdowmSide);                         //���±���
  }
  char txt[32];
  // V = ECPULSE1 * 100 * 48 * 21 / (105 * 512);  //�����ٶȣ�û�������Ҽ���ֵ��̫�ԣ�����������˿����޸�һ����ӵ���ʾ�У�
  sprintf(txt, "W:%02d", g_ucFlagW);  //ֱ������
  TFTSPI_P8X16Str(12, 0, txt, u16YELLOW, u16RED);

  sprintf(txt, "L:%02d", straightline);  //ֱ������
  TFTSPI_P8X16Str(12, 1, txt, u16YELLOW, u16RED);

  sprintf(txt, "Q%03d", drv_err / 10);  // 20�е�30�е�ƽ�����ֵ�����ڵ���������Ƶı���
  TFTSPI_P8X16Str(12, 2, txt, u16YELLOW, u16RED);

  sprintf(txt, "%03d", (uint16_t)Target_Speed);  //������Ʊ���
  TFTSPI_P8X16Str(12, 3, txt, u16YELLOW, u16RED);

  sprintf(txt, "L:%02d R:%02d A:%02d", num_left_lost, num_right_lost, num_all_lost);  //���ҡ�ȫ �������
  TFTSPI_P8X16Str(0, 4, txt, u16YELLOW, u16RED);

  sprintf(txt, "L:%02d U:%02d R:%02d", num_left_turn, num_up_turn, num_right_turn);  //���ϡ��� �۵���
  TFTSPI_P8X16Str(0, 5, txt, u16YELLOW, u16RED);

  sprintf(txt, "S%6dB%02dLD%02d", RAllPulse, num_black_white, Aver_left_lost);  //��������̣� �ڰ����������ڰ������������жϰ����ߣ�
  TFTSPI_P8X16Str(0, 6, txt, u16YELLOW, u16RED);

  sprintf(txt, "err:%04dFps:%02d", g_sSteeringError, Fps);  //���ֵ
  TFTSPI_P8X16Str(0, 7, txt, u16YELLOW, u16RED);

  sprintf(txt, "ServoP:%03dRD%02d", Servo_Center_Mid - ServoDuty, Aver_right_lost);  //���PWM
  TFTSPI_P8X16Str(0, 8, txt, u16YELLOW, u16RED);

  sprintf(txt, "R%01dT%01dY%01dB%01dZ%01dPD%02d%02d", g_ucFlagRoundabout, g_ucFlagT, g_ucFlagFork, count, g_ucFlagZebra, (int)Servo_P, (int)Servo_D);  //Ԫ�ر�־ ���� T�ο� Y�ο� ������ �������ߴ�1��ʼ��
  TFTSPI_P8X16Str(0, 9, txt, u16YELLOW, u16RED);
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  �������ƣ�void CameraCar(void)
*  ����˵��������ͷ����ѭ��
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2022��6��30��
*  ��   ע������ͷ�������򣬰������⣬ѭ����Ԫ��ʶ��Ԫ�ش�����⡣
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void CameraCar(void) {
  LED_Ctrl(LED1, RVS);  // LED��˸ ָʾ��������״̬
  drv_err = 0;          // 20-30
  drv_err2 = 0;         // 10-20
  drv_err3 = 0;         // 40-50
  leftdown_turn_flag = find_leftdown_guaidian();
  rightdown_turn_flag = find_rightdown_guaidian();
  StraightLine(UpdowmSide, 3);
  /* ����ٶ���PID���� */
  //����������ǰ10��20-30���е����ʣ��Դ����ı�����P 0Ϊ�м� -70< drv_err/10 <70
  for (uint8_t i = 20; i < 30; i++) drv_err += (ImageSide[i][0] + ImageSide[i][1] - LCDW);   // 20-29������ۼ�ֵ
  for (uint8_t i = 10; i < 20; i++) drv_err2 += (ImageSide[i][0] + ImageSide[i][1] - LCDW);  // 20-29������ۼ�ֵ
  /* ����ٶ���PID���� */
  //ֱ��������ٶ���pid
  if ((abs(drv_err2 / 10) > 10 || abs(drv_err / 10) > 10) && g_ucFlagW && g_ucFlagFork == NO && g_ucFlagRoundabout == NO && g_ucFlagT == NO) {
    Target_Speed = wandao_speed;
    Servo_P = Servo_P_wan;
    Servo_D = Servo_D_wan;
  } else if (jiasu_judge) {
    Target_Speed = jiasu_speed;
    Servo_P = Servo_P_default;
    Servo_D = Servo_D_default;
  } else {
    Target_Speed = initspeed;
    Servo_P = Servo_P_2;
    Servo_D = Servo_D_2;
  }
  //ֱ��������ٶ���pid
  ////////////////////////////////////////////////////////���� �����ⲻִ�к����Ԫ���ж� �����뿪�ز����Ϸ�Ϊ��ת���� �����·�Ϊ��ת���⣩//////////////////////////////////////////////////////////////////////////////////////////////////////
  if (game == no_start)  //���⣨����ʱ �ٶȡ��Ƕȡ��������ۼ�ֵ ������Ҫ����ã�
  {
    Target_Speed = chuku_speed;                        //�����ٶ�  ��һ�㲻���޸ģ�
    if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 0) {  //��ת����
      ServoDuty = chukujiao;
      if (right_chuku) {  //�������
        game = start;
        count++;
        RAllPulse = 0;
      }
    } else if (KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 1) {  //��ת����
      ServoDuty = -chukujiao;
      if (left_chuku) {  //�������
        game = start;
        count++;
        RAllPulse = 0;
      }
    } else if (KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 0) {  //����
      game = start;
      count++;
      RAllPulse = 0;
    } else if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 1) {  //����
      game = start;
      count = 2;
      RAllPulse = 0;
    }                                         /* else if(KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 0){//���µ���
                                                game = start;
                                                RAllPulse = 0;
                                           } */
    if (ServoDuty > 80) ServoDuty = 80;       //ƫ���޷� 0.766
    if (ServoDuty < -80) ServoDuty = -80;     //ƫ���޷�
    ServoCtrl(Servo_Center_Mid - ServoDuty);  //������
    return;
  }

  // Ԫ�ش������Ӵ˴���ʼ �ȴӻ�����ʼ T�� ��·�� ����������һ������һ��������׷
  ///////////////////////////////////////////////////��ʼѭ��////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (game == start) {
    /**************************************������***********************************************************************************************************************/

    // if (g_ucFlagRoundabout == 0 && g_ucFlagCross == 0 && g_ucFlagZebra == 0 && g_ucFlagFork == 0) {
    //   //��������
    //   if (RoadIsZebra(ImageSide, &g_ucFlagZebra) == 1) {  //�󳵿�
    //     g_ucFlagZebra = 1;
    //     RAllPulse = 0;
    //     g_ucFlagRoundabout = NO;
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
    //   }
    //   if (RoadIsZebra(ImageSide, &g_ucFlagZebra) == 2) {  //�ҳ���
    //     g_ucFlagZebra = 3;
    //     RAllPulse = 0;
    //     g_ucFlagRoundabout = NO;
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
    //   }
    // }
    // if (g_ucFlagZebra) {
    //   //�����ߴ�����һ��������Ҳ��ߣ�����ⷽ��������߻����Ҳ��ߣ��ڶ�������⣩
    //   ZebraProcess(ImageSide, &g_ucFlagZebra);
    // }

    /********************************����**********************************************************************************************************************************/
    // if (g_ucFlagW == 0 && g_ucFlagRoundabout == NO && g_ucFlagFork == Y_NO && g_ucFlagZebra == 0 && g_ucFlagT == 0) {
    //   // ��⻷��
    //   RoadIsRoundabout(UpdowmSide, Bin_Image, ImageSide);
    // }
    // if (g_ucFlagRoundabout != NO) {
    //   //����������Ϊ�󻷵����һ��� ���T�Ϳ� ��������T�Ϳ�һ��ʼʮ��������һ��ʼ�Ĵ���ʽҲһֱ�����Է���һ���⣬�ȹ���һ�׶κ����ж��ǻ�������T�֣�
    //   RoundaboutProcess(Bin_Image, ImageSide, UpdowmSide);
    // }
    // if (g_ucFlagT) {
    //   // T�ִ�����Ϊ��T�ֺ���T�֣���T�֣�����T��һֱ����ת ��T�֣�����T��һֱ����ת��
    //   TProcess(Bin_Image, UpdowmSide, ImageSide, &g_ucFlagT);
    // }

    /*********************************Y��·��************************************************************************************************************************/
    // if (/* g_ucFlagRoundabout == FlagRoundabout_L_1 || g_ucFlagRoundabout == FlagRoundabout_R_1 || */ g_ucFlagRoundabout == NO /* || FlagRoundabout_R == g_ucFlagRoundabout || FlagRoundabout_L == g_ucFlagRoundabout) */ && g_ucFlagFork == Y_NO && g_ucFlagZebra == 0 && g_ucFlagT == 0) {
    //   RoadIsFork(UpdowmSide, ImageSide);  //������һ�׶�Ҳ�ж�y����ΪĳЩʱ��y�ֻ�ͻ�����ʶ��
    // }
    // if (g_ucFlagFork != Y_NO) {
    //   //  Y�Ϳڴ������ﲻ�����ң���Ϊ��������������Ϊ��ת����ת���ɣ�����ѡ����Ƕ�Ϊ��ת��
    //   ForkProcess(ImageSide);
    // }

    //������һ�׶κͳ��������߶��ߴ���
    if (ImageSide[Round_ROW][1] == 93 && (g_ucFlagRoundabout == FlagRoundabout_R_1 || g_ucFlagRoundabout == FlagRoundabout_R)) {
      g_sSteeringError = 2 * ImageSide[Round_ROW][0] + Round_WIDTH - LCDW;
      AKMQZ = (60 - Round_ROW) * AKMQZ_xishu;
    } else if (ImageSide[Round_ROW][0] == 0 && (g_ucFlagRoundabout == FlagRoundabout_L_1 || g_ucFlagRoundabout == FlagRoundabout_L)) {
      g_sSteeringError = 2 * ImageSide[Round_ROW][1] - Round_WIDTH - LCDW;
      AKMQZ = (60 - Round_ROW) * AKMQZ_xishu;
    }
    //������һ�׶κͳ��������߶��ߴ���

    //���������߶��ߴ���
    else if (g_ucFlagRoundabout == FlagRoundabout_R_5) {
      g_sSteeringError = 2 * ImageSide[Round_ROW][0] + Round_WIDTH - LCDW;
      AKMQZ = (60 - Round_ROW) * AKMQZ_xishu;
    } else if (g_ucFlagRoundabout == FlagRoundabout_L_5) {
      g_sSteeringError = 2 * ImageSide[Round_ROW][1] - Round_WIDTH - LCDW;
      AKMQZ = (60 - Round_ROW) * AKMQZ_xishu;
    }
    //���������߶��ߴ���

    //���в��ߴ���
    else if (/* ImageSide[Round_ROW][1] == 93 && */ g_ucFlagRoundabout == FlagRoundabout_R_3) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_MID][0] + Round_WIDTH_MID - LCDW;
      AKMQZ = (60 - Round_ROW_MID) * AKMQZ_xishu;
    } else if (/* ImageSide[Round_ROW][0] == 0 &&  */ g_ucFlagRoundabout == FlagRoundabout_L_3) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_MID][1] - Round_WIDTH_MID - LCDW;
      AKMQZ = (60 - Round_ROW_MID) * AKMQZ_xishu;
    }
    //���в��ߴ���

    //����������
    else if (g_ucFlagRoundabout == FlagRoundabout_R_2) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_JIN][1] - Round_WIDTH_JIN - LCDW;
      AKMQZ = (60 - Round_ROW_JIN) * AKMQZ_xishu;
    } else if (g_ucFlagRoundabout == FlagRoundabout_L_2) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_JIN][0] + Round_WIDTH_JIN - LCDW;
      AKMQZ = (60 - Round_ROW_JIN) * AKMQZ_xishu;
    }
    //����������

    //����������
    else if (g_ucFlagRoundabout == FlagRoundabout_R_4) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_CHU][1] - Round_WIDTH_CHU - LCDW;
      AKMQZ = (60 - Round_ROW_CHU) * AKMQZ_xishu;
    } else if (g_ucFlagRoundabout == FlagRoundabout_L_4) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_CHU][0] + Round_WIDTH_CHU - LCDW;
      AKMQZ = (60 - Round_ROW_CHU) * AKMQZ_xishu;
    }
    //����������

    // y�ִ���
    else if (g_ucFlagFork == FlagFork_0 || g_ucFlagFork == FlagFork_2) {
      g_sSteeringError = 2 * ImageSide[Y_ROW][0] + Y_WIDTH - LCDW;
      AKMQZ = (60 - Y_ROW) * AKMQZ_xishu;
    }
    // y�ִ���

    /* t���� */
    else if (g_ucFlagT == 1) {  //��t
      g_sSteeringError = 2 * ImageSide[T_ROW_MID][0] + T_WIDTH_MId - LCDW;
      AKMQZ = (60 - T_ROW_MID) * AKMQZ_xishu;
    } else if (g_ucFlagT == 3) {  //��t
      g_sSteeringError = 2 * ImageSide[T_ROW_MID][1] - T_WIDTH_MId - LCDW;
      AKMQZ = (60 - T_ROW_MID) * AKMQZ_xishu;
    }
    /* t���� */

    //��t����
    else if (g_ucFlagT == 4) {
      g_sSteeringError = 2 * ImageSide[T_ROW][1] - T_WIDTH - LCDW;
      AKMQZ = (60 - T_ROW) * AKMQZ_xishu;
    } else if (g_ucFlagT == 2) {
      g_sSteeringError = 2 * ImageSide[T_ROW][0] + T_WIDTH - LCDW;
      AKMQZ = (60 - T_ROW) * AKMQZ_xishu;
    }
    //��t����
#ifdef cheku_1                                                                  //���ܳ������㷨
    else if (g_ucFlagZebra == 1) {                                              //�󳵿�
      if (ImageSide[ROAD_MAIN_ROW][1] > 47 && ImageSide[ROAD_MAIN_ROW][0] < 5)  //����ұ�����Ч�����ʧЧ�͸����ұ���
      {
        g_sSteeringError = 2 * ImageSide[ROAD_MAIN_ROW][1] - ROAD_MAIN_ROW - LCDW;
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else if (ImageSide[ROAD_MAIN_ROW][1] > 47 && ImageSide[ROAD_MAIN_ROW][0] > 5)  //����Ч����ѭ��
      {
        g_sSteeringError = RoadGetSteeringError(ImageSide, ROAD_MAIN_ROW);
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else {
        g_sSteeringError = 0;
      }

    } else if (g_ucFlagZebra == 3) {                                                 //�ҳ���
      if (ImageSide[ROAD_MAIN_ROW][0] < 47 && ImageSide[ROAD_MAIN_ROW][1] > 93 - 5)  //���zuo������Ч�ұ���ʧЧ�͸��������
      {
        g_sSteeringError = 2 * ImageSide[ROAD_MAIN_ROW][0] + ROAD_MAIN_ROW - LCDW;
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else if (ImageSide[ROAD_MAIN_ROW][0] < 47 && ImageSide[ROAD_MAIN_ROW][1] < 93 - 5)  //����Ч����ѭ��
      {
        g_sSteeringError = RoadGetSteeringError(ImageSide, ROAD_MAIN_ROW);
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else {
        g_sSteeringError = 0;
      }
    }
#else
    //���⴦��������
    else if (g_ucFlagZebra == 1) {  //��
      g_sSteeringError = 0;
    } else if (g_ucFlagZebra == 3) {  //��
      g_sSteeringError = 0;
    }
//���⴦��
#endif
    //���⴦��

    //�����Ǵ���
    else if (g_ucFlagW) {
      if (ImageSide[CURVE_ROW][0] == 0) {
        g_sSteeringError = 2 * ImageSide[CURVE_ROW][1] - CURVE_WIDTH - LCDW;  // 47Ϊ��30�е��������
        AKMQZ = (60 - CURVE_ROW) * AKMQZ_xishu;
      } else if (ImageSide[CURVE_ROW][1] == 93) {
        g_sSteeringError = 2 * ImageSide[CURVE_ROW][0] + CURVE_WIDTH - LCDW;
        AKMQZ = (60 - CURVE_ROW) * AKMQZ_xishu;
      } else {
        g_sSteeringError = RoadGetSteeringError(ImageSide, CURVE_ROW);
        AKMQZ = (60 - CURVE_ROW) * AKMQZ_xishu;
      }
    }
    //�����Ǵ���

    //����ѭ��
    else {
      if (ImageSide[ROAD_MAIN_ROW][0] == 0) {
        g_sSteeringError = 2 * ImageSide[ROAD_MAIN_ROW][1] - ROAD_MAIN_WIDTH - LCDW;  // 47Ϊ��30�е��������
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else if (ImageSide[ROAD_MAIN_ROW][1] == 93) {
        g_sSteeringError = 2 * ImageSide[ROAD_MAIN_ROW][0] + ROAD_MAIN_WIDTH - LCDW;
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else {
        g_sSteeringError = RoadGetSteeringError(ImageSide, ROAD_MAIN_ROW);
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      }
    }
    //����ѭ��
    /* ʮ�ִ��� */
    if (ImageSide[Round_ROW][1] >= 93 - 5 && ImageSide[Round_ROW][0] <= 5) {
      g_sSteeringError = 0;
    }
    /* ʮ�ִ��� */
    /* ͨ���ٶȾ��� */
    if (straightline < despeed_threshold) {  //���ܳ�
      Target_Speed = initspeed - (60 - straightline) * despeed;
    }
    /* ͨ���ٶȾ��� */
  }

  ////////////////////////////////////////////////////////////������/////////////////////////////////////////////////////////////////////////////////////////////
  if (game == over)  //��⣨ͬ����һ����
  {
    Target_Speed = jinku_speed;  //�����ٶ�  ��һ�㲻���޸ģ�
    Servo_P = Servo_P_jinku;     //����
    Servo_D = Servo_D_jinku;
    if (RAllPulse > 3000) {
      Target_Speed = 0;
    }
    // if (Target_Speed != 0&&straightline < 20) {  //���ܳ�
    //   Target_Speed = jinku_speed - (60 - straightline) * 8;
    // }

    if (KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 1) {  //��ת���
      ServoDuty = -jinkujiao;
    } else if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 0) {  //��ת���
      ServoDuty = jinkujiao;
    } else if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 1) {
      if (g_ucFlagZebra == 3) ServoDuty = jinkujiao;   // youת
      if (g_ucFlagZebra == 1) ServoDuty = -jinkujiao;  // zuoת���
    }
    if (ServoDuty > 80) ServoDuty = 80;       //ƫ���޷� 0.766
    if (ServoDuty < -80) ServoDuty = -80;     //ƫ���޷�
    ServoCtrl(Servo_Center_Mid - ServoDuty);  //������
    return;
  }


  
  Time = STM_GetNowUs(STM0);
  if (LastTime == 0) {
    ServoDuty = (g_sSteeringError * Servo_P + (g_sSteeringError - g_sSteeringError_last) * Servo_D / 100000000) / 10;  //ƫ��Ŵ�
    Fps = 0;
  } else {
    Fps = 1.0 / (Time - LastTime) * 1000000;
    ServoDuty = (g_sSteeringError * Servo_P + ((g_sSteeringError - g_sSteeringError_last) / ((1 / Fps) * 50)) * Servo_D / 100000000) / 10;  //ƫ��Ŵ�
  }
  LastTime = Time;
  g_sSteeringError_last = g_sSteeringError;
  if (ServoDuty > 80) ServoDuty = 80;       //ƫ���޷� 0.766
  if (ServoDuty < -80) ServoDuty = -80;     //ƫ���޷�
  ServoCtrl(Servo_Center_Mid - ServoDuty);  //������
}
/*********************************************************************************************************************************/
/*********************************************************************************************************************************/
//      //ʮ�ֲ���δ�õ�
//     if(g_ucFlagRoundabout == 0 && g_ucFlagCross == 0 && g_ucFlagFork == 0)
//     {
//         /* ���ʮ�� */
//         RoadIsCross(ImageSide, &g_ucFlagCross);
//     }
//     if(g_ucFlagCross)
//     {
//         /* ʮ�ִ��� */
//         CrossProcess(Image_Use, ImageSide, &g_ucFlagCross);
//     }
