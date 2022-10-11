#pragma once
#include "common_macro.h"

/* P5 Cell configuration messages local structure*/
typedef struct 
{
    uint8_t  product;                 /* 0: NR, 1: LTE */
    uint8_t  cellIndex;
    uint8_t  frameDuplexType;
    uint16_t bandWidthDl;
    uint16_t bandWidthUl;
    uint16_t txAntNum;
    uint16_t rxAntNum;
    	
    uint8_t  puschEqAlgFlag;          /*0:MRC,1:IRC,2:adaptive*/
    uint8_t  ruuAvgFlag;              /*主要针对两列导频计中，计算完最后一列导频后的RUU是否求平均，可配置*/
    uint8_t  ruuAvgRbGrpNum;          /*频域平局计算颗粒度*/
    uint8_t  fOEstFlag;               /*频偏估计开关*/
} L1CellConfigInfo;

/* UeGroupInfo,包含ue数和每个ue对应得pduIndex */
typedef struct
{
    uint8_t ueNum;           /* Number of UE in this group For SU-MIMO, one group includes one UE only. For MU-MIMO, one group includes up to 12 UEs */
    uint8_t pduIdx[];        /* This value is an index for number of PDU identified by nPDU in this message */
} UlueGoupNumInfo;

/* UlPduMappingInfo, 用来解析pdu和ueGroup之间的对应关系 */
typedef struct 
{
    uint8_t  groupIndex;
    uint8_t  ueIndex;
    uint16_t ueRnti;
    uint8_t  pduIndex;
} UlPduMappingInfo;

/* 每个PDU的头,包含pduType，pduSize */
typedef struct
{
    uint16_t pduType; /* 0:PRACH, 1:PUSCH, 2:PUCCH, 3:SRS */
    uint16_t pduSize; /* Size of the PDU control information (in bytes). This length value includes the 4 bytes required for the PDU type and PDU size parameters */
} PduHeadInfo;

/* Slot Messages Ul_TTI.request的头,PDU信息之前的部分 */
typedef struct
{
    uint16_t sfnNum;                           /* system frame number [0: 1023] */
    uint16_t slotNum;                          /* SLOT number [0: 159] */
    uint16_t pduNum;                           /* Number of PDUs that are included in this message */
    uint8_t  ulPduTypes;                       /* Maximum number of UL PDU types supported by UL_TTI.request */
    uint16_t pduNumPerType[MAX_UL_PDU_TYPES];  /* Number of PDUs of each type that are included in this message,nUlPduTypes = 5 */
    uint8_t  ueGroupNum;                       /* Number of UE Groups included in this message */
} UlTtiRequestHeadInfo;


typedef struct
{
    uint16_t            sfn;
    uint16_t            slot;

	uint8_t             cellIdx;
	uint8_t             pduNum;
    uint8_t             msgType;         //区分PUCCH还是PUSCH的消息,0:PUCCH,1:PUSCH
    uint8_t             rsvd;
}HacCfgHead;

typedef struct
{	
    uint8_t       sizeCrcL;            //每个码块CRC比特长度L，取值6或11
    uint8_t       lenQpc;              //PC比特个数，取值0或3
	uint8_t       typeRM;              //速率匹配的类型，0: repetition 1: punturing 2:shortening  
	uint8_t       interTval;           //信道交织使用的T值，满足T(T+1)/2>=E的最小整数

    uint16_t      sizeInput;           //每个码块对应的原始bit长度K 
	uint16_t	  sizeRmLenth;	       //速率匹配后每个码块的比特值E

    uint8_t       pathNum;             //译码路径   算法参数,待算法确认
    uint8_t       CrcOut;	           //译码的CRC结果，bit0有效，0：错误，1：正确
    uint8_t       rsvd[2];
	
	uint16_t      Qpc[3];              //PC比特位置索引，取值范围[0,1023]
	uint16_t      sizeOutput;          //译码输出bit长度K 

    uint32_t      BitInputAddrOffset;  //输入数据地址相对首地址的偏移
	uint32_t      OutAddrOffset;       //输出数据地址相对首地址的偏移
		
	uint32_t	  sizeCodingOut;	   //译码输入比特长度N=2^n，n的取值范围[5,10]
}PolarDecodePduInfo;

typedef struct
{
    HacCfgHead  hacCfgHead;

    uint8_t    *BitInputAddr;   //输入数据的首地址
	uint8_t    *OutBaseAddr;    //输出数据的首地址

	PolarDecodePduInfo  polarPduInfo[];    
}PolarDecodeHacCfgPara;

typedef struct
{
    uint8_t            pduIdx;           //配置索引，取值范围[0,pduNum-1]
    uint8_t            UciBitNum;        //译码输出比特长度，取值范围[3,11],LTE扩展CP PUCCH fmt2a/2b可取值12和13                                   
    uint8_t            LlrNum;           //取值20,24或32。20对应LTE PUCCH的（20，A）编码，24对应LTE PUCCH的（24，O）编码，32对应NR PUCCH&PUSCH的（32，K）或LTE PUSCH的（32，O）编码
    uint8_t            rsvd;

    uint16_t	       RateMatchBitLen;	 //RM译码解速率匹配前的比特数。LTE PUCCH取值20或48，LTE PUSCH取值范围[？];NR PUCCH取值范围[6,4608],NR PUSCH取值范围[？]
    uint16_t           RmDecodeOut;      //RM译码结果，bit0-bit10有效，LTE扩展CP PUCCH fmt2a/2b对应bit0-bit12有效
    
    uint16_t           RmDecodeMaxDtx;   //RM最大打分值的DTX检测结果，取值范围[]
    uint16_t           RmDecodeSubDtx;   //RM次大打分值的DTX检测结果，取值范围[]

    uint8_t            *pBitInputAddr;   //输入数据存放起始地址
}RMDecodePduInfo;

typedef struct
{
    HacCfgHead          hacCfgHead;

	RMDecodePduInfo     rmPduInfo[];    
}RMDecodeHacCfgPara;

uint32_t          g_ulTtiMessageTempBuff[2000] = { 0 };            /* ULTTIMessage 本地buffer */
UlPduMappingInfo  g_ulPduMappingInfo[MAX_CELL_NUM][200] = { 0 };   /* 暂时假设有200个PDU */
L1CellConfigInfo  g_cellConfigPara[MAX_CELL_NUM] = { 0 };