#pragma once
#include "phyctrl_pucch.h"

#define   FUNC_BIT_2_WORD(x)   (((x) + 31) >> 5)     

/* 组跳频，fgh(ns) 根据cell_para_id计算，为PUCCH计算组跳频基序列时共用*/
uint32_t gudNghData[FUNC_BIT_2_WORD(MAX_BIT_FGH)];  /*(8*20*2)>>5=10words*/
/* 序列跳  V_seq = c(2*ns+n_hop), 20->ns max,*/
uint32_t gudNvData[FUNC_BIT_2_WORD(MAX_BIT_V)];     /*(2*20)>>5 = 2words*/
/* PUCCH   n_cs_cell(ns,l)=sum(c(8 * N_UL_symb * ns + 8 * l + i) * 2^i) i = 0,...,7 */
uint32_t gudNcsData[FUNC_BIT_2_WORD(MAX_BIT_NCS)];  /*(8*14*20)>>5=70words*/

uint32_t gudNuValue[SLOT_NUM_PER_FRAME][HOP_NUM];             //20slot*2hop
uint32_t gudNvValue[SLOT_NUM_PER_FRAME][HOP_NUM];             //20slot*2hop
uint32_t gudNcsValue[SLOT_NUM_PER_FRAME*SYM_NUM_PER_SLOT];    //20slot*14symbol


ArmToDspPucParam    g_armtodspPucParam  = {0};                                          /* Pucch 参数本地buffer */

uint8_t g_pucchfmt1pdunum = 0;                                		                    /* 本小区pucch fmt1的PDU个数 */
uint8_t g_pucchfmt023pdunum = 0;                              		                    /* 本小区pucch fmt023的PDU个数 */
FapiNrMsgPucchPduInfo g_FapiPucchfmt1PduInfo[MAX_CELL_NUM][MAX_PUCCH_NUM] = {0};       	    /* 本小区pucch fmt1的PDU信息 */

uint8_t g_pucchNumpersym[SYM_NUM_PER_SLOT] = {0};               		                /* 本小区按符号统计的PUCCH个数 */
uint8_t g_pucchIndex[SYM_NUM_PER_SLOT][MAX_PUCCH_NUM] = {0};    		                /* 本小区按符号统计的PUCCH索引 */

uint8_t g_pucchpduGroupNum = 0;                                                         /* 本小区PUCCH fmt1的组数 */
uint8_t g_pucchpduNumPerGroup[MAX_PUCCH_NUM] = {0};                                     /* 本小区PUCCH fmt1每组PDU个数 */
uint8_t g_pucchpduIndexinGroup[MAX_PUCCH_NUM][MAX_USER_NUM_PER_OCC] = {0};              /* 本小区PUCCH fmt1各组PDU索引值 */