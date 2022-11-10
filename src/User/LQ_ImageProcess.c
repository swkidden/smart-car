/*
 * @LastEditors: Dark white
 * @LastEditTime: 2022-07-22 16:01:22
 * @FilePath: \jingsu\src\User\LQ_ImageProcess.c
 * @Description:
 *
 * Copyright (c) 2022 by Dark white, All Rights Reserved.
 */
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】北京龙邱智能科技TC264DA核心板
【编    写】zyf/chiusir
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2020年4月10日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://longqiu.taobao.com
------------------------------------------------
【dev.env.】Hightec4.9.3/Tasking6.3及以上版本
【Target 】 TC264DA
【Crystal】 20.000Mhz
【SYS PLL】 200MHz
基于iLLD_1_0_1_11_0底层程序
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
uint8_t leftdown_turn_flag = 0;   //左下转折点标志
uint8_t rightdown_turn_flag = 0;  //右下转折点标志
uint8_t leftup_turn_flag = 0;     //左下转折点标志
uint8_t rightup_turn_flag = 0;    //右下转折点标志
uint8_t xianhuan_L = 0;           //线环切点纵坐标
uint8_t xianhuan_R = 0;           //线环切点纵坐标
uint8_t leftdown_y = 0;           //拐点坐标
uint8_t leftdown_x = 0;
uint8_t rightdown_y = 0;
uint8_t rightdown_x = 0;
uint8_t ImageSide[LCDH][2];    //左右边线数组 0左 1右
uint8_t UpdowmSide[2][LCDW];   //上下边线数组 0上 1下
uint8_t up_turn[10];           //上边线转折点数组 描述横坐标
uint8_t left_turn[10];         //左边线转折点数组 描述纵坐标
uint8_t right_turn[10];        //右边线转折点数组 描述纵坐标
uint8_t num_left_lost = 0;     //左侧丢线个数
uint8_t num_right_lost = 0;    //右侧丢线个数
uint16_t Aver_left_lost = 0;   //左侧丢线的平均行数
uint16_t Aver_right_lost = 0;  //右侧丢线的平均行数
uint8_t num_all_lost = 0;      //左右同行丢线个数
uint8_t num_up_turn = 0;       //上边线转折点个数
uint8_t num_left_turn = 0;     //左边线转折点个数
uint8_t num_right_turn = 0;    //右边线转折点个数
uint8_t anleg_up_turn = 0;     //转折点角度
uint8_t anleg_left_turn = 0;
uint8_t anleg_right_turn = 0;
uint8_t num_black_white = 0;  //黑白相间个数
game_status game = no_start;  //比赛状态
uint8_t count = 0;            //斑马线次数
uint8_t straightline = 0;     //直道长度
uint8_t threerightguaidian_x = 93;
uint8_t threerightguaidian_y = 0;
uint8_t threeleftguaidian_x = 93;
uint8_t threeleftguaidian_y = 0;
/**  @brief    转向误差  */
sint16 g_sSteeringError = 0;
uint8_t AKMQZ = 0;  //控制行距离车身距离用于阿克曼转向
sint16 g_sSteeringError_last = 0;
/**  @brief    丢线标志位  */
uint8_t g_ucIsNoSide = 0;
/**  @brief    环岛标志位  */
FlagRoundabout g_ucFlagRoundabout = NO;

/**  @brief    十字标志位  */
uint8_t g_ucFlagCross = 0;

/**  @brief    斑马线标志位  */
uint8_t g_ucFlagZebra = 0;

/**  @brief    Y型岔口标志位  */
FlagFork g_ucFlagFork = Y_NO;
uint8_t g_ucForkNum = 0;

/**  @brief    T型岔口标志位  */
uint8_t g_ucFlagT = 0;
/* 弯道标志位 */
uint8_t g_ucFlagW = 0;       // 1为右弯道 2为左弯道
pid_param_t BalDirgyro_PID;  // 方向PID

float Servo_P = 16;
float Servo_D = 10;
char txt[30];
uint32_t Time = 0;
uint32_t LastTime = 0;
uint16_t Fps = 0;
/*!
 * @brief    画边线
 *
 * @param
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2020/6/28 星期日
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
 * @description: 画上边线转折点
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
 * @description: 画左边线转折点
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
 * @description: 画右边线转折点
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
//  * @description: 判断是否是直线
//  * @param {int} *
//  * @param {int} coordinatesSize点个数
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
 * @brief    判断上边线是否单调
 * @param    X1 :起始X点
 * @param    X2 :终止X点              X1 < X2
 * @param    imageIn ： 边线数组
 *
 * @return   0：不单调or错误， 1：单调递增， 2：单调递减
 *
 * @note
 *
 * @see
 *
 * @date     2021/11/30 星期二
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
 * @description: 用于判断赛道长度
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
 * @brief    判断是否是直道
 *
 * @param    image ： 二值图像信息
 *
 * @return   0：不是直道， 1：直道
 *
 * @note     思路：两边边线都单调
 *
 * @see
 *
 * @date     2020/6/23 星期二
 */
uint8_t RoadIsStraight(uint8_t imageSide[LCDH][2]) {
  uint8_t i = 0;
  uint8_t leftState = 0, rightState = 0;

  /* 左边线是否单调 */
  for (i = ROAD_START_ROW - 1; i > ROAD_END_ROW; i--) {
    if (imageSide[i][0] + 5 < imageSide[i + 1][0]) {
      leftState = 1;
      break;
    }
  }

  /* 右边线是否单调 */
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
*  函数名称：TProcess
*  功能说明：T字补线处理
*  参数说明:
*               imageUp         ：上下边线
*               imageInput      :边缘二值化图像数组
*               imageSide       ：左右边线
*               *flag           :标志位
*  函数返回：无
*  修改时间：2022/6/23 星期四
*  备   注：判断标志位的值，丢线个数，折点个数等等 可以将车放到赛道上，观察图像或者数值，以实际情况调整常数
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t TProcess(uint8_t imageInput[LCDH][LCDW], uint8_t imageUp[2][LCDW], uint8_t imageSide[LCDH][2], uint8_t *flag) {
  switch (*flag) {
    case 1:
      Target_Speed = T_speed_Z;
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //蜂鸣器响                                                                       //右T形 从环岛第二阶段（不丢线阶段）进入
      if (T_2) {                                 //如果左右同时丢线且上线单调就出T PS:t字出识别同环岛
        *flag = 2;
        RAllPulse = 0;
      } else if (RAllPulse >= 30000) {
        *flag = 0;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器灭
      }
      break;
    case 2:  //出右T形
      Target_Speed = T_speed_C;
      Servo_P = Servo_P_chut;
      Servo_D = Servo_D_chut;
      if (R_T_2) {
        *flag = 0;  // 切换状态 出T字完成
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);
      }
      break;

    case 3:  //左T形
      Target_Speed = T_speed_Z;
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //蜂鸣器响                                                                       //右T形 从环岛第二阶段（不丢线阶段）进入
      if (T_2) {
        *flag = 4;
        RAllPulse = 0;
      } else if (RAllPulse >= 30000) {
        *flag = 0;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器灭
      }
      break;
    case 4:  //出左T形
      Target_Speed = T_speed_C;
      Servo_P = Servo_P_chut;
      Servo_D = Servo_D_chut;
      if (L_T_2) {
        *flag = 0;  //切换状态 出T字完成
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);
      }
      break;
  }
  return 0;
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  函数名称：RoadIsZebra
*  功能说明：斑马线检测函数
*  参数说明：无
*  函数返回：无
*  修改时间：2022/6/23 星期四
*  备   注：判断标志位的值，丢线个数，折点个数等等 可以将车放到赛道上，观察图像或者数值，以实际情况调整常数
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t RoadIsZebra(uint8_t imageSide[LCDH][2], uint8_t *flag) {
  uint8_t j, i, num_black_white = 0, count1 = 0, diuL = 0, diuR = 0, budiuL = 0, budiuR = 0;
  //用控制行上10行进行判断防止车辆因为车库跑飞
  if (ImageSide[ROAD_MAIN_ROW - 5 - 5][0] <= 5 && ImageSide[ROAD_MAIN_ROW - 5 - 5][1] <= 47) {  //一边丢线一边往中间突变
    if (ImageSide[ROAD_MAIN_ROW - 10 - 5][1] >= 47 && ImageSide[ROAD_MAIN_ROW - 5][1] >= 47 && ImageSide[ROAD_MAIN_ROW][1] >= 47) {
      for (i = ROAD_MAIN_ROW - 10 - 5; i < ROAD_MAIN_ROW - 5; i++) {
        for (j = 20; j < 70; j++) {
          if (Bin_Image[i][j] == 1 && Bin_Image[i][j - 1] == 0) {
            num_black_white++;
            // printf("%d\n", num_black_white);
            if (num_black_white > 55) {
              RAllPulse = 0;  //编码器累计值清零
              return 1;       //左车库
            }
          }
        }
      }
    }
  }
  if (ImageSide[ROAD_MAIN_ROW - 5 - 5][1] >= 90 && ImageSide[ROAD_MAIN_ROW - 5 - 5][0] >= 47) {  //一边丢线一边往中间突变
    if (ImageSide[ROAD_MAIN_ROW - 10 - 5][0] <= 47 && ImageSide[ROAD_MAIN_ROW - 5][0] <= 47 && ImageSide[ROAD_MAIN_ROW][0] <= 47) {
      for (i = ROAD_MAIN_ROW - 10 - 5; i < ROAD_MAIN_ROW - 5; i++) {
        for (j = 20; j < 70; j++) {
          if (Bin_Image[i][j] == 1 && Bin_Image[i][j - 1] == 0) {
            num_black_white++;
            // printf("%d\n", num_black_white);
            if (num_black_white > 55) {
              RAllPulse = 0;  //编码器累计值清零
              return 2;       //右车库
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
  //   RAllPulse = 0;  //编码器累计值清零
  //   return 1;
  // }
  // if (num_black_white > 55 && count1 > 55 && diuR > 10 && budiuL > 18) {
  //   RAllPulse = 0;  //编码器累计值清零
  //   return 2;
  // }
  return 0;
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  函数名称：RoadIsRoundabout
*  功能说明：环岛检测函数
*  参数说明:
*               Upimage        ：上下边线
*               imageInput     :边缘二值化图像数组
*               image          ：左右边线
*  函数返回：0
*  修改时间：2022/6/23 星期四
*  备   注：判断标志位的值，丢线个数，折点个数等等 可以将车放到赛道上，观察图像或者数值，以实际情况调整常数
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t RoadIsRoundabout(uint8_t Upimage[2][LCDW], uint8_t imageInput[LCDH][LCDW], uint8_t image[LCDH][2]) {
  //右环岛判断（正入）
  //上边线在左右边线最大最小值上的差值大于30 且 左边线没有丢线 且 右边线丢线 且 左边先折点数为0 且 上边线折点数大于等于3 且 右边线折点数大于1 则 判断为右环岛
  if (R_Round_0) {
    RAllPulse = 0;
    g_ucFlagRoundabout = FlagRoundabout_R;
    return 0;
  }
  /* 左环岛判断（正入） */
  if (L_Round_0) {
    RAllPulse = 0;
    g_ucFlagRoundabout = FlagRoundabout_L;
    return 0;
  }
  return 0;
}
/*!
 * @brief    获取环岛边线
 *
 * @param    imageInput ： 二值图像信息
 * @param    imageOut   ： 边线数组
 * @param    status     ： 1：左环岛(边线)  2：右环岛(边线)
 *
 * @return
 *
 * @note     思路：环岛一边边线严格单调，根据一边边线，获取另一边线
 *
 * @see
 *
 * @date     2020/6/23 星期二
 */
void RoundaboutGetSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageSide[LCDH][2], uint8_t status) {
  uint8_t i = 0, j = 0;

  switch (status) {
      /* 左环岛 */
    case 1: {
      /* 重新确定左边界 */
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
      /* 重新确定右边界 */
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
      /* 重新确定上边界 */
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
      /* 重新确定下边界 */
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
 * @brief    判断边线是否存在弧形
 *
 * @param    imageInput ： 二值图像信息
 * @param    imageOut   ： 边线数组
 * @param    status     ： 1：左边线  2：右边线
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2020/6/23 星期二
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

          /* 有弧线 */
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

          /* 有弧线 */
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
 * @brief    判断边线是否存在弧形
 *
 * @param    SideInput ： 上边线数组
 * @param    num       ： 检测幅度
 * @param    index     ： 最低点
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2021/12/01 星期三
 */
uint8_t UpSideErr(uint8_t SideInput[2][LCDW], uint8_t status, uint8_t num, uint8_t *index) {
  uint8_t dec = 0, inc = 0, i;
  //上线是否右突起
  switch (status) {
    case 1:
      for (i = 159 - 1; i > 0; i--) {
        if (UpdowmSide[0][i] > 1 && UpdowmSide[0][i + 1] > 1) {
          if (UpdowmSide[0][i] >= UpdowmSide[0][i + 1])
            inc++;
          else
            dec++;
          /* 有弧线 */
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
    //下边线
    case 2:
      for (i = 159 - 1; i > 0; i--) {
        if (UpdowmSide[1][i] != 1 && UpdowmSide[1][i + 1] != 1) {
          if (UpdowmSide[1][i] >= UpdowmSide[1][i + 1])
            inc++;
          else
            dec++;
          /* 有弧线 */
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
 * @brief    补线处理
 *
 * @param    imageSide  : 边线
 * @param    status     : 1：左边线补线   2：右边线补线
 * @param    startX     : 起始点 列数
 * @param    startY     : 起始点 行数
 * @param    endX       : 结束点 列数
 * @param    endY       : 结束点 行数
 *
 * @return
 *
 * @note     endY 一定要大于 startY
 *
 * @see
 *
 * @date     2020/6/24 星期三
 */
void ImageAddingLine(uint8_t imageSide[LCDH][2], uint8_t status, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY) {
  int i = 0;

  /* 直线 x = ky + b*/
  float k = 0.0f, b = 0.0f;
  switch (status) {
    case 1:  //左补线
    {
      k = (float)((float)endX - (float)startX) / (float)((float)endY - (float)startY);
      b = (float)startX - (float)startY * k;

      for (i = startY; i < endY; i++) {
        imageSide[i][0] = (uint8_t)(k * i + b);
      }
      break;
    }

    case 2:  //右补线
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
 * @brief    寻找跳变点
 *
 * @param    imageSide   ： 边线数组
 * @param    status      ：1：左边界   2：右边界
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2020/6/24 星期三
 */
uint8_t ImageGetHop(uint8_t imageSide[LCDH][2], uint8_t state, uint8_t *x, uint8_t *y) {
  int i = 0;
  uint8_t px = 0, py = 0;
  uint8_t count = 0;
  switch (state) {
    case 1:
      /* 寻找跳变点 */
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
      /* 寻找跳变点 */
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
*  函数名称：RoundaboutProcess
*  功能说明：环岛补线处理
*  参数说明:
*               UpdowmSide          ：上下边线
*               imageInput          :边缘二值化图像数组
*               imageSide           ：左右边线
*  函数返回：无
*  修改时间：2022/6/23 星期四
*  备   注：判断标志位的值，丢线个数，折点个数等等 可以将车放到赛道上，观察图像或者数值，以实际情况调整常数
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void RoundaboutProcess(uint8_t imageInput[LCDH][LCDW], uint8_t imageSide[LCDH][2], uint8_t UpdowmSide[2][LCDW]) {
  switch (g_ucFlagRoundabout) {
      /* 发现右环岛 第一次丢线 在不丢线环岛中央时切换到下一个状态 */
    // 0
    case NO:
      break;
    // 1初见 右环岛 或 T型口 或 车库
    case FlagRoundabout_R:                       //识别到右测环岛 此时还未到环底部 因为右边丢线，所以需要处理，与进入T字时处理方法相同
      Target_Speed = Round_speed_S;              //调整速度
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //蜂鸣器响                                                                       //右T形 从环岛第二阶段（不丢线阶段）进入
      if ((R_Round_1))                           //环岛不丢状态2
      {
        g_ucFlagRoundabout = FlagRoundabout_R_1;  //切换状态
        RAllPulse = 0;                            //编码器累计值清零
      } else if (num_left_lost > 5 && RAllPulse > 3000) {
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;                   //环岛清空
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
        return;
      } else if (R_T_1) {  //行程大于5000 且 上边线是单调的 则判断为右T形
        g_ucFlagT = 1;
        RAllPulse = 0;            // T形口标志位 置为 1
        g_ucFlagRoundabout = NO;  //环岛清空
        return;
      }

      if (RoadIsZebra(ImageSide, &g_ucFlagZebra)) {
        g_ucFlagZebra = 3;
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
      }
      break;
    /* 2不丢线在第二次丢线时切换下一状态 */
    case FlagRoundabout_R_1:
      Target_Speed = Round_speed_S;
      if ((R_Round_2))                            //上线不是单调的则不是T型口　在寻找入环点
      {                                           //左侧不丢线 且 右侧丢线超过10个像素点 且 左侧没有折点 且 上方超过1个折点 则 此时开始入环
        RAllPulse = 0;                            //编码器累计值清零
        g_ucFlagRoundabout = FlagRoundabout_R_2;  //状态切换到下一阶段
      } else if (num_left_lost > 5) {
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;                   //环岛清空
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
        return;
      } else if (R_T_1) {  //行程大于5000 且 上边线是单调的 则判断为右T形
        g_ucFlagT = 1;
        RAllPulse = 0;            // T形口标志位 置为 1
        g_ucFlagRoundabout = NO;  //环岛清空
        return;
      } else if (RAllPulse > 10000) {              //行程大于5000退出
        RAllPulse = 0;                             // T形口标志位 置为 1
        g_ucFlagRoundabout = NO;                   // t字退出
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
        return;                                    //环岛清空
      }
      if (RoadIsZebra(ImageSide, &g_ucFlagZebra)) {
        g_ucFlagZebra = 3;
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
      }
      break;
    // 3 入岛判断是否进入成功 进入成功则切换至下一状态（入岛时）
    case FlagRoundabout_R_2:
      Servo_P = Servo_P_jinhuan_R;
      Servo_D = Servo_D_jinhuan_R;
      Target_Speed = Round_speed_D;               //减速
      if (R_Round_3) {                            //上线单调代表入环成功开始正常循迹
        g_ucFlagRoundabout = FlagRoundabout_R_3;  //切换状态
        RAllPulse = 0;                            //编码器累计值清零                                                                                    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);                                   //蜂鸣器灭
      }
      break;
    /* 4入环后正常循迹 */
    case FlagRoundabout_R_3:  //正常循迹当弯道处理（入岛后）
      Target_Speed = Round_speed_W;
      if (R_Round_4) {  //如果左右同时丢线且上线单调就出环
        g_ucFlagRoundabout = FlagRoundabout_R_4;
        RAllPulse = 0;
      }
      break;
    // 5 执行出岛动作（右转）
    case FlagRoundabout_R_4:
      Servo_P = Servo_P_chuhuan_R;
      Servo_D = Servo_D_chuhuan_R;
      Target_Speed = Round_speed_D;
      if (R_Round_5) {
        g_ucFlagRoundabout = FlagRoundabout_R_5;  //切换状态
        RAllPulse = 0;                            //例程清零
      }
      break;
    // 6出岛完成，右侧补线以确保车直行
    case FlagRoundabout_R_5:
      Servo_P = Servo_P_chuhuanhou_R;
      Servo_D = Servo_D_chuhuanhou_R;
      Target_Speed = Round_speed_S;
      if (R_Round_6)  //出环岛完成
      {
        g_ucFlagRoundabout = NO;                   //切换状态
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
        RAllPulse = 0;
      }
      break;
      /* ***********************************************左环岛************************************************* */
    case FlagRoundabout_L:  //识别到右测环岛 此时还未到环底部 因为右边丢线，所以需要处理，与进入T字时处理方法相同
      Target_Speed = Round_speed_S;
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //蜂鸣器响
      if (L_Round_1)                             //行进路程大于1500之后开始判断有没有再次出现丢线
      {
        g_ucFlagRoundabout = FlagRoundabout_L_1;  //切换状态
        RAllPulse = 0;                            //编码器累计值清零
      } else if (num_right_lost > 5 && RAllPulse > 3000) {
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;                   //环岛清空
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
        return;
      } else if (L_T_1) {
        RAllPulse = 0;            //行程大于5000 且 上边线是单调的 则判断为右T形
        g_ucFlagT = 3;            // T形口标志位 置为 1
        g_ucFlagRoundabout = NO;  // t字退出
        return;                   //环岛清空
      }
      if (RoadIsZebra(ImageSide, &g_ucFlagZebra)) {
        g_ucFlagZebra = 1;
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
      }
      break;
    /* 2不丢线在第二次丢线时切换下一状态 */
    case FlagRoundabout_L_1:
      Target_Speed = Round_speed_S;
      if (L_Round_2)                              //上线不是单调的则不是T型口　在寻找入环点
      {                                           //左侧不丢线 且 右侧丢线超过10个像素点 且 左侧没有折点 且 上方超过1个折点 则 此时开始入环
        RAllPulse = 0;                            //编码器累计值清零
        g_ucFlagRoundabout = FlagRoundabout_L_2;  //状态切换到下一阶段
      } else if (num_right_lost > 5) {
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;                   //环岛清空
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
        return;
      } else if (L_T_1) {
        RAllPulse = 0;                             //行程大于5000 且 上边线是单调的 则判断为右T形
        g_ucFlagT = 3;                             // T形口标志位 置为 1
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
        g_ucFlagRoundabout = NO;                   // t字退出
        return;                                    //环岛清空
      } else if (RAllPulse > 10000) {              //行程大于5000退出
        RAllPulse = 0;                             // T形口标志位 置为 1
        g_ucFlagRoundabout = NO;                   // t字退出
        return;                                    //环岛清空
      }
      if (RoadIsZebra(ImageSide, &g_ucFlagZebra)) {
        g_ucFlagZebra = 1;
        RAllPulse = 0;
        g_ucFlagRoundabout = NO;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
      }
      break;
    // 3 入岛判断是否进入成功 进入成功则切换至下一状态（入岛时）
    case FlagRoundabout_L_2:  //减速
      Servo_P = Servo_P_jinhuan_L;
      Servo_D = Servo_D_jinhuan_L;
      Target_Speed = Round_speed_D;
      if (L_Round_3) {                            //上线单调代表入环成功开始正常循迹
        g_ucFlagRoundabout = FlagRoundabout_L_3;  //切换状态
        RAllPulse = 0;                            //编码器累计值清零
      }
      break;
    /* 4入环后正常循迹 */
    case FlagRoundabout_L_3:  //正常循迹当弯道处理（入岛后）                             // Target_Speed = 3000;                                        //行程超过3500则切换状态，车辆正常循迹
      Target_Speed = Round_speed_W;
      if (L_Round_4) {  //如果左右同时丢线且上线单调就出环
        g_ucFlagRoundabout = FlagRoundabout_L_4;
        RAllPulse = 0;
      }
      break;
    // 5 执行出岛动作（右转）
    case FlagRoundabout_L_4:
      Servo_P = Servo_P_chuhuan_L;
      Servo_D = Servo_D_chuhuan_L;
      Target_Speed = Round_speed_D;
      if (L_Round_5) {
        g_ucFlagRoundabout = FlagRoundabout_L_5;  //切换状态
        RAllPulse = 0;                            //例程清零
      }

      break;
    // 6出岛完成，右侧补线以确保车直行
    case FlagRoundabout_L_5:
      Target_Speed = Round_speed_S;
      Servo_P = Servo_P_chuhuanhou_L;
      Servo_D = Servo_D_chuhuanhou_L;
      if (L_Round_6)  //出环岛完成
      {
        g_ucFlagRoundabout = NO;                   //切换状态
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
        RAllPulse = 0;
      }
      break;
      // 5左环岛与右环岛一致只是方向左右对调
      // case FlagRoundabout_L:
      //   Target_Speed = 2000;    //减速
      //   if (RAllPulse <= 3500)  //行进路程小于3500
      //   {
      //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //蜂鸣器响

      //     ImageAddingLine(imageSide, 1, 47, 0, 47 - 47 / 2, 58);  //可自行修改 左补线，不左转
      //   } else {
      //     errU1 = RoadUpSide_Mono(24, 70, UpdowmSide);  //上单调增或者减
      //     if (errU1 == 0)                               //上线不是单调的 再判断环岛
      //     {
      //       if (num_left_lost > 10 && num_right_lost == 0 && num_up_turn > 1 && num_right_turn == 0) {
      //         RAllPulse = 0;
      //         g_ucFlagRoundabout = FlagRoundabout_L_1;
      //       }
      //     } else {  //里程大于5000 且如果是单调的 则判断为左T形
      //       if (RAllPulse > 5000) {
      //         g_ucFlagT = 3;            // T形口标志位置为
      //         g_ucFlagRoundabout = NO;  //环岛清空
      //       }
      //     }
      //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器灭
      //   }

      /****************斑马线判定***************************************************/
      //          num_black_white = 0;
      //          //扫描二值化数据第30行 看右多少个黑白交替的边界
      //          for(i = LCDW-1; i > 20; i--)
      //          {
      //              if(Bin_Image[30][i] +  Bin_Image[30][i-1] == 1)
      //                  num_black_white++;
      //          }
      //          if(num_black_white >= 14)   //黑白交替大于14则判定为斑马线
      //          {
      //              g_ucFlagRoundabout = NO;  //环岛清空
      //              g_ucFlagZebra = 1;          //斑马线标志位置1
      //              RAllPulse = 0;      //编码器累计值清零
      //            }
      //          break;
      // 6
    // case FlagRoundabout_L_1:
    //   if (RAllPulse <= 3500)  //行进路程小于3500
    //   {
    //     Target_Speed = 2000;                                    //减速
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);               //蜂鸣器响
    //     ImageAddingLine(imageSide, 2, 47, 0, 47 + 47 / 2, 58);  //可自行修改 左转入岛
    //   } else {
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);           //蜂鸣器灭
    //     flag_up_line = RoadUpSide_Mono(5, 90, UpdowmSide);  //上线 单调 递增 返回1
    //     if (flag_up_line == 1) {
    //       g_ucFlagRoundabout = FlagRoundabout_L_2;
    //       RAllPulse = 0;
    //     }
    //   }
    //   break;
    // // 7
    // case FlagRoundabout_L_2:
    //   if (RAllPulse <= 3000)  //行进路程小于3000
    //   {
    //     Target_Speed = 2000;                                   //减速
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);              //蜂鸣器响
    //     ImageAddingLine(imageSide, 2, 47 / 3, 0, 47 / 2, 58);  //可自行修改 左转出岛
    //   } else {
    //     Target_Speed = 2000;  //减速
    //     if (num_left_lost > 15 && num_right_lost == 0 && num_right_turn == 0) {
    //       g_ucFlagRoundabout = FlagRoundabout_L_3;
    //       RAllPulse = 0;
    //     }
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器响
    //   }
    //   break;
    // // 8
    // case FlagRoundabout_L_3:
    //   if (RAllPulse <= 3500)  //行进路程小于3500
    //   {
    //     Target_Speed = 2000;                                    //减速
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);               //蜂鸣器响
    //     ImageAddingLine(imageSide, 1, 47, 0, 47 - 47 / 2, 58);  //可自行修改 左边补线 退出环岛
    //   } else {
    //     g_ucFlagRoundabout = NO;
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器响
    //   }
    //   break;
    default:
      break;
  }
}

/*!
 * @brief    获取十字边线
 *
 * @param    imageInput ： 二值图像信息
 * @param    imageOut   ： 边线数组
 *
 * @return
 *
 * @note     思路：从中间向两边搜线
 *
 * @see
 *
 * @date     2020/6/23 星期二
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
 * @brief    十字补线处理
 *
 * @param    imageInput ： 二值图像信息
 * @param    imageSide  ： 边线数组
 * @param    status     ：十字标志位   1：发现十字    2：进入十字   3：出十字
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2020/6/24 星期三
 */
void CrossProcess(uint8_t imageInput[LCDH][LCDW], uint8_t imageSide[LCDH][2], uint8_t *state) {
  uint8_t pointX = 0, pointY = 0;
  uint8_t leftIndex = 0;
  static uint8_t count = 0;
  switch (*state) {
    case 1: {
      /* 重新获取边线 */
      CrossGetSide(imageInput, imageSide);
      /* 寻找跳变点 */
      if (ImageGetHop(imageSide, 1, &pointX, &pointY)) {
        /* 补线 */
        ImageAddingLine(imageSide, 1, pointX, pointY, 0, ROAD_START_ROW);
      }
      leftIndex = pointY;
      pointX = 0;
      pointY = 0;
      /* 寻找跳变点 */
      if (ImageGetHop(imageSide, 2, &pointX, &pointY)) {
        /* 补线 */
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
      /* 检查弧线 */
      if (RoundaboutGetArc(imageSide, 1, 5, &leftIndex)) {
        /* 重新确定左边界 */
        RoundaboutGetSide(imageInput, imageSide, 1);
        if (ImageGetHop(imageSide, 1, &pointX, &pointY)) {
          /* 补线 */
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
      /* 重新确定左边界 */
      RoundaboutGetSide(imageInput, imageSide, 1);
      if (ImageGetHop(imageSide, 1, &pointX, &pointY)) {
        /* 检查弧线 */
        if (RoundaboutGetArc(imageSide, 1, 5, &leftIndex)) {
          /* 补线 */
          ImageAddingLine(imageSide, 1, pointX, pointY, imageSide[leftIndex][0], leftIndex);
        } else {
          /* 补线 */
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
*  函数名称：RoadIsFork
*  功能说明：Y字检测
*  参数说明:
*               imageInput      ：上下边线
*               imageSide       ：左右边线
*  函数返回：0
*  修改时间：2022/6/23 星期四
*  备   注：判断标志位的值，丢线个数，折点个数等等 可以将车放到赛道上，观察图像或者数值，以实际情况调整常数
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
/* 容易进y */
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
/* 容易进y */
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
*  函数名称：ForkProcess
*  功能说明：Y字补线处理
*  参数说明:
*               imageSide      ：左右边线
*  函数返回：0
*  修改时间：2022/6/23 星期四
*  备   注：判断标志位的值，丢线个数，折点个数等等 可以将车放到赛道上，观察图像或者数值，以实际情况调整常数
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
sint32 RAllFork = 0;
void ForkProcess(uint8_t imageSide[LCDH][2]) {
  switch (g_ucFlagFork) {
    case FlagFork_0:  // y型进入
      Target_Speed = Y_speed;
      Servo_P = Servo_P_Y;
      Servo_D = Servo_D_Y;
      g_ucFlagRoundabout = NO;
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);    //蜂鸣器响
      if (Y_1) {                                   //进入到y字中正常循迹
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器灭
        g_ucFlagFork = FlagFork_1;
        RAllPulse = 0;
      } else if (RAllPulse > 6000) {
        g_ucFlagFork = NO;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器灭
      }
      break;
    case FlagFork_1:  // y字中正常循迹
      if (Y_2) {
        g_ucFlagFork = FlagFork_2;  //切换到出
        RAllPulse = 0;              //清空里程
      }
      break;
    case FlagFork_2:  //出 补线
      Target_Speed = Y_speed;
      Servo_P = Servo_P_Y;
      Servo_D = Servo_D_Y;
      PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);    //蜂鸣器响
      if (Y_3) {                                   //出y成功
        g_ucFlagFork = Y_NO;                       //标志位恢复
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器灭
      } else if (RAllPulse > 6000) {
        g_ucFlagFork = NO;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器灭
      }
      break;
    default:
      break;
  }
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  函数名称：ZebraProcess
*  功能说明：斑马线车库处理
*  参数说明:
*               imageSide      ：左右边线
*               *state         :标志位
*  函数返回：无
*  修改时间：2022/6/23 星期四
*  备   注：判断标志位的值，丢线个数，折点个数等等 可以将车放到赛道上，观察图像或者数值，以实际情况调整常数
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void ZebraProcess(uint8_t imageSide[LCDH][2], uint8_t *state) {
  // 1左 3右

  switch (count) {
    case 1:                                                             //第一次识别到斑马线
      if (KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 1 && *state == 1) {  //误判
        *state = 0;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
      }
      if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 0 && *state == 3) {  //误判
        *state = 0;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
      }
      if (RAllPulse < 1000)  //补线防止左转或右转 补线方向与出库方向相关
      {
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);                       //蜂鸣器响
      } else if (*state == 1 && num_left_lost < 5 || RAllPulse > 2000)  //左车库
      {
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);                        //关闭蜂鸣器
        *state = 0;                                                      //清空斑马线标志位 以便下次触发
        count++;                                                         //进来次数加1
      } else if (*state == 3 && num_right_lost < 5 || RAllPulse > 2000)  //右车库
      {
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //关闭蜂鸣器
        *state = 0;                                //清空斑马线标志位 以便下次触发
        count++;                                   //进来次数加1
      }
      break;

    case 2:                                                                    //识别到第二次 代表可以入库了
      if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 1) {                        //向下调试模式
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);                              //蜂鸣器响
                                                                               // *state = 0;
        game = over;                                                           //切换比赛状态 进入入库模式
        RAllPulse = 0;                                                         //里程清零
        count++;                                                               //次数加1，可有可无
      } else if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 0 && *state == 3) {  //右侧
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);                              //蜂鸣器响
                                                                               // *state = 0;
        game = over;                                                           //切换比赛状态 进入入库模式
        RAllPulse = 0;                                                         //里程清零
        count++;                                                               //次数加1，可有可无
      } else if (KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 1 && *state == 1) {
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);  //蜂鸣器响
        game = over;                               //切换比赛状态 进入入库模式
        RAllPulse = 0;                             //里程清零
        count++;                                   //次数加1，可有可无
      } else {                                     //误判
        *state = 0;
        RAllPulse = 0;
        PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 0);  //蜂鸣器mie
      }
      break;
  }
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  函数名称：ZebraProcess
*  功能说明：斑马线车库处理
*  参数说明:
*               imageSide      ：左右边线
*               lineIndex      ：领跑行
*  函数返回：误差值
*  修改时间：2022/6/23 星期四
*  备   注：
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
int16_t RoadGetSteeringError(uint8_t imageSide[LCDH][2], uint8_t lineIndex) { return imageSide[lineIndex][0] + imageSide[lineIndex][1] - LCDW; }

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  函数名称：RoadIsNoSide
*  功能说明：判断是左右否丢线
*  参数说明：    imageInput  ：二进制图像数组
*             imageOut    ：边线数组[0]:左  [1]：右
*             lineIndex   ：指定行
*  函数返回：0：没有丢线   1:左边丢线  2：右边丢线  3： 左右都丢线   4：错误
*  修改时间：2022年6月24日
*  备   注：guo
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t RoadIsNoSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[LCDH][2], uint8_t lineIndex) {
  uint8_t state = 0;
  uint8_t i = 0;
  static uint8_t last = 59;

  imageOut[lineIndex][0] = 0;
  imageOut[lineIndex][1] = 93;
  /* 用距离小车比较近的行 判断是否丢线 */
  for (i = last; i > 1; i--) {
    if (!imageInput[lineIndex][i]) {
      imageOut[lineIndex][0] = i;
      break;
    }
  }
  if (i == 1) state = 1;  //左边界丢线

  for (i = last; i < 93; i++) {
    if (!imageInput[lineIndex][i]) {
      imageOut[lineIndex][1] = i;
      break;
    }
  }
  if (i == 93) {
    /* 左右边界丢线 */
    if (state == 1) state = 3;
    /* 右边界丢线 */
    else
      state = 2;
  }
  if (imageOut[lineIndex][1] <= imageOut[lineIndex][0]) state = 4;
  return state;
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  函数名称：UD_Road_Is_No_Side
*  功能说明：判断上下是否丢线
*  参数说明：    imageInput  ：二进制图像数组
*             imageOut    ：边线数组[0]:上  [1]：下
*             lineIndex   ：指定行
*  函数返回：0：没有丢线   1:上边丢线  2：下边丢线  3： 上下都丢线   4：错误
*  修改时间：2022年6月24日
*  备    注：guo
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t UD_Road_Is_No_Side(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[2][LCDW], uint8_t lineIndex) {
  uint8_t i;
  uint8_t state = 0;          //状态
  static uint8_t last1 = 30;  //开始点

  //中间线赋值上下顶点
  imageOut[0][lineIndex] = 0;
  imageOut[1][lineIndex] = LCDH - 1;  // 59
  /* 30-2 */
  for (i = last1; i > 1; i--) {
    if (!imageInput[i][lineIndex]) {
      imageOut[0][lineIndex] = i;
      break;
    }
  }
  if (i == 1) /* 上边界丢线 */
    state = 1;

  /* 30-59 */
  for (i = last1; i < LCDH; i++) {
    if (!imageInput[i][lineIndex])  //边线为黑色 黑色为0
    {
      imageOut[1][lineIndex] = i;
      break;
    }
  }

  if (i == LCDH) {
    /* 上下边界都丢线 */
    if (state == 1) state = 3;
    /* 下边界丢线 */
    else
      state = 2;
  }
  if (imageOut[1][lineIndex] <= imageOut[0][lineIndex]) {
    state = 4;
  }
  return state;
}

/*!
 * @brief    丢线处理
 *
 * @param    imageInput ： 二值图像信息
 * @param    imageOut   ： 边线数组
 * @param    mode       ： 那边丢线？   1：左边丢线  2：右边丢线
 * @param    lineIndex  ： 丢线行数
 *
 * @return
 *
 * @note
 *
 * @see
 *
 * @date     2020/6/24 星期三
 */
void RoadNoSideProcess(uint8_t imageInput[2][LCDW], uint8_t imageOut[LCDH][2], uint8_t mode, uint8_t lineIndex) {
  uint8_t i = 0;  //行补列
  switch (mode) {
    case 1:  //右弯
      for (i = lineIndex; i > 0; i--) {
        imageOut[i][0] = imageInput[0][47 + i];
      }
      break;
    case 2:  //左弯
      for (i = lineIndex; i > 0; i--) {
        imageOut[i][1] = imageInput[0][47 - i];  //输入为上边线用上边线给左右边线补线
      }
      break;
  }
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  函数名称：ImageGetSide
*  功能说明：获取左右边线
*  参数说明:            imageInput  ：边缘图像数组
*                     imageOut    ：左右边线数组
*  函数返回：0
*  修改时间：2022/6/23 星期四
*  备   注：不要改
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t ImageGetSide(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[LCDH][2]) {
  uint8_t i = 0, j = 0, L_arry = 10, R_arry = 10;
  // right_turn_flag = 0, left_turn_flag = 0
  //  right_turn_flag = 0;
  //  left_turn_flag = 0;
  num_left_lost = 0;   //左侧丢线个数清零
  num_right_lost = 0;  //右侧丢线个数清零
  num_all_lost = 0;    //左右都丢线清零
  Aver_right_lost = 0;
  Aver_left_lost = 0;
  RoadIsNoSide(imageInput, imageOut, ROAD_START_ROW);
  g_ucFlagW = 0;
  /* 离车头近的行 寻找边线 59-5 */
  for (i = ROAD_START_ROW - 1; i > ROAD_END_ROW; i--) {
    imageOut[i][0] = 0;
    imageOut[i][1] = 93;

    /* 根据边界连续特性 寻找边界 */
    for (j = imageOut[i + 1][0] + L_arry; j > 0; j--) {  //左边线
      if (j >= 93) j = 93;
      if (j < 0) j = 0;
      if (!imageInput[i][j])  //黑色是0 为边线
      {
        imageOut[i][0] = j;
        break;
      } else
        imageOut[i][0] = 0;
    }
    /* 右边线 */
    for (j = imageOut[i + 1][1] - R_arry; j < 93; j++)  //以边线的5个像素为范围查找边线
    {
      if (j >= 93) j = 93;
      if (j < 0) j = 0;
      if (!imageInput[i][j]) {
        imageOut[i][1] = j;
        break;
      } else
        imageOut[i][1] = 93;
    }

    if (imageOut[i][1] < imageOut[i][0]) {  //边界矫正防止左右边界对换
      uint8_t temp = imageOut[i][1];
      imageOut[i][1] = imageOut[i][0];
      imageOut[i][0] = temp;
    }
    // if (abs(imageOut[i][0] - imageOut[i][1]) < 5) {  //防止边界重合
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

    /* 如果左边界 即将超出中线 则检查是否右丢线 */ /* 右转弯 */
    if (/* g_ucFlagRoundabout != FlagRoundabout_R_2 && g_ucFlagRoundabout != FlagRoundabout_R_4 && g_ucFlagRoundabout != FlagRoundabout_L_2 && */ imageOut[i][0] > (LCDW / 2 - 5) && imageOut[i][1] > (LCDW - 5)) {
      /* 右丢线处理 */
      // RoadNoSideProcess(UpdowmSide, imageOut, 1, i);
      g_ucFlagW = 1;
      L_arry = 5;
      R_arry = 0;  //取消查询右边界
      // if (num_left_lost) Aver_left_lost /= num_left_lost;  //返回前丢线行数求平均
      // if (num_right_lost) Aver_right_lost /= num_right_lost;
      // return 1;
    }

    /* 如果右边界 即将超出中线 则检查是否左丢线 */ /* 左转弯 */
    if (/* g_ucFlagRoundabout != FlagRoundabout_R_2 && g_ucFlagRoundabout != FlagRoundabout_L_2 && g_ucFlagRoundabout != FlagRoundabout_R_4 && */ imageOut[i][1] < (LCDW / 2 + 5) && imageOut[i][0] < 5) {
      /* 左丢线处理 */
      // RoadNoSideProcess(UpdowmSide, imageOut, 2, i);
      g_ucFlagW = 2;
      R_arry = 5;
      L_arry = 0;  //取消查询左边界
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
*  函数名称：UpdownSideGet
*  功能说明：获取上边线
*  参数说明:            imageInput  ：边缘图像数组
*                     imageOut    ：上下边线数组
*  函数返回：0
*  修改时间：2022/6/23 星期四
*  备   注：不要改
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t UpdownSideGet(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[2][LCDW]) {
  uint8_t i = 0, j = 0;
  uint8_t last = 55;

  imageOut[0][47] = 0;
  imageOut[1][47] = 59;
  memset(imageOut, 0, sizeof(imageOut));
  /* 用中线比较近的行 判断是否丢线 */
  for (i = last; i >= 0; i--) {  //从last行开始沿着中线向上找到第一个边界
    if (!imageInput[i][47]) {
      imageOut[0][47] = i;
      break;
    }
  }
  if (i < 0) {
    return 0;
  }
  /* 中线往左寻找边线 */
  for (i = 47 - 1; i > 0; i--) {
    imageOut[0][i] = 0;
    imageOut[1][i] = 59;

    /* 根据边界连续特性 寻找边界 */
    for (j = imageOut[0][i + 1] + 5; j > 0; j--) {
      if (!imageInput[j][i]) {
        imageOut[0][i] = j;
        break;
      }
    }
  }
  /*中线往右 寻找边线*/
  for (i = 47 + 1; i < 93; i++) {
    imageOut[0][i] = 0;
    imageOut[1][i] = 59;

    /* 根据边界连续特性 寻找边界 */
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
*  函数名称：TurnNumSideGet
*  功能说明：获取边线的转折点个数
*  参数说明:无
*  函数返回：0
*  修改时间：2022/6/23 星期四
*  备   注：不要改你妈比
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
  //计算上边线有多少个折点 10-80
  for (i = 10; i < 80; i++) {
    err_front = UpdowmSide[0][i - 4] - UpdowmSide[0][i];  //左边的y-此点的y
    err_front1 = UpdowmSide[0][i - 2] - UpdowmSide[0][i];
    while (UpdowmSide[0][i] == UpdowmSide[0][i + 1]) {
      i++;
    }
    err_back = UpdowmSide[0][i] - UpdowmSide[0][i + 4];  //此点的y-右边的y
    err_back1 = UpdowmSide[0][i] - UpdowmSide[0][i + 2];
    if (err_front * err_back < 0 && err_front1 * err_back1 < 0 && UpdowmSide[0][i] > 5) {
      num_up_turn++;                                        //某个点两边的y都大于他或者小于他
      if (num_up_turn <= 10) up_turn[num_up_turn - 1] = i;  // x左标
    }
    /* ****************   ********************        */
    /*                  *  */  //示意图byzhou
  }
  //计算左边线有多少个折点 从55-20
  for (i = 55; i > 20; i--) {
    err_front = ImageSide[i + 1][0] - ImageSide[i][0];

    while (ImageSide[i][0] == ImageSide[i - 1][0]) {
      i--;
    }
    err_back = ImageSide[i][0] - ImageSide[i - 1][0];
    if (err_front * err_back < 0) {
      num_left_turn++;
      if (num_left_turn <= 10 && i < 40) left_turn[num_left_turn - 1] = i;  // y左标
    }
  }
  //计算右边线有多少个折点 从55-20
  for (i = 55; i > 20; i--) {
    err_front = ImageSide[i + 1][1] - ImageSide[i][1];

    while (ImageSide[i][1] == ImageSide[i - 1][1]) {
      i--;
    }
    err_back = ImageSide[i][1] - ImageSide[i - 1][1];

    if (err_front * err_back < 0) {
      num_right_turn++;
      if (num_right_turn <= 10 && i < 40) right_turn[num_right_turn - 1] = i;  // y左标}
    }
  }
  return 0;
}
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  函数名称：ImagePortFilter
*  功能说明：除单一的噪点
*  参数说明:        imageInput      :边缘二值化数组
*                 imageOut        ：输出数组
*  函数返回：无
*  修改时间：2022/6/23 星期四
*  备   注：不要改
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void ImagePortFilter(uint8_t imageInput[LCDH][LCDW], uint8_t imageOut[LCDH][LCDW]) {
  uint8_t temp = 0;

  for (int i = 1; i < LCDH - 1; i++) {
    for (int j = 1; j < LCDW - 1; j++) {
      temp = imageInput[i - 1][j - 1] + imageInput[i - 1][j] + imageInput[i - 1][j + 1] + imageInput[i][j - 1] + imageInput[i][j] + imageInput[i][j + 1] + imageInput[i + 1][j - 1] + imageInput[i + 1][j] + imageInput[i + 1][j + 1];

      /* 邻域内5个点是边沿 则保留该点 可以调节这里优化滤波效果 */
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
int drv_err;  // 20-30行中值平均值
int drv_err2;
int drv_err3;
void TFT_Show_Camera_Info(void) {
  //按下K2锁死屏幕画面方便观察图像
  //    if(KEY_Read(KEY2)==0)
  //    {while(!KEY_Read(KEY2));
  //        while(1){
  //            if(KEY_Read(KEY2) == 0)     //再按K2解锁屏幕
  //            {
  //                while(!KEY_Read(KEY2));
  //                break;
  //            }
  //        }
  //    }
  //
  if (KEY_Read(KEY2) == 0) {
    TFTSPI_Fill_Area(0, 0, 93, 59, u16BLACK);  // 清屏（黑色）
    TFTSPI_BinRoadSide(ImageSide);             //左右边线
    TFTSPI_BinRoad_Leftturn(left_turn);        //画转折点
    TFTSPI_BinRoad_Rightturn(right_turn);
    TFTSPI_Draw_Line(0, ROAD_MAIN_ROW, 94, ROAD_MAIN_ROW, u16BLUE);  //启跑行
    TFTSPI_Draw_Line(0, CURVE_ROW, 94, CURVE_ROW, u16RED);           //弯道控制行
  } else if (KEY_Read(KEY1) == 0)                                    //按下K0 显示下面图像
  {
    TFTSPI_Fill_Area(0, 0, 93, 59, u16BLACK);  // 清屏（黑色）
    TFTSPI_BinRoad_UpdownSide(UpdowmSide);     //上下边线                   //        TFTSPI_Draw_Line(0, ROAD_MAIN_ROW, 93, ROAD_MAIN_ROW, u16RED);  //领跑行显示
    TFTSPI_BinRoad_Upturn(up_turn);            //        TFTSPI_Draw_Line(47, 0, 47, 60, u16RED);                        //显示中线
  } else if (KEY_Read(KEY0) == 0)              //按下K0 显示下面图像
  {
    // TFTSPI_Road(0, 0, LCDH, LCDW, (uint8_t *)Image_Use);  //原图像显示     与边缘图像同时只可开启一个
    TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *)Edge_arr);  //显示边缘提取图像
                                                                  //        TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *) Bin_Image);  //显示边缘提取图像
    TFTSPI_BinRoadSide(ImageSide);                                //左右边线
    TFTSPI_BinRoad_UpdownSide(UpdowmSide);                        //上下边线
                                                                  //        TFTSPI_Draw_Line(0, ROAD_MAIN_ROW, 93, ROAD_MAIN_ROW, u16RED);  //领跑行显示
                                                                  //        TFTSPI_Draw_Line(47, 0, 47, 60, u16RED);                        //显示中线
  } else                                                          //不按下K0则只显示 左 右 上 边线
  {
    // TFTSPI_Fill_Area(0, 0, 93, 59, u16BLACK);   // 清屏（黑色）
    TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *)Bin_Image);  //显示边缘提取图像
    TFTSPI_BinRoadSide(ImageSide);                                 //左右边线
    TFTSPI_BinRoad_UpdownSide(UpdowmSide);                         //上下边线
  }
  char txt[32];
  // V = ECPULSE1 * 100 * 48 * 21 / (105 * 512);  //计算速度（没有卵用且计算值不太对，如果车调好了可以修改一下添加到显示中）
  sprintf(txt, "W:%02d", g_ucFlagW);  //直道长度
  TFTSPI_P8X16Str(12, 0, txt, u16YELLOW, u16RED);

  sprintf(txt, "L:%02d", straightline);  //直道长度
  TFTSPI_P8X16Str(12, 1, txt, u16YELLOW, u16RED);

  sprintf(txt, "Q%03d", drv_err / 10);  // 20行到30行的平均误差值，用于调整舵机控制的比例
  TFTSPI_P8X16Str(12, 2, txt, u16YELLOW, u16RED);

  sprintf(txt, "%03d", (uint16_t)Target_Speed);  //舵机控制比例
  TFTSPI_P8X16Str(12, 3, txt, u16YELLOW, u16RED);

  sprintf(txt, "L:%02d R:%02d A:%02d", num_left_lost, num_right_lost, num_all_lost);  //左、右、全 丢线情况
  TFTSPI_P8X16Str(0, 4, txt, u16YELLOW, u16RED);

  sprintf(txt, "L:%02d U:%02d R:%02d", num_left_turn, num_up_turn, num_right_turn);  //左、上、右 折点数
  TFTSPI_P8X16Str(0, 5, txt, u16YELLOW, u16RED);

  sprintf(txt, "S%6dB%02dLD%02d", RAllPulse, num_black_white, Aver_left_lost);  //编码器里程， 黑白相间个数（黑白相间个数用于判断斑马线）
  TFTSPI_P8X16Str(0, 6, txt, u16YELLOW, u16RED);

  sprintf(txt, "err:%04dFps:%02d", g_sSteeringError, Fps);  //误差值
  TFTSPI_P8X16Str(0, 7, txt, u16YELLOW, u16RED);

  sprintf(txt, "ServoP:%03dRD%02d", Servo_Center_Mid - ServoDuty, Aver_right_lost);  //舵机PWM
  TFTSPI_P8X16Str(0, 8, txt, u16YELLOW, u16RED);

  sprintf(txt, "R%01dT%01dY%01dB%01dZ%01dPD%02d%02d", g_ucFlagRoundabout, g_ucFlagT, g_ucFlagFork, count, g_ucFlagZebra, (int)Servo_P, (int)Servo_D);  //元素标志 环岛 T形口 Y形口 斑马线 （斑马线从1开始）
  TFTSPI_P8X16Str(0, 9, txt, u16YELLOW, u16RED);
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
*  函数名称：void CameraCar(void)
*  功能说明：摄像头整车循迹
*  参数说明：无
*  函数返回：无
*  修改时间：2022年6月30日
*  备   注：摄像头整车程序，包括出库，循迹，元素识别，元素处理，入库。
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void CameraCar(void) {
  LED_Ctrl(LED1, RVS);  // LED闪烁 指示程序运行状态
  drv_err = 0;          // 20-30
  drv_err2 = 0;         // 10-20
  drv_err3 = 0;         // 40-50
  leftdown_turn_flag = find_leftdown_guaidian();
  rightdown_turn_flag = find_rightdown_guaidian();
  StraightLine(UpdowmSide, 3);
  /* 弯道速度与PID决策 */
  //计算领跑行前10（20-30）行的曲率，以此来改变舵机的P 0为中间 -70< drv_err/10 <70
  for (uint8_t i = 20; i < 30; i++) drv_err += (ImageSide[i][0] + ImageSide[i][1] - LCDW);   // 20-29行误差累计值
  for (uint8_t i = 10; i < 20; i++) drv_err2 += (ImageSide[i][0] + ImageSide[i][1] - LCDW);  // 20-29行误差累计值
  /* 弯道速度与PID决策 */
  //直道或弯道速度与pid
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
  //直道或弯道速度与pid
  ////////////////////////////////////////////////////////出库 不出库不执行后面的元素判定 （拨码开关波到上方为左转出库 拨到下方为右转出库）//////////////////////////////////////////////////////////////////////////////////////////////////////
  if (game == no_start)  //出库（出库时 速度、角度、编码器累计值 三者需要搭配好）
  {
    Target_Speed = chuku_speed;                        //出库速度  （一般不做修改）
    if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 0) {  //右转出库
      ServoDuty = chukujiao;
      if (right_chuku) {  //出库完成
        game = start;
        count++;
        RAllPulse = 0;
      }
    } else if (KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 1) {  //左转出库
      ServoDuty = -chukujiao;
      if (left_chuku) {  //出库完成
        game = start;
        count++;
        RAllPulse = 0;
      }
    } else if (KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 0) {  //向上
      game = start;
      count++;
      RAllPulse = 0;
    } else if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 1) {  //向下
      game = start;
      count = 2;
      RAllPulse = 0;
    }                                         /* else if(KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 0){//向下调试
                                                game = start;
                                                RAllPulse = 0;
                                           } */
    if (ServoDuty > 80) ServoDuty = 80;       //偏差限幅 0.766
    if (ServoDuty < -80) ServoDuty = -80;     //偏差限幅
    ServoCtrl(Servo_Center_Mid - ServoDuty);  //舵机打角
    return;
  }

  // 元素处理程序从此处开始 先从环岛开始 T字 岔路口 斑马线依次一个函数一个函数的追
  ///////////////////////////////////////////////////开始循迹////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (game == start) {
    /**************************************斑马线***********************************************************************************************************************/

    // if (g_ucFlagRoundabout == 0 && g_ucFlagCross == 0 && g_ucFlagZebra == 0 && g_ucFlagFork == 0) {
    //   //检测斑马线
    //   if (RoadIsZebra(ImageSide, &g_ucFlagZebra) == 1) {  //左车库
    //     g_ucFlagZebra = 1;
    //     RAllPulse = 0;
    //     g_ucFlagRoundabout = NO;
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
    //   }
    //   if (RoadIsZebra(ImageSide, &g_ucFlagZebra) == 2) {  //右车库
    //     g_ucFlagZebra = 3;
    //     RAllPulse = 0;
    //     g_ucFlagRoundabout = NO;
    //     PIN_InitConfig(Beep, PIN_MODE_OUTPUT, 1);
    //   }
    // }
    // if (g_ucFlagZebra) {
    //   //斑马线处理（第一次左或者右补线，与出库方向决定左补线还是右补线，第二次则入库）
    //   ZebraProcess(ImageSide, &g_ucFlagZebra);
    // }

    /********************************环岛**********************************************************************************************************************************/
    // if (g_ucFlagW == 0 && g_ucFlagRoundabout == NO && g_ucFlagFork == Y_NO && g_ucFlagZebra == 0 && g_ucFlagT == 0) {
    //   // 检测环岛
    //   RoadIsRoundabout(UpdowmSide, Bin_Image, ImageSide);
    // }
    // if (g_ucFlagRoundabout != NO) {
    //   //环岛处理，分为左环岛和右环岛 检测T型口 （环岛与T型口一开始十分相像且一开始的处理方式也一直，所以放在一起检测，度过第一阶段后再判断是环岛还是T字）
    //   RoundaboutProcess(Bin_Image, ImageSide, UpdowmSide);
    // }
    // if (g_ucFlagT) {
    //   // T字处理，分为左T字和右T字（左T字：整个T字一直在左转 右T字：整个T字一直在右转）
    //   TProcess(Bin_Image, UpdowmSide, ImageSide, &g_ucFlagT);
    // }

    /*********************************Y形路口************************************************************************************************************************/
    // if (/* g_ucFlagRoundabout == FlagRoundabout_L_1 || g_ucFlagRoundabout == FlagRoundabout_R_1 || */ g_ucFlagRoundabout == NO /* || FlagRoundabout_R == g_ucFlagRoundabout || FlagRoundabout_L == g_ucFlagRoundabout) */ && g_ucFlagFork == Y_NO && g_ucFlagZebra == 0 && g_ucFlagT == 0) {
    //   RoadIsFork(UpdowmSide, ImageSide);  //环岛第一阶段也判断y字因为某些时侯y字会和环岛误识别
    // }
    // if (g_ucFlagFork != Y_NO) {
    //   //  Y型口处理，这里不分左右（因为是往返赛道，则都为左转或右转即可，这里选择的是都为左转）
    //   ForkProcess(ImageSide);
    // }

    //环岛第一阶段和出环岛单边丢线处理
    if (ImageSide[Round_ROW][1] == 93 && (g_ucFlagRoundabout == FlagRoundabout_R_1 || g_ucFlagRoundabout == FlagRoundabout_R)) {
      g_sSteeringError = 2 * ImageSide[Round_ROW][0] + Round_WIDTH - LCDW;
      AKMQZ = (60 - Round_ROW) * AKMQZ_xishu;
    } else if (ImageSide[Round_ROW][0] == 0 && (g_ucFlagRoundabout == FlagRoundabout_L_1 || g_ucFlagRoundabout == FlagRoundabout_L)) {
      g_sSteeringError = 2 * ImageSide[Round_ROW][1] - Round_WIDTH - LCDW;
      AKMQZ = (60 - Round_ROW) * AKMQZ_xishu;
    }
    //环岛第一阶段和出环岛单边丢线处理

    //出环岛单边丢线处理
    else if (g_ucFlagRoundabout == FlagRoundabout_R_5) {
      g_sSteeringError = 2 * ImageSide[Round_ROW][0] + Round_WIDTH - LCDW;
      AKMQZ = (60 - Round_ROW) * AKMQZ_xishu;
    } else if (g_ucFlagRoundabout == FlagRoundabout_L_5) {
      g_sSteeringError = 2 * ImageSide[Round_ROW][1] - Round_WIDTH - LCDW;
      AKMQZ = (60 - Round_ROW) * AKMQZ_xishu;
    }
    //出环岛单边丢线处理

    //环中补线处理
    else if (/* ImageSide[Round_ROW][1] == 93 && */ g_ucFlagRoundabout == FlagRoundabout_R_3) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_MID][0] + Round_WIDTH_MID - LCDW;
      AKMQZ = (60 - Round_ROW_MID) * AKMQZ_xishu;
    } else if (/* ImageSide[Round_ROW][0] == 0 &&  */ g_ucFlagRoundabout == FlagRoundabout_L_3) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_MID][1] - Round_WIDTH_MID - LCDW;
      AKMQZ = (60 - Round_ROW_MID) * AKMQZ_xishu;
    }
    //环中补线处理

    //进环岛处理
    else if (g_ucFlagRoundabout == FlagRoundabout_R_2) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_JIN][1] - Round_WIDTH_JIN - LCDW;
      AKMQZ = (60 - Round_ROW_JIN) * AKMQZ_xishu;
    } else if (g_ucFlagRoundabout == FlagRoundabout_L_2) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_JIN][0] + Round_WIDTH_JIN - LCDW;
      AKMQZ = (60 - Round_ROW_JIN) * AKMQZ_xishu;
    }
    //进环岛处理

    //出环岛处理
    else if (g_ucFlagRoundabout == FlagRoundabout_R_4) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_CHU][1] - Round_WIDTH_CHU - LCDW;
      AKMQZ = (60 - Round_ROW_CHU) * AKMQZ_xishu;
    } else if (g_ucFlagRoundabout == FlagRoundabout_L_4) {
      g_sSteeringError = 2 * ImageSide[Round_ROW_CHU][0] + Round_WIDTH_CHU - LCDW;
      AKMQZ = (60 - Round_ROW_CHU) * AKMQZ_xishu;
    }
    //出环岛处理

    // y字处理
    else if (g_ucFlagFork == FlagFork_0 || g_ucFlagFork == FlagFork_2) {
      g_sSteeringError = 2 * ImageSide[Y_ROW][0] + Y_WIDTH - LCDW;
      AKMQZ = (60 - Y_ROW) * AKMQZ_xishu;
    }
    // y字处理

    /* t字中 */
    else if (g_ucFlagT == 1) {  //右t
      g_sSteeringError = 2 * ImageSide[T_ROW_MID][0] + T_WIDTH_MId - LCDW;
      AKMQZ = (60 - T_ROW_MID) * AKMQZ_xishu;
    } else if (g_ucFlagT == 3) {  //左t
      g_sSteeringError = 2 * ImageSide[T_ROW_MID][1] - T_WIDTH_MId - LCDW;
      AKMQZ = (60 - T_ROW_MID) * AKMQZ_xishu;
    }
    /* t字中 */

    //出t处理
    else if (g_ucFlagT == 4) {
      g_sSteeringError = 2 * ImageSide[T_ROW][1] - T_WIDTH - LCDW;
      AKMQZ = (60 - T_ROW) * AKMQZ_xishu;
    } else if (g_ucFlagT == 2) {
      g_sSteeringError = 2 * ImageSide[T_ROW][0] + T_WIDTH - LCDW;
      AKMQZ = (60 - T_ROW) * AKMQZ_xishu;
    }
    //出t处理
#ifdef cheku_1                                                                  //防跑出车库算法
    else if (g_ucFlagZebra == 1) {                                              //左车库
      if (ImageSide[ROAD_MAIN_ROW][1] > 47 && ImageSide[ROAD_MAIN_ROW][0] < 5)  //如果右边线有效左边线失效就跟踪右边线
      {
        g_sSteeringError = 2 * ImageSide[ROAD_MAIN_ROW][1] - ROAD_MAIN_ROW - LCDW;
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else if (ImageSide[ROAD_MAIN_ROW][1] > 47 && ImageSide[ROAD_MAIN_ROW][0] > 5)  //都有效正常循迹
      {
        g_sSteeringError = RoadGetSteeringError(ImageSide, ROAD_MAIN_ROW);
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else {
        g_sSteeringError = 0;
      }

    } else if (g_ucFlagZebra == 3) {                                                 //右车库
      if (ImageSide[ROAD_MAIN_ROW][0] < 47 && ImageSide[ROAD_MAIN_ROW][1] > 93 - 5)  //如果zuo边线有效右边线失效就跟踪左边线
      {
        g_sSteeringError = 2 * ImageSide[ROAD_MAIN_ROW][0] + ROAD_MAIN_ROW - LCDW;
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else if (ImageSide[ROAD_MAIN_ROW][0] < 47 && ImageSide[ROAD_MAIN_ROW][1] < 93 - 5)  //都有效正常循迹
      {
        g_sSteeringError = RoadGetSteeringError(ImageSide, ROAD_MAIN_ROW);
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else {
        g_sSteeringError = 0;
      }
    }
#else
    //车库处理车库锁死
    else if (g_ucFlagZebra == 1) {  //左
      g_sSteeringError = 0;
    } else if (g_ucFlagZebra == 3) {  //右
      g_sSteeringError = 0;
    }
//车库处理
#endif
    //车库处理

    //弯道打角处理
    else if (g_ucFlagW) {
      if (ImageSide[CURVE_ROW][0] == 0) {
        g_sSteeringError = 2 * ImageSide[CURVE_ROW][1] - CURVE_WIDTH - LCDW;  // 47为第30行的赛道宽度
        AKMQZ = (60 - CURVE_ROW) * AKMQZ_xishu;
      } else if (ImageSide[CURVE_ROW][1] == 93) {
        g_sSteeringError = 2 * ImageSide[CURVE_ROW][0] + CURVE_WIDTH - LCDW;
        AKMQZ = (60 - CURVE_ROW) * AKMQZ_xishu;
      } else {
        g_sSteeringError = RoadGetSteeringError(ImageSide, CURVE_ROW);
        AKMQZ = (60 - CURVE_ROW) * AKMQZ_xishu;
      }
    }
    //弯道打角处理

    //正常循迹
    else {
      if (ImageSide[ROAD_MAIN_ROW][0] == 0) {
        g_sSteeringError = 2 * ImageSide[ROAD_MAIN_ROW][1] - ROAD_MAIN_WIDTH - LCDW;  // 47为第30行的赛道宽度
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else if (ImageSide[ROAD_MAIN_ROW][1] == 93) {
        g_sSteeringError = 2 * ImageSide[ROAD_MAIN_ROW][0] + ROAD_MAIN_WIDTH - LCDW;
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      } else {
        g_sSteeringError = RoadGetSteeringError(ImageSide, ROAD_MAIN_ROW);
        AKMQZ = (60 - ROAD_MAIN_ROW) * AKMQZ_xishu;
      }
    }
    //正常循迹
    /* 十字处理 */
    if (ImageSide[Round_ROW][1] >= 93 - 5 && ImageSide[Round_ROW][0] <= 5) {
      g_sSteeringError = 0;
    }
    /* 十字处理 */
    /* 通用速度决策 */
    if (straightline < despeed_threshold) {  //防跑出
      Target_Speed = initspeed - (60 - straightline) * despeed;
    }
    /* 通用速度决策 */
  }

  ////////////////////////////////////////////////////////////入库结束/////////////////////////////////////////////////////////////////////////////////////////////
  if (game == over)  //入库（同出库一样）
  {
    Target_Speed = jinku_speed;  //出库速度  （一般不做修改）
    Servo_P = Servo_P_jinku;     //出库
    Servo_D = Servo_D_jinku;
    if (RAllPulse > 3000) {
      Target_Speed = 0;
    }
    // if (Target_Speed != 0&&straightline < 20) {  //防跑出
    //   Target_Speed = jinku_speed - (60 - straightline) * 8;
    // }

    if (KEY_Read(DSW0) == 0 && KEY_Read(DSW1) == 1) {  //左转入库
      ServoDuty = -jinkujiao;
    } else if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 0) {  //右转入库
      ServoDuty = jinkujiao;
    } else if (KEY_Read(DSW0) == 1 && KEY_Read(DSW1) == 1) {
      if (g_ucFlagZebra == 3) ServoDuty = jinkujiao;   // you转
      if (g_ucFlagZebra == 1) ServoDuty = -jinkujiao;  // zuo转入库
    }
    if (ServoDuty > 80) ServoDuty = 80;       //偏差限幅 0.766
    if (ServoDuty < -80) ServoDuty = -80;     //偏差限幅
    ServoCtrl(Servo_Center_Mid - ServoDuty);  //舵机打角
    return;
  }


  
  Time = STM_GetNowUs(STM0);
  if (LastTime == 0) {
    ServoDuty = (g_sSteeringError * Servo_P + (g_sSteeringError - g_sSteeringError_last) * Servo_D / 100000000) / 10;  //偏差放大
    Fps = 0;
  } else {
    Fps = 1.0 / (Time - LastTime) * 1000000;
    ServoDuty = (g_sSteeringError * Servo_P + ((g_sSteeringError - g_sSteeringError_last) / ((1 / Fps) * 50)) * Servo_D / 100000000) / 10;  //偏差放大
  }
  LastTime = Time;
  g_sSteeringError_last = g_sSteeringError;
  if (ServoDuty > 80) ServoDuty = 80;       //偏差限幅 0.766
  if (ServoDuty < -80) ServoDuty = -80;     //偏差限幅
  ServoCtrl(Servo_Center_Mid - ServoDuty);  //舵机打角
}
/*********************************************************************************************************************************/
/*********************************************************************************************************************************/
//      //十字部分未用到
//     if(g_ucFlagRoundabout == 0 && g_ucFlagCross == 0 && g_ucFlagFork == 0)
//     {
//         /* 检测十字 */
//         RoadIsCross(ImageSide, &g_ucFlagCross);
//     }
//     if(g_ucFlagCross)
//     {
//         /* 十字处理 */
//         CrossProcess(Image_Use, ImageSide, &g_ucFlagCross);
//     }
