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
    uint8_t       cbIdx;
	uint8_t       sizeCrcL;       //每个码块CRC 的L
	uint8_t       polarEncodeLayerNum;  //Polar译码层数  log2（sizeInput）
	uint8_t       pathNum;     //译码路径
	uint8_t       lenQpc;      //校验位的个数  ；定值为3
	uint8_t       Qpc[3]; 
	uint16_t      sizeInput;      //每个码块输出的原始bit长度 
	
    uint32_t      *cbInputAddr;  //每个码块的取的地址；PBCH:一次加扰之后的输出bit流
    	
	uint32_t	  sizeCodingOut;	//编码后bit长度  2^n
    uint8_t       bitmap[64];   //bitmap 
		     
}EncodeCBInfo;

typedef struct
{
    uint8_t       pduIdx;
    uint8_t       cbNum; 
	//速率匹配
	uint8_t       typeRM;  //0: repetition 1: punturing 2:shortening  打掉符号的位置
	uint8_t       interTval;  //PUCCH : 1	 T(T+1)/2 >= E
	
	uint8_t       *pBitInputAddr;
	uint16_t	   sizeRmLenth;	   // PBCH:E= 864
	EncodeCBInfo   cbinfo[];
}EncodePduInfo;

typedef struct
{
    uint16_t    sfn;
    uint16_t    slot;
	uint8_t     cellIdx;
	uint8_t     pduNum;
	uint8_t     *pOutBaseAddr;   //数据的首地址；
	EncodePduInfo  polarPduInfo[];    
}PhyEncodeHacCfgParam;

uint32_t          g_ulTtiMessageTempBuff[2000] = { 0 };            /* ULTTIMessage 本地buffer */
UlPduMappingInfo  g_ulPduMappingInfo[MAX_CELL_NUM][200] = { 0 };   /* 暂时假设有200个PDU */
L1CellConfigInfo  g_cellConfigPara[MAX_CELL_NUM] = { 0 };