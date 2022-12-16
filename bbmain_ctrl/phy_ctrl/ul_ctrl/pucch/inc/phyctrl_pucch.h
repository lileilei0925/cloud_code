#pragma once
#include "../../../../common/inc/common_typedef.h"
#include "../../../../common/inc/common_macro.h"
#include "../../../../common/inc/fapi_mac2phy_interface.h"

#define MAX_RX_ANT_NUM 4
#define MAX_OCC_NUM_FMT1 7
#define MAX_USER_NUM_PER_OCC 6
#define SC_NUM_PER_RB 12
#define MAX_DMRS_SYM_NUM 7
#define PUC_FMT3_MAX_DMRS_NUM 4
#define MAX_PUCCH_FMT0_1_NUM    (64)
#define MAX_PUCCH_FMT2_3_NUM    (38)
#define MAX_PUCCH_NUM           (MAX_PUCCH_FMT0_1_NUM + MAX_PUCCH_FMT2_3_NUM)
#define PUCCH_UCI_PING_PONG_NUM 2
#define MAX_PUCCH_FMT01_NUM 64    /* 待定 */
#define MAX_PUCCH_FMT23_NUM 38
#define MAX_PUCCH_3_BLOCK_NUM  10
#define PUCCH_RPT_BUFFER_SIZE  (1024*8)

enum PUCCH_FORMAT
{
    PUCCH_FORMAT_0 = 0,
    PUCCH_FORMAT_1 = 1,      
    PUCCH_FORMAT_2 = 2,
    PUCCH_FORMAT_3 = 3,    
    PUCCH_FORMAT_BUTT
 };

 enum PUCCH_PART
{
    PUCCH_PART1    =  0,
    PUCCH_PART2    =  1, 
    PUCCH_PART_NUM
 };

enum PUCCH_UCI_STATE{
    Pucch_Uci_Idle_State = 0,
    Pucch_Wait_Part1_Result_State,
    Pucch_Wait_Part2_Result_State,
    Pucch_Uci_Packing_State,
    Pucch_Uci_State_Num
};

enum PUCCH_UCI_EVENT{
	Pucch_Slot_Tast_Start_Event = 0,
	Pucch_Part1_Result_Trigger_Event,
	Pucch_Part2_Result_Trigger_Event,
	Pucch_UCI_Packing_Over_Event,
    Pucch_Uci_Event_Num
};

typedef struct
{
    uint8_t pduIdxInner;    /* 物理层内部使用的每个UE的索引 */
    uint8_t rsvd[3];
    uint16_t mcsBitMap;     /* ARM发送需要检测的MCS值给DSP，通过bitmap指示，bit0对应mcs=0，依次类推，bit0-bit11有效 */
    uint8_t cyclicShift[SYM_NUM_PER_SLOT];  /* 38.211协议 6.3.2.2.2计算得到的各符号α值，取值[0,11] */
}PucFmt0Param;

typedef struct
{
    uint8_t  pduIdxInner;                   /* 物理层内部使用的每个UE的索引 */
    uint8_t  rsvd;
    uint8_t  m0;                            /* initial cyclic shift */
    uint8_t  uciBitNum;  					/* UCI的payload,取值[1,2] */
}Fmt1UEParam;

typedef struct
{
    uint16_t ueTapBitMap;       /* 所有ue的alpha值(0-11)的bitmap，如alpha为3，那么bit3设为1; alpha = ((NR_SC_NUM_PER_RB - m0 ) mod NR_SC_NUM_PER_RB) */
    uint8_t rsvd[2];
    uint8_t timeDomainOccIdx;   /* 时域OCC的索引值，取值[0,6] */
    uint8_t userNumPerOcc;      /* 每个OCC上复用的用户数，协议规定最多12个，实际最多6个，取值[1,6] */
    Fmt1UEParam fmt1UEParam[MAX_USER_NUM_PER_OCC];/* 当前OCC上fmt1 UE参数 */
}Fmt1ParamOcc;

typedef struct
{
    uint8_t cyclicShift[SYM_NUM_PER_SLOT];   /* 38.211协议 6.3.2.2.2计算得到的各符号α值，取值[0,11]，注意不用加m0和mcs,数组内符号索引为fmt1所占符号 */
    uint8_t occNum;         /* occ数目 */
    uint8_t minUserOccIdx;  /* 复用用户数最少的OCC在数组中的索引,如果多个OCC上的用户数相同，取最小occ index的OCC在数组中的索引 */
    Fmt1ParamOcc fmt1ParamOcc[MAX_OCC_NUM_FMT1];/* fmt1 OCC级参数 */
}PucFmt1Param;

typedef struct
{
    uint8_t  pduIdxInner;        /* 物理层内部使用的每个UE的索引 */
    uint8_t  rsvd;
    uint16_t bitNum;            /* SR&HARQ&CSI part1三者bit数之和 */
    uint32_t *scrambSeqAddr;    /* 加扰序列在DDR中的存放地址 */
    uint32_t *llrAddr;          /* DSP处理完的LLR数据的存放地址，供HAC取数进行译码 */
}PucFmt2Param;

typedef struct
{
    uint16_t llrStart;       /* 某符号中，LLR在此PUCCH format3所占RE中的起始位置 */
    uint16_t llrDuration;    /* LLR持续数目，即所占连续RE数目 */
}Fmt3UciLlrPos;

typedef struct
{
    uint8_t pduIdxInner;        /* 物理层内部使用的每个UE的索引 */
    uint8_t pi2bpsk;            /* pi/2-BPSK是否使能标志，取值0:不使能，使用QPSK，1:使能，使用pi/2-BPSK */
    uint8_t addDmrsEnable;      /* 附加导频是否使能标志，取值0:不使能，1:使能 */
    uint8_t rsvd1;
    uint8_t dmrsSymIdx[HOP_NUM][PUC_FMT3_MAX_DMRS_NUM];
    uint8_t cyclicShift[HOP_NUM][PUC_FMT3_MAX_DMRS_NUM];    /* 38.211协议 6.3.2.2.2计算得到的各符号α值 */

    uint8_t rsvd2;
    uint8_t uciPart2Enable;
    uint8_t part1BlockNum;
    uint8_t part2BlockNum;
    Fmt3UciLlrPos part1LlrPos[MAX_PUCCH_3_BLOCK_NUM];
    Fmt3UciLlrPos part2LlrPos[MAX_PUCCH_3_BLOCK_NUM];

    uint32_t *scrambSeqAddr;    /* 加扰序列在DDR中的存放地址 */
    uint32_t *llrAddr[2];       /* DSP处理完的LLR数据的存放地址，供HAC取数进行译码，2代表2part */
}PucFmt3Param;


typedef struct
{
    /* frequency domain */
    uint16_t prbStart;      /* PRB起始索引，取值[0,274] */
    uint8_t prbSize;       /* PRB个数，取值[1,16] */

    /* time domain */
    uint8_t startSymIdx;    /* PUC所占符号中起始符号索引，取值[0,13] */
    uint8_t symNum;         /* 本跳符号个数，fmt0/2取值[1,2]，fmt1/3/4取值[4,14] */
    uint8_t dmrsSymNum;     /* 导频符号个数,取值[1-7] */
    uint8_t uciSymNum;      /* 数据符号个数,取值[1-7] */

    /* PUC data在DDR中的存放地址 */
    uint32_t *dataStartAddr;

    /* ZC基序列或PN序列在DDR中的存放地址:
       fmt0数据/fmt1数据和导频/fmt3导频使用ZC序列,某hop内,各符号天线上的ZC基序列相同;
       fmt2导频使用PN序列,数组的2个元素分别为2个符号的PN序列的地址 */
    uint32_t *baseSeqAddr;
}PucHopParam;

typedef struct
{
    uint8_t pucFormat; 	            /* PUCCH格式，取值0/1/2/3，暂不支持格式 4 */
    uint8_t rxAntNum;	            /* 基站接收天线个数，取值2/4 */
    uint8_t rsvd;
    uint8_t hopNum;
    PucHopParam hopParam[HOP_NUM];  /* 不跳频时，参数在数组0 */

    /* 各format需要的参数 */
    union{
        PucFmt0Param fmt0Param;
        PucFmt1Param fmt1Param;
        PucFmt2Param fmt2Param;
        PucFmt3Param fmt3Param;
    }formatxParam;
}PucParam;

typedef struct
{
    uint8_t deltaOffset;    /* 算法参数，抽头offset，目前固定配置为0 */ //提取出来放到CELL级，ARM确定下放哪儿
    uint8_t noiseTapNum;    /* 算法参数，参与噪声估计的时域抽头数，取值3或6 */
    uint8_t rsvd[2];
    uint32_t threshold;     /* 算法参数，DTX检测门限，取值范围待定 */
}PucFmt0AlgoParam;

typedef struct
{
    uint8_t noiseTapNum;    /* 算法参数，参与噪声估计的时域抽头数，取值3或6 */    
    uint8_t rsvd[3];
    uint32_t threshold;     /* 算法参数，DTX检测门限，取值范围待定 */
}PucFmt1AlgoParam;

typedef struct
{
    uint8_t MrcIrcFlag; /* 算法参数，MRC/IRC模式，取值0:MRC,1:IRC,2: MRC/IRC自适应 */
    uint8_t rsvd;
    uint16_t Epsilon;       /* 算法参数，MRC/IRC切换门限系数，默认值是3，定点为768，(0,16,8) */
    uint16_t beta;          /* 算法参数，对角加载系数，浮点为0.0005，定点为0x0083，(0,16,-2) */
    uint16_t segNum;        /* 算法参数，RB个数按照粒度Ruu Granularity分为seg的个数，取值范围待定 */
    uint32_t threshold;     /* 算法参数，DTX检测门限，取值范围待定 */
}PucFmt23AlgoParam;

typedef struct
{
    uint8_t pucchNum;       /* PUCCH总个数 */
	uint8_t rsvd[3];

    uint8_t fmt0Num;
    uint8_t fmt1Num;
    uint8_t fmt2Num;
    uint8_t fmt3Num;

    PucFmt0AlgoParam fmt0AlgoParam;
    PucFmt1AlgoParam fmt1AlgoParam;
    PucFmt23AlgoParam fmt2AlgoParam;
    PucFmt23AlgoParam fmt3AlgoParam;
	
	PucParam pucParam[MAX_PUCCH_NUM];   /* 所有PUCCH的参数 */
}PucchPara;

typedef struct
{
    uint8_t pduIdxInner;
    uint8_t mcsValue;       /* DSP检出的MCS的值 */
    uint8_t rsvd;
    uint8_t isDtx;          /* DTX状态：1：DTX; 0:非DTX */
    int32_t snr;            /* SNR，线性值，由ARM转成FAPI需要的单位 */
}PucchFmt0Rpt;

typedef struct
{
    uint8_t pduIdxInner;
    uint8_t rsvd[3];
    uint8_t uciBitNum;          /* UCI比特个数，取值1或2 */
    uint8_t uciDecodeValue[2];  /* UCI解调结果, 每个bit保存到uint8_t中 */
    uint8_t isDtx;              /* DTX状态：1：DTX; 0:非DTX */
    int32_t snr;                /* SNR，线性值，由ARM转成FAPI需要的单位 */
}PucchFmt1Rpt;

typedef struct
{
    uint8_t pduIdxInner;
    uint8_t rsvd[2];
    uint8_t isDtxM;     /* DSP计算的DTXm状态：1：DTX; 0:非DTX */
    int32_t snr;        /* SNR，线性值，由ARM转成FAPI需要的单位  */
}PucchFmt23Rpt;

typedef struct
{
    uint8_t pucchFmt0Num;
    uint8_t pucchFmt1Num;
    uint8_t pucchFmt23Num;
    uint8_t rsv[2];

    PucchFmt0Rpt pucchFmt0Rpt[MAX_PUCCH_FMT0_1_NUM];
    PucchFmt1Rpt pucchFmt1Rpt[MAX_PUCCH_FMT0_1_NUM];
    PucchFmt23Rpt pucchFmt23Rpt[MAX_PUCCH_FMT2_3_NUM];
}PucchRpt;

typedef struct
{
	uint8_t  pduBitmap;       					/* bit0:SR,bit1:HARQ,其他比特位清0。0:存在,1:不存在 */
    uint8_t  PucchFormat;						/* PUCCH格式,0: PUCCH Format0,1: PUCCH Format1 */
	uint8_t  UL_CQI;							/* SNR,取值范围[0,255],代表-64dB到63dB,步长0.5dB，无效值255 */
    uint8_t  rsv1;

	uint32_t Handle;  							/* ？ARM侧无法区分，待定 */
    
	uint16_t RNTI;    							/* UE的RNTI */
    uint16_t rsv2;    							
	
	uint16_t TA;								/* UE的TA值,取值范围[0,63],213协议4.2节,无效值65535 */
	uint16_t RSSI;								/* 取值范围[0,1280],步长0.1dB */
	
	SRInfoFmt01   srInfoFmt01;
	HARQInfoFmt01 harqInfoFmt01;
}PucFmt01Rpt;

typedef struct
{
	uint8_t  pduBitmap;       					/* bit0:SR,bit1:HARQ,bit2:CSI Part 1,bit3:CSI Part 2,其他比特位清0。0:存在,1:不存在 */
    uint8_t  PucchFormat;						/* PUCCH格式,0: PUCCH Format2,1: PUCCH Format3,2: PUCCH Format4 */
	uint8_t  UL_CQI;							/* SNR,取值范围[0,255],代表-64dB到63dB,步长0.5dB，无效值255 */
    uint8_t  rsv1;

	uint32_t Handle; 
    
	uint16_t RNTI;    							/* UE的RNTI */
	uint16_t TA;								/* UE的TA值,取值范围[0,63],213协议4.2节,无效值65535 */
	
    uint16_t RSSI;								/* 取值范围[0,1280],步长0.1dB */
	uint16_t rsv2;

	SRInfoFmt23    srInfoFmt234;
	
	HARQInfoFmt23  harqInfoFmt234;
	
	CSIpart1Info   csipart1Info;
	
	CSIpart2Info   csipart2Info;
}PucFmt23Rpt;

typedef struct
{
	uint8_t PucFmt01Num;
	uint8_t PucFmt23Num;
    uint8_t rsv[2];

	PucFmt01Rpt  pucFmt01Rpt[MAX_PUCCH_NUM];
	PucFmt23Rpt  pucFmt23Rpt[MAX_PUCCH_NUM];
}PucFmtRpt;

typedef struct
{
    uint8_t pucchfmtpdunum[PUCCH_FORMAT_BUTT];                                               /* 本小区pucch fmt0/1/2/3的PDU个数 */
    uint8_t pucchfmtpduIdxInner[PUCCH_FORMAT_BUTT][MAX_PUCCH_NUM];                           /* 本小区pucch fmt0/1/2/3的PDU内部索引 */

    uint8_t pucchNumpersym[SYM_NUM_PER_SLOT];               		                         /* 本小区按符号统计的PUCCH个数 */
    
    uint8_t pucchIndex[SYM_NUM_PER_SLOT][MAX_PUCCH_NUM];    		                         /* 本小区按符号统计的PUCCH索引 */

    uint8_t pucchNum;  
    uint8_t pucchpduGroupNum;                                                                /* 本小区PUCCH fmt1的组数 */                                            
    uint8_t pucchpduNumPerGroup[MAX_PUCCH_NUM];                                              /* 本小区PUCCH fmt1每组统计的PDU个数 */
    
    uint8_t pucchuserNumPerOcc[MAX_PUCCH_NUM][MAX_OCC_NUM_FMT1];                             /* 本小区PUCCH fmt1每组按OCC统计的UE个数 */
    uint8_t pucchpduIndexinGroup[MAX_PUCCH_NUM][MAX_OCC_NUM_FMT1][MAX_USER_NUM_PER_OCC];     /* 本小区PUCCH fmt1各组内各OCC值相同用户对应的PDU索引值 */

    FapiNrMsgPucchPduInfo FapiPucchPduInfo[MAX_PUCCH_NUM];       	                         /* 本小区pucch的PDU信息 */
}ArmPucParam;

typedef struct
{
    HacCfgHead          hacCfgHead;

	PolarDecodePduInfo  polarPduInfo[MAX_PUCCH_FMT23_NUM];    
}PucchPolarDecodeHacCfgPara;

typedef struct
{
    HacCfgHead          hacCfgHead;

	RMDecodePduInfo     rmPduInfo[MAX_PUCCH_FMT23_NUM];    
}PucchRMDecodeHacCfgPara;

typedef struct 
{
    uint16_t sfnIndex;                                    /* system frame number [0: 1023] */
    uint16_t slotIndex;                                   /* slot number [0: 159]  */
    uint16_t uciNum;                                      
    
    FapiNrPucchFmt01Indication fapiNrPucchFmt01Indication[MAX_PUCCH_FMT01_NUM];
}PucchFmt01Rst;

typedef struct 
{
    uint16_t sfnIndex;                                    /* system frame number [0: 1023] */
    uint16_t slotIndex;                                   /* slot number [0: 159]  */
    uint16_t uciNum;                                      
    
    FapiNrPucchFmt23Indication fapiNrPucchFmt23Indication[MAX_PUCCH_FMT23_NUM];
}PucchFmt23Rst;
