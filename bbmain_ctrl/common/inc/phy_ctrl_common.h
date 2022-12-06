#pragma once
#include "common_macro.h"

/* P5 Cell configuration messages local structure*/
typedef struct 
{
    uint8_t  cellType;                 /* 0: NR, 1: LTE */
    uint8_t  cellIndex;
    uint8_t  frameDuplexType;
    uint16_t bandWidthDl;
    uint16_t bandWidthUl;
    uint16_t txAntNum;
    uint16_t rxAntNum;
    /* prach 算法相关接口 */
    uint16_t winThSingle;
    uint16_t winThMulti;
    /* pusch 算法相关接口 */
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
    uint16_t sfnIndex;                         /* system frame number [0: 1023] */
    uint16_t slotIndex;                        /* SLOT number [0: 159] */
    uint16_t pduNum;                           /* Number of PDUs that are included in this message */
    uint8_t  ulPduTypes;                       /* Maximum number of UL PDU types supported by UL_TTI.request */
    uint16_t pduNumPerType[MAX_UL_PDU_TYPES];  /* Number of PDUs of each type that are included in this message,nUlPduTypes = 5 */
    uint8_t  ueGroupNum;                       /* Number of UE Groups included in this message */
} UlTtiRequestHeadInfo;


typedef struct
{
    uint16_t            sfn;           //系统帧号，取值范围[0,1023]，随路信息
    uint8_t             slot;          //时隙号，取值范围[0,19]，随路信息
	uint8_t             cellIdx;       //小区索引，取值范围[0,3]，随路信息
	
    uint8_t             pduNum;        //译码配置个数，取值范围[0,3]
    uint8_t             intFlag;       //译码完成后HAC是否触发中断标志，0：不触发，1：触发
    uint8_t             codeType;      //HAC编码/译码类型，随路信息
    uint8_t             msgType;       //消息类型

    uint8_t             msgIdx;        //本包数据的消息索引
    uint8_t             rsvd[3];
    
    uint32_t            OutputAddr;     //译码输出数据起始地址  //head(8字节)+data（32字节对齐），data返回数据格式待定，松排或紧排确认即可
}HacCfgHead;

typedef struct
{
    uint32_t      segStartAddr;
    uint16_t      segCycNum;
    uint32_t      segLlrNum;
    uint32_t      segPeriod;
}LlrSegInfo;

typedef struct
{	
	uint8_t       sizeT;              //解信道交织使用的T值，满足T(T+1)/2>=E的最小整数,译码输入参数
    uint8_t	      nVal;	              //译码输入比特长度N=2^n，n的取值范围[5,10],译码输入参数
    int16_t       uciBitNum;          //译码最终输出UCI比特数,212协议5.2.1章节中的A值

    uint32_t      llrNum;             //LLR总个数，取值范围[]
    
    uint8_t       ueIdx;   
    uint8_t       uciType; 
    uint8_t       llrSegNum; 
    uint8_t       rsvd; 
    LlrSegInfo    llrSegInfo[5];

    uint32_t      OutputAddr;     //译码输出数据起始地址 
}PolarDecodePduInfo;

typedef struct
{
    uint8_t       uciBitNum;           //译码输出比特长度，取值范围[3,11],LTE扩展CP PUCCH fmt2a/2b可取值12和13,译码输入参数                                   
    uint8_t       codeMethod;          //20对应LTE PUCCH的（20，A）编码；24对应LTE PUCCH的（24，O）编码；32对应NR PUCCH&PUSCH的（32，K）或LTE PUSCH的（32，O）编码；
    uint8_t       ueIdx;
    uint8_t       uciType;
    
    uint32_t      llrNum;              //取值20,24或32。20对应LTE PUCCH的（20，A）编码，24对应LTE PUCCH的（24，O）编码，32对应NR PUCCH&PUSCH的（32，K）或LTE PUSCH的（32，O）编码,译码输入参数

    uint8_t	      llrSegNum;	   
    uint8_t       rsvd[3];
    
    LlrSegInfo    llrSegInfo[5];
	uint32_t      OutputAddr;           //译码输出数据地址
}RMDecodePduInfo;

typedef struct
{
    uint8_t          rsvd[16];          //待确认具体字段后补充
}HacOutputHead;

typedef struct
{
    uint8_t          rsvd[52];          //待确认具体字段后补充
}PolarUeDecodeOut;

typedef struct
{
    uint8_t          rsvd[8];          //待确认具体字段后补充
}RMUeDecodeOut;

typedef struct 
{                          
    HacOutputHead    hacOutputHead;

    PolarUeDecodeOut polarUeDecodeOut[MAX_PUSCH_PDU_NUM];
}PolarDecodeOut;

typedef struct 
{
    HacOutputHead    hacOutputHead;
    
    RMUeDecodeOut    rmUeDecodeOut[MAX_PUSCH_PDU_NUM];
}RMDecodeOut;

typedef struct FsmTable{
    uint32_t  curState;           //当前状态
	uint32_t  event;              //trigger事件
    void      (*handlerFun)();    //handler函数指针
    uint32_t  nextState;          //转移后的状态
}FsmTable;

typedef struct FSM{
    uint32_t curState;//当前状态
    FsmTable *fsmTable;//状态表
    uint32_t size;//表的项数
}FSM;

typedef struct TLV_HEAD
{
    uint16_t SFN;         
    uint16_t Slot;         
    uint16_t NumUCIs;      
}TLV_HEAD;

typedef struct TLV_BODY
{
    uint16_t PDUType;           // TLV 消息tag（消息编号id），根据此tag解析value
    uint16_t PDUSize;      		// TLV 消息值的长度
    uint16_t tlvBodyValue[0];   // TLV 消息值
}TLV_BODY;

typedef struct TLV_MSG
{
    struct TLV_HEAD tlvHead;
    struct TLV_BODY tlvBody;
}TLV_MSG;

uint32_t          g_ulTtiMessageTempBuff[2000] = { 0 };            /* ULTTIMessage 本地buffer */
UlPduMappingInfo  g_ulPduMappingInfo[MAX_CELL_NUM][200] = { 0 };   /* 暂时假设有200个PDU */
L1CellConfigInfo  g_cellConfigPara[MAX_CELL_NUM] = { 0 };

uint8_t Buffer[65536] = {0};//64k