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

PucchPara    g_PucchPara[MAX_CELL_NUM];                      /* Pucch armtodsp参数本地buffer *///放到DDR?

ArmPucParam  g_armPucParam       = {0};                      /* Pucch 参数本地buffer */ 

/* PUCCH UCI译码参数 */
PucchPolarDecodeHacCfgPara  g_pucchPolarDecodeHacCfgParaDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][PUCCH_UCI_PART_NUM];//放到DDR
PucchRMDecodeHacCfgPara     g_pucchRMDecodeHacCfgParaDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][PUCCH_UCI_PART_NUM];//放到DDR

PucchPolarDecodeHacCfgPara  g_pucchPolarDecodeHacCfgParal2[MAX_CELL_NUM][PUCCH_UCI_PING_PONG_NUM][PUCCH_UCI_PART_NUM];//放到L2
PucchRMDecodeHacCfgPara     g_pucchRMDecodeHacCfgParaL2[MAX_CELL_NUM][PUCCH_UCI_PING_PONG_NUM][PUCCH_UCI_PART_NUM];//放到L2

/* PUCCH UCI译码输入数据 */
//PUCCH输出规划和如何摆放？待规划

/* PUCCH UCI译码输出 */
PolarDecodeOut       g_pucchPolarDecodeOutDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][PUCCH_UCI_PART_NUM];//放到DDR
RMDecodeOut          g_pucchRMDecodeOutDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][PUCCH_UCI_PART_NUM];//放到DDR

PolarDecodeOut       g_pucchPolarDecodeOutL2[MAX_CELL_NUM][PUCCH_UCI_PING_PONG_NUM][PUCCH_UCI_PART_NUM];//放到L2
RMDecodeOut          g_pucchRMDecodeOutL2[MAX_CELL_NUM][PUCCH_UCI_PING_PONG_NUM][PUCCH_UCI_PART_NUM];//放到L2

/* PUCCH UCI译码组包结果 */
PucchFmt01Rst     g_pucchFmt01Rst[MAX_CELL_NUM][SLOT_NUM_PER_FRAME];//放到DDR

PucchFmt23Rst     g_pucchFmt23Rst[MAX_CELL_NUM][SLOT_NUM_PER_FRAME];//放到DDR

FsmTable g_pucchUciTable[] =
{
    //{当前状态S，                                  trigger事件E，                              handler函数H，                           转移后的状态S}
	{Pucch_Uci_Idle_State,                         Pucch_Slot_Tast_Start_Event,                PucchPart1ParaCfgHandler,               Pucch_Wait_Part1_Result_State},//S0{E0,H0}->S1
	
	{Pucch_Wait_Part1_Result_State,                Pucch_Part1_Result_Trigger_Event,           PucchPart1ParsePart2ParaCfgHandler,     Pucch_Wait_Part2_Result_State},//S1{E1,H1}->S2
	
	{Pucch_Wait_Part2_Result_State,                Pucch_Part2_Result_Trigger_Event,           PucchPart2ParseHandler,                 Pucch_Uci_Packing_State},//S2{E2,H2}->S3
	
    {Pucch_Uci_Packing_State,                      Pucch_UCI_Packing_Over_Event,               PucchUciSendHandler,                    Pucch_Uci_Idle_State},//S3{E3,H3}->S0
};