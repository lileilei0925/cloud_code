#pragma once
#include "phyctrl_pucch.h"

#define   FUNC_BIT_2_WORD(x)   (((x) + 31) >> 5)     

uint32_t g_NghNvNcsBuff[6];  /*组跳频，序列跳，循环移位扰码计算相关buff,Ngh/Nv最多使用一个DW,Ncs最多使用 (((8*14sym)>>5向上取整)+1=6DW*/

uint8_t g_NuValue[HOP_NUM];                               //2hop
uint8_t g_NvValue[HOP_NUM];                               //2hop
uint8_t g_NcsValue[SYM_NUM_PER_SLOT];                     //14symbol
uint8_t g_part1ReNum[SYM_NUM_PER_SLOT];                   //14symbol

uint8_t g_pucchFmt01RptNum[MAX_CELL_NUM][SLOT_NUM_PER_FRAME];//放到DDR
uint8_t g_pucchFmt23RptNum[MAX_CELL_NUM][SLOT_NUM_PER_FRAME];//放到DDR
uint8_t g_pucchFmt01RptIndex[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][MAX_PUCCH_NUM];//放到DDR
uint8_t g_pucchFmt23RptIndex[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][MAX_PUCCH_NUM];//放到DDR

uint8_t g_pucchCsiPart2Flag[MAX_CELL_NUM][SLOT_NUM_PER_FRAME] = {0};

uint32_t g_fmt23dataScrambuff[144];                       //fmt23数据符号扰码生成缓存
uint32_t g_fmt2pilotScrambuff[2][5];                      //fmt2导频符号扰码生成缓存

float maxCodeRateTab[8] = {0.08, 0.15, 0.25, 0.35, 0.45, 0.60, 0.80, 0.80};

PucchPara    g_PucchPara[MAX_CELL_NUM][SLOT_NUM_PER_FRAME];            /* Pucch armtodsp参数本地buffer *///放到DDR

ArmPucParam  g_armPucParam[MAX_CELL_NUM][SLOT_NUM_PER_FRAME] = {0};    /* Pucch 参数本地buffer *///放到DDR

/* PUCCH UCI译码参数 */
PucchPolarDecodeHacCfgPara  g_pucchPolarDecodeHacCfgParaDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][PUCCH_PART_NUM];//放到DDR
PucchRMDecodeHacCfgPara     g_pucchRMDecodeHacCfgParaDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][PUCCH_PART_NUM];//放到DDR

uint16_t g_pucchCsiPart2BitLength[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][MAX_PUCCH_NUM];//保存格式3 CSI Part2比特长度,UE间松排

PucchPolarDecodeHacCfgPara  g_pucchPolarDecodeHacCfgParal2[MAX_CELL_NUM][PUCCH_UCI_PING_PONG_NUM][PUCCH_PART_NUM];//放到L2
PucchRMDecodeHacCfgPara     g_pucchRMDecodeHacCfgParaL2[MAX_CELL_NUM][PUCCH_UCI_PING_PONG_NUM][PUCCH_PART_NUM];//放到L2

/* PUCCH UCI译码输入数据 */
//PUCCH输出规划和如何摆放？待规划

/* PUCCH UCI译码输出 */
PolarDecodeOut       g_pucchPolarDecodeOutDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][PUCCH_PART_NUM];//放到DDR
RMDecodeOut          g_pucchRMDecodeOutDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][PUCCH_PART_NUM];//放到DDR

PolarDecodeOut       g_pucchPolarDecodeOutL2[MAX_CELL_NUM][PUCCH_UCI_PING_PONG_NUM][PUCCH_PART_NUM];//放到L2
RMDecodeOut          g_pucchRMDecodeOutL2[MAX_CELL_NUM][PUCCH_UCI_PING_PONG_NUM][PUCCH_PART_NUM];//放到L2

/* PUCCH UCI译码组包结果 */
PucchFmt01Rst     g_pucchFmt01Rst[MAX_CELL_NUM][SLOT_NUM_PER_FRAME];//放到DDR

PucchFmt23Rst     g_pucchFmt23Rst[MAX_CELL_NUM][SLOT_NUM_PER_FRAME];//放到DDR


TLV_MSG g_pucchTlvMsg[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][PUCCH_PART_NUM];//放到DDR

uint8_t g_pucchRptBuffer[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][PUCCH_PART_NUM][PUCCH_RPT_BUFFER_SIZE];//放到DDR

FSM g_pucchFSM[MAX_CELL_NUM][PUCCH_UCI_PING_PONG_NUM];//放到DDR