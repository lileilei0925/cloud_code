#pragma once
#include <stdio.h>
#include <string.h>
#include "../inc/common_typedef.h"
#include "../inc/common_macro.h"

/*******************************************************************************
* 函数名称: do_brev
* 函数功能: 完成对一个Uint 32位数bit位反转存储
* 相关文档:
* 函数参数:
* 参数名称:   类型   输入/输出   描述
* 返回值:   result bit翻转后的值
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
* 函数名称: PseudoRandomSeqGen
* 函数功能: 计算c(n)序列
* 相关文档: 3GPP TS 211 计算PN序列
* 函数参数:
* 参数名称:   类型   输入/输出   描述
*
* pucDataOut        UINT32*  out       c序列的指针
* Cinit             UINT32   in        cinit值
* SequenceLen       UINT32   in        序列的长度
* StartSaveIdx      UINT8    in        开始保存的索引,以DW为单位
*
* 返回值:   无
* 函数类型: <回调、中断、可重入（阻塞、非阻塞）等函数必须说明类型及注意事项>
* 函数说明:（以下述列表的方式说明本函数对全局变量的使用和修改情况，以及本函数
*
*******************************************************************************/
void PseudoRandomSeqGen(uint32_t* pDataOut, uint32_t Cinit, uint32_t SequenceLen, uint8_t StartSaveIdx)
{
	uint32_t LoopIdx = 0;
	uint32_t x1_p1 = 0;
	uint32_t x1_p2 = 0;
	uint32_t x2_p1 = 0;
	uint32_t x2_p2 = 0;
	uint32_t x1_pre = 0;
	uint32_t x1_new = 0;
	uint32_t x2_pre = 0;
	uint32_t x2_new = 0;
	uint32_t num_pn_word = 0;

	/*初始化x1序列*/
	x1_pre = 0x80000001;//do_brev(0x80000001)
	/*初始化x2序列*/
	x2_p1 = do_brev(Cinit);
	x2_p2 = (x2_p1 >> 31) ^ _extu(x2_p1, 1, 31) ^ _extu(x2_p1, 2, 31) ^ _extu(x2_p1, 3, 31);
	x2_pre = x2_p1 | x2_p2;

	/* x1x2C */
	num_pn_word = (SequenceLen + 31) >> 5;//向上取整

	/* 计算前1600个bit */
	for (LoopIdx = 1; LoopIdx < 50; LoopIdx++)/*Nc=1600*/
	{
		/*x1序列*/
		x1_new = (x1_pre << 1) ^ (x1_pre << 4);/*前28bit*/
		x1_p1 = (x1_pre << 1) | (x1_new >> 31);
		x1_p2 = (x1_pre << 4) | (x1_new >> 28);
		x1_pre = x1_p1 ^ x1_p2;/*前28bit和后4bit拼接*/
		/*x2序列*/
		x2_new = (x2_pre << 1) ^ (x2_pre << 2) ^ (x2_pre << 3) ^ (x2_pre << 4);/*前28bit*/
		x2_p1 = ((x2_pre << 1) | (x2_new >> 31)) ^ ((x2_pre << 2) | (x2_new >> 30));
		x2_p2 = ((x2_pre << 3) | (x2_new >> 29)) ^ ((x2_pre << 4) | (x2_new >> 28));
		x2_pre = x2_p1 ^ x2_p2;/*前28bit和后4bit拼接*/
	}

	for (LoopIdx = 0; LoopIdx < num_pn_word; LoopIdx++)
	{
		/*x1序列*/
		x1_new = (x1_pre << 1) ^ (x1_pre << 4);/*前28bit*/
		x1_p1 = (x1_pre << 1) | (x1_new >> 31);
		x1_p2 = (x1_pre << 4) | (x1_new >> 28);
		x1_pre = x1_p1 ^ x1_p2;/*前28bit和后4bit拼接*/
		/*x2序列*/
		x2_new = (x2_pre << 1) ^ (x2_pre << 2) ^ (x2_pre << 3) ^ (x2_pre << 4);/*前28bit*/
		x2_p1 = ((x2_pre << 1) | (x2_new >> 31)) ^ ((x2_pre << 2) | (x2_new >> 30));
		x2_p2 = ((x2_pre << 3) | (x2_new >> 29)) ^ ((x2_pre << 4) | (x2_new >> 28));
		x2_pre = x2_p1 ^ x2_p2;/*前28bit和后4bit拼接*/

		/*按照要求保存*/
		if(LoopIdx >= StartSaveIdx)
		{
			*pDataOut = x1_pre ^ x2_pre;
			pDataOut++;
		}
	}
	return;
}

/*******************************************************************************
* 函数名称: ceil_div
* 函数功能: 实现向上取整的除法
* 相关文档: 
* 函数参数:
* 参数名称:   类型   输入/输出   描述
*
* a        uint16_t   in       被除数 
* b        uint16_t   in       除数 
*
* 返回值:   c 
* 函数类型: <回调、中断、可重入（阻塞、非阻塞）等函数必须说明类型及注意事项>
* 函数说明:（以下述列表的方式说明本函数对全局变量的使用和修改情况，以及本函数
*
*******************************************************************************/
uint16_t ceil_div(uint16_t a, uint16_t b)
{
	uint16_t c = a / b;

	if (a > b * c){
		return (c + 1);
	}
	else{
		return c;
	}
}

/*******************************************************************************
* 函数名称: count_bit1_and_index
* 函数功能: 对输入数据以二进制从低位到高位统计bit1的个数和对应的位置索引
* 相关文档: 
* 函数参数:
* 参数名称:   类型   输入/输出   描述
*
* inputData  uint16_t  in      输入数据
* bit1Num    uint8_t*  out     输出inputData中1的bit数量      
* bit1Index  uint8_t*  out     输出inputData中1对应的bit索引   
*
* 返回值:   无
* 函数类型: <回调、中断、可重入（阻塞、非阻塞）等函数必须说明类型及注意事项>
* 函数说明:（以下述列表的方式说明本函数对全局变量的使用和修改情况，以及本函数
*
*******************************************************************************/
void count_bit1_and_index(uint16_t inputData, uint8_t *bit1Num, uint8_t *bit1Index)
{
	uint8_t count0 = 0;
	uint8_t count1 = 0;

	while (inputData)
	{
		if((inputData % 2) == 1){
            bit1Index[count0] = count1;
			count0++;
		}
		inputData = inputData >> 1;
		count1++;
	}
	*bit1Num = count0;
}

/*******************************************************************************
* 函数名称: genbitmask
* 函数功能: 从低位到高位，根据输入比特个数，生成比特掩码
* 相关文档: 
* 函数参数:
* 参数名称:     类型      输入/输出   描述
*
* inputbitNum  uint8_t   in        输入比特个数
* bitmask      uint16_t  out       输出比特掩码    
*
* 返回值:   无
* 函数类型: <回调、中断、可重入（阻塞、非阻塞）等函数必须说明类型及注意事项>
* 函数说明:（以下述列表的方式说明本函数对全局变量的使用和修改情况，以及本函数
*
*******************************************************************************/
uint16_t genbitmask(uint8_t bitNum)
{
	uint8_t bitmask = 0;

	while (bitNum)
	{
		bitmask += (1<<(bitNum-1)); 
		bitNum--;
	}
	return bitmask;
}


/*******************************************************************************
* 函数名称: InterceptData
* 函数功能: 从低位到高位，根据输入比特个数，生成比特掩码
* 相关文档: 
* 函数参数:
* 参数名称:       类型   输入/输出   描述
*
* DataIn         uint8_t*   in     输入数据起始地址
* paramOffsets   uint16_t   in     输入参数偏移
* paramSizes     uint8_t    in     输入参数长度 
*                uint16_t   out    输出数据       
*
* 返回值:   无
* 函数类型: <回调、中断、可重入（阻塞、非阻塞）等函数必须说明类型及注意事项>
* 函数说明:（以下述列表的方式说明本函数对全局变量的使用和修改情况，以及本函数
*
*******************************************************************************/
uint16_t InterceptData(uint8_t *DataIn , uint16_t paramOffsets, uint8_t paramSizes)
{
	uint8_t StartIdx,EndIdx;
	uint16_t bitmask;

	StartIdx = (paramOffsets >> 8);
	EndIdx   = ((paramOffsets + paramSizes) >>8 );
//paramSize是否会超过8？
	bitmask  = genbitmask(paramSizes);
	if(EndIdx = (StartIdx + 1))
	{
		return ((((*(DataIn + StartIdx)) << 4) + *(DataIn + EndIdx)) >> (8 - (paramOffsets&0x7)) & bitmask);
	}
	else
	{
		return (((*(DataIn + EndIdx)) >> (8 - (paramOffsets&0x7))) & bitmask);
	}
	
}


/*******************************************************************************
* 函数名称: log2Ceiling
* 函数功能: 计算log2的值，结果向上取整
* 相关文档: 
* 函数参数:
* 参数名称:   类型     输入/输出   描述
*
* DataIn    uint16_t    in      输入数据
*
* 返回值:   uint16_t
* 函数类型: <回调、中断、可重入（阻塞、非阻塞）等函数必须说明类型及注意事项>
* 函数说明:（以下述列表的方式说明本函数对全局变量的使用和修改情况，以及本函数
*
*******************************************************************************/
uint16_t log2Ceiling(uint16_t dataIn) 
{
  uint16_t cnt = 0; 
  
  while(dataIn) {
    dataIn >>= 1; 
    cnt++; 
  }
  return (dataIn > (1<<(cnt - 1))) ? cnt:(cnt - 1); //如果入参为0，返回无效值0xffff
}