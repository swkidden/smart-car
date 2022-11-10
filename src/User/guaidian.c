#include "guaidian.h"

uint8_t find_rightdown_guaidian() {
  uint8_t count = 0, i = 0, j = 0;
  for (i = 55; i > 0; i--) {
    if (ImageSide[i][1] > 30 && ImageSide[i - 5][1] > 93 - 5 && ImageSide[i - 1][1] - ImageSide[i][1] > 3) {  //出现跳变点
                                                                                                              // d_debug("出现右侧跳变点X:%dY:%d\n", ImageSide[i][1], i);
      for (j = 1; j < 4; j++) {
        if (abs(ImageSide[i + 4][1] - ImageSide[i][1]) > 6) break;     //判断点下4行是一条连续的直线 上4行是跳变的直线
        if (abs(ImageSide[i + j][1] - ImageSide[i + j + 1][1]) > 1) {  //下4行不是连续的直线                                                                                                        //    d_debug("不是连续的直线X:%dY:%d\n", ImageSide[i + j][1], i);
          break;
        }
        if (ImageSide[i - j-5][1] < 93 - 5) {  //上四行不是丢线的直线
                                             // d_debug("不是丢线的直线X:%dY:%d\n", ImageSide[i - j][1], i);
          break;
        }
      }
      if (j == 4) {  //代表上面没break代表符合要求 进一步判断点的右边是否存在直线
        count = 0;
        // d_debug("找到右侧跳变点X:%dY:%d\n", ImageSide[i][1], i);
        for (uint8_t j = ImageSide[i][1]; j <= 93; j++) {
          for (uint8_t k = i - 1; k <= i + 1; k++) {
            if (k > 59) {
              break;
            }
            if (!Bin_Image[k][j]) {
              count++;
              if (count > 10) {  //找到拐点
                rightdown_x = ImageSide[i][1];
                rightdown_y = i;
                return 1;
              }
            }
          }
        }
      }
    }
  }
  rightdown_x = 0;
  rightdown_y = 0;

  return 0;
}
uint8_t find_leftdown_guaidian() {
  uint8_t count = 0, i = 0, j = 0;
  for (i = 55; i > 0; i--) {
    if (ImageSide[i][0] < 64 && ImageSide[i - 5][0] < 5 && ImageSide[i][0] - ImageSide[i - 1][0] > 3) {  //出现跳变点
      // d_debug("出现左侧跳变点X:%dY:%d\n", ImageSide[i][0], i);
      for (j = 1; j < 4; j++) {
        if (abs(ImageSide[i + 4][0] - ImageSide[i][0]) > 6) break;     //判断点下4行是一条连续的直线 上4行是跳变的直线
        if (abs(ImageSide[i + j][0] - ImageSide[i + j + 1][0]) > 1) {  //下4行不是连续的直线
          // d_debug("不是连续的直线X:%dY:%d\n", ImageSide[i+j][0], i);
          break;
        }
        if (ImageSide[i - j-5][0] > 5) {  //上四行不是丢线的直线
          // d_debug("不是丢线的直线X:%dY:%d\n", ImageSide[i-j][0], i);
          break;
        }
      }
      if (j == 4) {  //代表上面没break代表符合要求 进一步判断点的左边是否存在直线
        count = 0;
        // d_debug("找到左侧跳变点X:%dY:%d\n", ImageSide[i][0], i);
        for (uint8_t j = ImageSide[i][0]; j >= 0; j--) {
          for (uint8_t k = i - 1; k <= i + 1; k++) {
            if (k > 59) {
              break;
            }
            if (!Bin_Image[k][j]) {
              count++;
              if (count > 10) {  //找到拐点
                leftdown_x = ImageSide[i][0];
                leftdown_y = i;
                return 1;
              }
            }
          }
        }
      }
    }
  }
  leftdown_x = 0;
  leftdown_y = 0;
  return 0;
}
uint8_t find_xianhuan_L() {
  //  d_debug("find——xianhuan");
  for (uint8_t i = 55; i >= 20; i--) {
    if ((ImageSide[i][0] - ImageSide[i - 1][0]) >= 0 && (ImageSide[i][0] - ImageSide[i - 2][0]) >= 0 && (ImageSide[i][0] - ImageSide[i - 3][0]) >= 0 && (ImageSide[i][0] - ImageSide[i + 1][0]) >= 0 && Bin_Image[i - 1][ImageSide[i][0]] == 1 && Bin_Image[i + 1][ImageSide[i][0]] == 1 &&
        ImageSide[i - 2][0] > 1) {
      xianhuan_L = i;  //找到左线环切点
                       //    d_debug("find:%d", i);
      return 1;
    }
  }
  xianhuan_L = 0;  // qing 0
  return 0;
}
uint8_t find_xianhuan_R() {
  // d_debug("find——xianhuan");
  for (uint8_t i = 55; i >= 20; i--) {
    if ((ImageSide[i][1] - ImageSide[i - 1][1]) <= 0 && (ImageSide[i][1] - ImageSide[i - 2][1]) <= 0 && (ImageSide[i][1] - ImageSide[i - 3][1]) <= 0 && (ImageSide[i][1] - ImageSide[i + 1][1]) <= 0 && Bin_Image[i - 1][ImageSide[i][1]] == 1 && Bin_Image[i + 1][ImageSide[i][1]] == 1 &&
        ImageSide[i - 2][1] > 1) {
      xianhuan_R = i;  //找到右线环切点
      // d_debug("find:%d", i);
      return 1;
    }
  }
  xianhuan_R = 0;
  return 0;
}

