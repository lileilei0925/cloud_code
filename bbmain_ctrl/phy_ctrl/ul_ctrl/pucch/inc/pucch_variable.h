#pragma once
#include "phyctrl_pucch.h"

#define   FUNC_BIT_2_WORD(x)   (((x) + 31) >> 5)     

uint32_t g_NghNvNcsBuff[6];  /*组跳频，序列跳，循环移位扰码计算相关buff,Ngh/Nv最多使用一个DW,Ncs最多使用 (((8*14sym)>>5向上取整)+1=6DW*/

uint8_t g_NuValue[HOP_NUM];                               //2hop
uint8_t g_NvValue[HOP_NUM];                               //2hop
uint8_t g_NcsValue[SYM_NUM_PER_SLOT];                     //14symbol
uint8_t g_part1ReNum[SYM_NUM_PER_SLOT];                   //14symbol

uint32_t g_fmt23dataScrambuff[144];                       //fmt23数据符号扰码生成缓存
uint32_t g_fmt2pilotScrambuff[2][5];                      //fmt2导频符号扰码生成缓存

float maxCodeRateTab[8] = {0.08, 0.15, 0.25, 0.35, 0.45, 0.60, 0.80, 0.80};

PucchPara    g_PucchPara[MAX_CELL_NUM];                      /* Pucch armtodsp参数本地buffer */

ArmPucParam  g_armPucParam       = {0};                      /* Pucch 参数本地buffer */ 

RMDecodeHacCfgPara  g_rmDecodeHacCfgPara[MAX_CELL_NUM];      /* RM译码参数配置 */ 

PolarDecodeHacCfgPara g_polarDecodeHacCfgPara[MAX_CELL_NUM]; /* Polar译码参数配置 */ 

