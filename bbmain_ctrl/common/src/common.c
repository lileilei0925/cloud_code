#include <stdio.h>
#include <string.h>
#include "../inc/common_typedef.h"
#include "../inc/common_macro.h"


uint32_t do_brev(uint32_t val_32bit);
void PseudoRandomSeqGen(uint8_t* pucDataOut, uint32_t udCinit, uint32_t udSequenceLen);


/*******************************************************************************
* 函数名称: do_brev
* 函数功能: 完成对一个Uint 32位数bit位反转存储
* 相关文档:
* 函数参数:
* 参数名称:   类型   输入/输出   描述
* 返回值:   无
* 函数类型: <回调、中断、可重入（阻塞、非阻塞）等函数必须说明类型及注意事项>
* 函数说明:（以下述列表的方式说明本函数对全局变量的使用和修改情况，以及本函数
*           未完成或者可能的改动）

*******************************************************************************/
uint32_t do_brev(uint32_t val_32bit)
{
	uint32_t result     = 0;
	uint32_t udLoopIdx  = 0;

	while (udLoopIdx < 32)
	{
		result = (result << 1) | (val_32bit & 0x1);
		val_32bit >>= 1;
		udLoopIdx++;
	}
	return result;
}

/*******************************************************************************
* 函数名称: App_pseudo_random_seq_gen
* 函数功能: 计算c(n)序列
* 相关文档: 3GPP TS 211 计算PN序列
* 函数参数:
* 参数名称:   类型   输入/输出   描述
*
* pucDataOut        UCHAR*   out       c序列的指针
* udCinit           UINT32   in        cinit值
* udSequenceLen     UINT32   in        序列的长度
*
* 返回值:   无
* 函数类型: <回调、中断、可重入（阻塞、非阻塞）等函数必须说明类型及注意事项>
* 函数说明:（以下述列表的方式说明本函数对全局变量的使用和修改情况，以及本函数
*
*******************************************************************************/
void PseudoRandomSeqGen(uint8_t* pucDataOut, uint32_t udCinit, uint32_t udSequenceLen)
{
	uint32_t udLoopIdx = 0;
	uint32_t x1_p1 = 0;
	uint32_t x1_p2 = 0;
	uint32_t x2_p1 = 0;
	uint32_t x2_p2 = 0;
	uint32_t x1_pre = 0;
	uint32_t x1_new = 0;
	uint32_t x2_pre = 0;
	uint32_t x2_new = 0;
	uint32_t* pudOut = NULL;
	uint32_t num_pn_word = 0;

	pudOut = (uint32_t*)pucDataOut;

	/*初始化x1序列*/
	x1_pre = 0x80000001;
	/*初始化x2序列*/
	x2_p1 = do_brev(udCinit);
	x2_p2 = (x2_p1 >> 31) ^ _extu(x2_p1, 1, 31) ^ _extu(x2_p1, 2, 31) ^ _extu(x2_p1, 3, 31);
	x2_pre = x2_p1 | x2_p2;

	/* x1x2C */
	num_pn_word = (udSequenceLen + 31) >> 5;

	/* 计算前1600个bit */
	for (udLoopIdx = 1; udLoopIdx < 50; udLoopIdx++)/*Nc=1600*/
	{
		/*x1序列*/
		x1_new = (x1_pre << 1) ^ (x1_pre << 4);/*前28bit*/
		x1_p1 = (x1_pre << 1) | (x1_new >> 31);
		x1_p2 = (x1_pre << 4) | (x1_new >> 28);
		x1_pre = x1_p1 ^ x1_p2;
		/*x2序列*/
		x2_new = (x2_pre << 1) ^ (x2_pre << 2) ^ (x2_pre << 3) ^ (x2_pre << 4);/*前28bit*/
		x2_p1 = ((x2_pre << 1) | (x2_new >> 31)) ^ ((x2_pre << 2) | (x2_new >> 30));
		x2_p2 = ((x2_pre << 3) | (x2_new >> 29)) ^ ((x2_pre << 4) | (x2_new >> 28));
		x2_pre = x2_p1 ^ x2_p2;
	}

	for (udLoopIdx = 0; udLoopIdx < num_pn_word; udLoopIdx++)
	{
		/*x1序列*/
		x1_new = (x1_pre << 1) ^ (x1_pre << 4);/*前28bit*/
		x1_p1 = (x1_pre << 1) | (x1_new >> 31);
		x1_p2 = (x1_pre << 4) | (x1_new >> 28);
		x1_pre = x1_p1 ^ x1_p2;
		/*x2序列*/
		x2_new = (x2_pre << 1) ^ (x2_pre << 2) ^ (x2_pre << 3) ^ (x2_pre << 4);/*前28bit*/
		x2_p1 = ((x2_pre << 1) | (x2_new >> 31)) ^ ((x2_pre << 2) | (x2_new >> 30));
		x2_p2 = ((x2_pre << 3) | (x2_new >> 29)) ^ ((x2_pre << 4) | (x2_new >> 28));
		x2_pre = x2_p1 ^ x2_p2;
		pudOut[udLoopIdx] = x1_pre ^ x2_pre;
	}
	return;
}