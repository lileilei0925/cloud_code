#pragma once
#include "phyctrl_pucch.h"

#define   FUNC_BIT_2_WORD(x)   (((x) + 31) >> 5)     

uint32_t g_NghNvNcsBuff[6];  /*组跳频，序列跳，循环移位扰码计算相关buff,Ngh/Nv最多使用一个DW,Ncs最多使用 (((8*14sym)>>5向上取整)+1=6DW*/

uint8_t g_NuValue[HOP_NUM];                               //2hop
uint8_t g_NvValue[HOP_NUM];                               //20slot*2hop
uint8_t g_NcsValue[SYM_NUM_PER_SLOT];                     //14symbol

uint32_t g_fmt23dataScrambuff[144];                          //
uint32_t g_fmt2pilotScrambuff[2][72];                       //


ArmToDspPucParam    g_armtodspPucParam  = {0};                                          /* Pucch 参数本地buffer */

uint8_t g_pucchfmt1pdunum = 0;                                		                    /* 本小区pucch fmt1的PDU个数 */
uint8_t g_pucchfmt023pdunum = 0;                              		                    /* 本小区pucch fmt023的PDU个数 */
FapiNrMsgPucchPduInfo g_FapiPucchfmt1PduInfo[MAX_CELL_NUM][MAX_PUCCH_NUM] = {0};       	    /* 本小区pucch fmt1的PDU信息 */

uint8_t g_pucchNumpersym[SYM_NUM_PER_SLOT] = {0};               		                /* 本小区按符号统计的PUCCH个数 */
uint8_t g_pucchIndex[SYM_NUM_PER_SLOT][MAX_PUCCH_NUM] = {0};    		                /* 本小区按符号统计的PUCCH索引 */

uint8_t g_pucchpduGroupNum = 0;                                                         /* 本小区PUCCH fmt1的组数 */
uint8_t g_pucchpduNumPerGroup[MAX_PUCCH_NUM] = {0};                                     /* 本小区PUCCH fmt1每组PDU个数 */
uint8_t g_pucchpduIndexinGroup[MAX_PUCCH_NUM][MAX_USER_NUM_PER_OCC] = {0};              /* 本小区PUCCH fmt1各组PDU索引值 */