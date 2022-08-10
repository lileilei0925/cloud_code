#pragma once
#include "phyctrl_pucch.h"


ArmToDspPucParam    g_armtodspPucParam               = {0};                         /* Pucch 参数本地buffer */

uint8_t g_pucchfmt1pdunum[MAX_CELL_NUM] = {0};                                		/* 本小区pucch fmt1的PDU个数 */
uint8_t g_pucchfmt023pdunum[MAX_CELL_NUM] = {0};                              		/* 本小区pucch fmt023的PDU个数 */
FapiNrMsgPucchPduInfo g_FapiPucchPduInfo[MAX_CELL_NUM][MAX_PUCCH_NUM] = {0};       	/* 本小区pucch fmt1的PDU信息 */

uint8_t g_pucchNumpersym[MAX_CELL_NUM][SYM_NUM_PER_SLOT] = {0};               		/* 本小区按符号统计的PUCCH个数 */
uint8_t g_pucchIndex[MAX_CELL_NUM][SYM_NUM_PER_SLOT][MAX_PUCCH_NUM] = {0};    		/* 本小区按符号统计的PUCCH索引 */

uint8_t g_pucchpduGroupNum[MAX_CELL_NUM];                                           /* 本小区PUCCH fmt1的组数 */
uint8_t g_pucchpduNumPerGroup[MAX_CELL_NUM][MAX_PUCCH_NUM];                         /* 本小区PUCCH fmt1每组PDU个数 */
uint8_t g_pucchpduIndexinGroup[MAX_CELL_NUM][MAX_PUCCH_NUM][MAX_USER_NUM_PER_OCC];  /* 本小区PUCCH fmt1各组PDU索引值 */