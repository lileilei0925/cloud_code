#pragma once
#include <stdio.h>
#include <string.h>
#include "../inc/common_typedef.h"
#include "../inc/common_macro.h"
#include "../inc/phy_ctrl_common.h"
#include "../../phy_ctrl/ul_ctrl/pusch/inc/phyctrl_pusch.h"
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
uint16_t ceilDiv (uint16_t a, uint16_t b)
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
* 函数名称: findBit1NumAndIndex
* 函数功能: 对输入数据以二进制从低位到高位统计bit1的个数和对应的位置索引
* 相关文档: 
* 函数参数:
* 参数名称:   类型   输入/输出   描述
*
* inputData  uint16_t   in      输入数据
* bit1Num    uint16_t*  out     输出inputData中1的bit数量      
* bit1Index  uint16_t*  out     输出inputData中1对应的bit索引   
*
* 返回值:   无
* 函数类型: <回调、中断、可重入（阻塞、非阻塞）等函数必须说明类型及注意事项>
* 函数说明:（以下述列表的方式说明本函数对全局变量的使用和修改情况，以及本函数
*
*******************************************************************************/
void findBit1NumAndIndex (uint16_t inputData, uint16_t *bit1Num, uint16_t *bit1Index)
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
* 函数名称: findRbgNumAndSize
* 函数功能: 对输入数据以二进制从低位到高位统计连续1的个数，每组连续的1认为是一个RBG，并返回RBG的数量，和每个RBG的起始RB和RB数
* 相关文档: 
* 函数参数:
* 参数名称:   类型   输入/输出   描述
*
* inputData        uint8_t*   in           输入数据地址，uint8_t*格式输入
* dataNum          uint8_t    in           输入数量的个数,总数据量为sizeof(uint8_t)*dataNum
* rbgNum           uint8_t*   out          输出inputData中连续1的组数      
* rbgStartAndsize  NrPuschCePara*  out     输出inputData中每组RBG的起始RB和RB数   
*
* 返回值:   无
* 函数类型: <回调、中断、可重入（阻塞、非阻塞）等函数必须说明类型及注意事项>
* 函数说明:（以下述列表的方式说明本函数对全局变量的使用和修改情况，以及本函数
*
*******************************************************************************/
void findRbgNumAndSize (uint8_t *inputData, uint8_t dataNum, uint8_t *rbgNum, NrPuschCePara *rbgStartAndsize)
{
    uint8_t loopIndex0, loopIndex1;
    uint8_t tempData ;
    uint8_t bitOneCnt, allBitCnt, rbgCnt;

    rbgCnt    = 0; 
    bitOneCnt = 0;
    allBitCnt = 0;
    for (loopIndex0 = 0; loopIndex0 < dataNum; loopIndex0++){
        tempData = inputData[loopIndex0];
        for (loopIndex1 = 0; loopIndex1 < 8; loopIndex1++){   
            if ((tempData & 0x1) == 1){
                bitOneCnt++;      //记录连续1的个数，即当前rbg簇的大小
            }
            else{
                if(bitOneCnt > 0){ // 如果出现0且bitCnt大于0，则认为连续的1中断，记录本次的rbg大小，和rbg起始，且连续rbg计数自加
                    rbgStartAndsize->rbStart = allBitCnt - bitOneCnt;  //起始
                    rbgStartAndsize->rbNum   = bitOneCnt;              //大小
                    rbgStartAndsize++;  
                    bitOneCnt = 0;
                    rbgCnt++;
                }
            }
            tempData = tempData >> 1;
            allBitCnt++;
        }
    }
    *rbgNum = rbgCnt;
}

uint16_t CalcCsiPart2BitLength(UciInfoAddInV3 *uciInfoAddInV3, uint8_t *CsiPart1Payload, uint8_t *sizesPart1Params, uint16_t *map , uint16_t csiPart1BitLength, uint8_t numPart1Params)
{
    uint16_t numPart2s;
    uint16_t paramOffsets;
    uint16_t ParamValue;
    uint16_t mapIndex;
    uint8_t  Part1ParamsIdx;
    uint8_t  paramSizes;
    uint8_t  bitNum;
    Part2ReportInfo *part2ReportInfo = NULL;
    
    numPart2s         = 1;//uciInfoAddInV3->numPart2s;暂时只支持1个CSI report
    part2ReportInfo   = uciInfoAddInV3->part2ReportInfo;
    if(numPart1Params != part2ReportInfo->part1ParamsNum)
    {
        return 0;
    }

    mapIndex = 0;
    for(Part1ParamsIdx = 0; Part1ParamsIdx < numPart1Params; Part1ParamsIdx++)
    {
        paramOffsets = part2ReportInfo->paramOffsets[Part1ParamsIdx];
        paramSizes   = part2ReportInfo->paramSizes[Part1ParamsIdx];  
        ParamValue   = InterceptData(CsiPart1Payload, paramOffsets, paramSizes);//CsiPart1Payload,paramOffsets,paramSizes?从CsiPart1Payload截取值
        bitNum       = sizesPart1Params[Part1ParamsIdx];
        ParamValue   = ParamValue&genbitmask(bitNum);//二次截取
        mapIndex     = (mapIndex<<bitNum) + ParamValue;
    }

    return map[mapIndex];
}

/*状态机注册,给它一个状态表*/
void FSM_Regist(FSM *fsm, FsmTable *table)
{
    fsm->fsmTable = table;
}

/*状态迁移*/
void FSM_StateTransfer(FSM *fsm, uint32_t state)
{
    fsm->curState = state;
}

/*事件处理*/
void FSM_EventHandle(FSM *fsm, uint32_t event)
{
    int curState;
    int nextState;
    int size;
    int flag = 0; //标识是否满足状态转移条件
    int index;
    FsmTable *ActTable = fsm->fsmTable;
    void (*handlerFun)()  = NULL;  //函数指针初始化为空

    curState  = fsm->curState;
    size      = fsm->size;

    /*获取当前动作函数*/
    for (index = 0; index < size; index++)
    {
        //当前状态下来指定事件，才会状态转移
        if ((curState == ActTable[index].curState) && (event == ActTable[index].event))
        {
            flag = 1;
            handlerFun = ActTable[index].handlerFun;
            nextState  = ActTable[index].nextState;
            break;
        }
    }

    if (flag) //如果满足条件了
    {
        /*动作执行*/
        if (NULL != handlerFun)
        {
            handlerFun();
        }

        //跳转到下一个状态
        FSM_StateTransfer(fsm, nextState);
    }
    else
    {
        printf("状态转移异常\n");
    }
}

void SendTlvMsg(TLV_MSG *tlvMsg, uint8_t *buffer, uint8_t *value)
{ 
	uint8_t  pduBitmap;
	uint16_t pduSize;
	uint16_t tempSize;
	
	FapiNrPucchFmt01Indication  *fapiNrPucchFmt01Indication = NULL;
	FapiNrPucchFmt23Indication  *fapiNrPucchFmt23Indication = NULL;
	FapiNrPushUciIndication 	*fapiNrPushUciIndication 	= NULL;
	SRInfoFmt01                 *srInfoFmt01 	            = NULL;
	SRInfoFmt23                 *srInfoFmt23 	            = NULL;
	HARQInfoFmt01               *harqInfoFmt01	            = NULL;
	HARQInfoFmt23               *harqInfoFmt23	            = NULL;
	CSIpart1Info                *csipart1Info               = NULL;
	CSIpart2Info                *csipart2Info               = NULL;

	pduSize = 0;
    /* 根据tlvBodyType对数据进行处理 */
    switch(tlvMsg->tlvBody.PDUType) {
        case PUSCH_UCI_INDICATION:
			{
				fapiNrPushUciIndication = (FapiNrPushUciIndication  *)value;
				pduSize = sizeof(FapiNrPushUciIndication) - sizeof(HARQInfoFmt23) - sizeof(CSIpart1Info) - sizeof(CSIpart2Info);
				memcpy(buffer,fapiNrPushUciIndication,pduSize);
				pduBitmap = fapiNrPucchFmt23Indication->pduBitmap;
				if(pduBitmap&0x2)//存在HARQ
				{
					harqInfoFmt23 = &(fapiNrPushUciIndication->harqInfoFmt23);
					tempSize      = sizeof(HARQInfoFmt23) - (48 - (harqInfoFmt23->HarqBitLen + 7)>>3);
					memcpy((buffer + pduSize),&(fapiNrPushUciIndication->harqInfoFmt23),tempSize);
					pduSize += tempSize;
				}
				if(pduBitmap&0x4)//存在CSI Part1
				{
					csipart1Info = &(fapiNrPushUciIndication->csipart1Info);
					tempSize     = sizeof(CSIpart1Info) - (48 - (csipart1Info->CsiPart1BitLen + 7)>>3);
					memcpy((buffer + pduSize),&(fapiNrPushUciIndication->csipart1Info),tempSize);
					pduSize += tempSize;
				}
				if(pduBitmap&0x8)//存在CSI Part2
				{
					csipart2Info = &(fapiNrPushUciIndication->csipart2Info);
					tempSize     = sizeof(CSIpart2Info) - (48 - (csipart2Info->CsiPart2BitLen + 7)>>3);
					memcpy((buffer + pduSize),&(fapiNrPushUciIndication->csipart2Info),tempSize);
					pduSize += tempSize;
				}
			}
		break;
        case PUCCH_FMT01_INDICATION:
            {
                fapiNrPucchFmt01Indication = (FapiNrPucchFmt01Indication  *)value;
				pduSize = sizeof(FapiNrPucchFmt01Indication) - sizeof(SRInfoFmt01) - sizeof(HARQInfoFmt01);
				memcpy(buffer,fapiNrPucchFmt01Indication,pduSize);
				pduBitmap = fapiNrPucchFmt01Indication->pduBitmap;
				if(pduBitmap&0x1)//存在SR
				{
				    tempSize = sizeof(SRInfoFmt01);
					memcpy((buffer + pduSize),&(fapiNrPucchFmt01Indication->srInfoFmt01),tempSize);
					pduSize += tempSize;
				}
				if(pduBitmap&0x2)//存在HARQ
				{
					harqInfoFmt01 = &(fapiNrPucchFmt01Indication->harqInfoFmt01);
					tempSize      = sizeof(HARQInfoFmt01) - (2 - harqInfoFmt01->NumHarq);
					memcpy((buffer + pduSize),&(fapiNrPucchFmt01Indication->harqInfoFmt01),tempSize);
					pduSize += tempSize;
				}                   
            }
            break;
		case PUCCH_FMT23_INDICATION:
            {
                fapiNrPucchFmt23Indication = (FapiNrPucchFmt23Indication  *)value;
				pduSize = sizeof(FapiNrPucchFmt23Indication) - sizeof(SRInfoFmt23) - sizeof(HARQInfoFmt23) - sizeof(CSIpart1Info) - sizeof(CSIpart2Info);
				memcpy(buffer,fapiNrPucchFmt23Indication,pduSize);
				pduBitmap = fapiNrPucchFmt23Indication->pduBitmap;
				if(pduBitmap&0x1)//存在SR
				{
				    srInfoFmt23 = &(fapiNrPucchFmt23Indication->srInfoFmt23);
				    tempSize    = sizeof(SRInfoFmt23) - 1;
					memcpy((buffer + pduSize),&(fapiNrPucchFmt23Indication->srInfoFmt23),tempSize);
					pduSize += tempSize;
				}
				if(pduBitmap&0x2)//存在HARQ
				{
					harqInfoFmt23 = &(fapiNrPucchFmt23Indication->harqInfoFmt23);
					tempSize      = sizeof(HARQInfoFmt23) - (48 - (harqInfoFmt23->HarqBitLen + 7)>>3);
					memcpy((buffer + pduSize),&(fapiNrPucchFmt23Indication->harqInfoFmt23),tempSize);
					pduSize += tempSize;
				}
				if(pduBitmap&0x4)//存在CSI Part1
				{
					csipart1Info = &(fapiNrPucchFmt23Indication->csipart1Info);
				    tempSize     = sizeof(CSIpart1Info) - (48 - (csipart1Info->CsiPart1BitLen + 7)>>3);
					memcpy((buffer + pduSize),&(fapiNrPucchFmt23Indication->csipart1Info),tempSize);
					pduSize += tempSize;
				}
				if(pduBitmap&0x8)//存在CSI Part2
				{
					csipart2Info = &(fapiNrPucchFmt23Indication->csipart2Info);
				    tempSize     = sizeof(CSIpart2Info) - (48 - (csipart2Info->CsiPart2BitLen + 7)>>3);
					memcpy((buffer + pduSize),&(fapiNrPucchFmt23Indication->csipart2Info),tempSize);
					pduSize += tempSize;
				}
            }
            break;
        default:
            break;
			
		tlvMsg->tlvBody.PDUSize = pduSize;
    }
}
