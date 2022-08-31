#pragma once
#include "../../../../common/inc/common_typedef.h"
#include "../../../../common/inc/common_macro.h"

#define MAX_RX_ANT_NUM 4
#define MAX_USER_NUM_PER_OCC 6
#define SC_NUM_PER_RB 12
#define MAX_DMRS_SYM_NUM 7
#define MAX_PUCCH_NUM 50    /* 待定 */

enum PUCCH_FORMAT
{
    PUCCH_FORMAT_0 = 0,
    PUCCH_FORMAT_1 = 1,      
    PUCCH_FORMAT_2 = 2,
    PUCCH_FORMAT_3 = 3,    
    PUCCH_FORMAT_BUTT
 };
 
typedef struct
{
	uint8_t  srFlag;       					    /* 是否SR发送时机，0:无SR，1:SR发送时机 */
    uint8_t  harqBitLength;  					/* HARQ的payload,取值[0,2] */
	uint8_t  cyclicShift[SYM_NUM_PER_SLOT]; 	/* 38.211协议 6.3.2.2.2计算得到的各符号α值，取值[0,11] */
	uint16_t rnti;							    /* UE唯一标识，取值[1,65535] */
    uint8_t  rsvd[2];
}PucFmt0Param;

typedef struct
{
	uint8_t  srFlag;          				    /* 是否SR发送时机，0:无SR，1:SR发送时机 */
    uint8_t  harqBitLength;     				/* HARQ的payload,取值[0,2] */
	uint8_t  cyclicShift[SYM_NUM_PER_SLOT]; 	/* 38.211协议 6.3.2.2.2计算得到的各符号α值，取值[0,11] */
	uint16_t rnti;	
    uint8_t  rsvd[2];
}PucFmt1UEParam;

typedef struct
{
    uint16_t ueTapBitMap[SYM_NUM_PER_SLOT];  /* 所有ue在某OFDM符号上的cycle shift(0-11)值的bitmap，如cycle shift为3，那么bit3设为1;NR_SYM_NUM_PER_SLOT为PUCCH所占符号的索引 */
    uint8_t rsvd[2];
    uint8_t timeDomainOccIdx;  				    /* 时域OCC的索引值，取值[0,6] */
    uint8_t userNumPerOcc;     				    /* 每个occ上复用的用户数，协议规定最多12个，实际最多6个，取值[1,6] */
    PucFmt1UEParam fmt1UEParam[MAX_USER_NUM_PER_OCC];/* fmt1 UE参数 */
}PucFmt1Param;

typedef struct
{
	uint8_t  srBitLen;          /* SR比特数，取值[0,4] */
    uint8_t  rsvd;
	uint16_t harqBitLength;	    /* HARQ的payload,取值[0,1706]除了1 */
    uint16_t csiPart1BitLength; /* CSI part1的payload,取值[0,1706] */
	uint16_t rnti;			    /* UE唯一标识，取值[1,65535] */
    uint32_t *scrambSeqAddr[HOP_NUM];   /* 加扰序列在DDR中的存放地址,TODO:根据HAC存放确定是否需要2个hop的首地址 */
}PucFmt2Param;

typedef struct
{
	uint8_t pi2bpsk;		    /* pi/2-BPSK是否使能标志，取值0:不使能，使用QPSK，1:使能，使用pi/2-BPSK */
	uint8_t addDmrsFlag;	    /* 附加导频是否使能标志，取值0:不使能，1:使能 */
    uint8_t rsvd;
	uint8_t srBitLen;           /* SR比特数，取值[0,4] */
	uint16_t harqBitLength;	    /* HARQ的payload,取值[0,1706]除了1 */
    uint16_t csiPart1BitLength; /* CSI part1的payload,取值[0,1706] */
	uint16_t rnti;		        /* UE唯一标识，取值[1,65535] */
	uint8_t  cyclicShift[SYM_NUM_PER_SLOT]; 	/* 38.211协议 6.3.2.2.2计算得到的各符号α值 */
    uint32_t *scrambSeqAddr[HOP_NUM];           /* 加扰序列在DDR中的存放地址，TODO:根据HAC存放确定是否需要2个hop的首地址 */
}PucFmt3Param;

typedef struct
{
    uint8_t PduIdxInner;
    uint8_t pucFormat; 	            /* PUCCH格式，取值0/1/2/3，暂不支持格式 4 */

    uint8_t rxAntNum;	            /* 基站接收天线个数，取值2/4 */
    uint8_t rsvd[3];

    /* frequency domain */
    uint16_t prbStart; 			    /* PRB起始索引，取值[0,274] */
    uint16_t prbSize; 			    /* PRB个数，取值[1,16] */
    uint8_t  intraSlotFreqHopping;  /* 时隙内跳频是否使能标志，0:不使能,1:使能 */
	uint8_t  secondHopSymIdx; 	    /* 跳频后符号索引，取值[1,13] */
    uint16_t secondHopPrb; 		    /* 跳频后起始RB索引，取值[0,274] */

    /* time domain */
    uint8_t startSymIdx;  		    /* 起始符号索引，取值[0,13] */
    uint8_t symNum; 			    /* 符号个数，fmt0/2取值[1,2]，fmt1/3/4取值[4,14] */
	uint8_t dmrsSymNum[HOP_NUM];    /* 导频符号个数,取值[1-7] */
    uint8_t uciSymNum[HOP_NUM];     /* 数据符号个数,取值[1-7] */

    /* PUC data在DDR中的存放地址 */
    uint32_t *dataStartAddr[HOP_NUM];  
    
    /* ZC基序列或PN序列在DDR中的存放地址:
       fmt0数据/fmt1数据和导频/fmt3导频使用ZC序列,某hop内,各符号天线上的ZC基序列相同;
       fmt2导频使用PN序列,数组的2个元素分别为2个符号的PN序列的地址 */
    uint32_t *baseSeqAddr[HOP_NUM]; 

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
    uint8_t MrcIrcFlag;     /* 算法参数，MRC/IRC模式，取值0:MRC,1:IRC,2: MRC/IRC自适应 */ 
    uint8_t rsvd[3];
    uint16_t beta;          /* 算法参数，对角拉齐因子，取值范围待定 */
    uint16_t segNum;        /* 算法参数，RB个数按照粒度Ruu Granularity分为seg的个数，取值范围待定 */
    uint32_t threshold;     /* 算法参数，DTX检测门限，取值范围待定 */
}PucFmt23AlgoParam;

typedef struct
{
    uint8_t pucchNum;       /* PUCCH总个数 */
	uint8_t rsvd[3];

    uint8_t fmt0Num; 
    uint8_t fmt0Idx[MAX_PUCCH_NUM]; /* 在pucParam[MAX_PUCCH_NUM]中的索引 */
    uint8_t fmt1Num; 
    uint8_t fmt1Idx[MAX_PUCCH_NUM];
    uint8_t fmt2Num; 
    uint8_t fmt2Idx[MAX_PUCCH_NUM]; /* 考虑先处理format2/3,DSP处理好后发给HAC继续处理 */
    uint8_t fmt3Num; 
    uint8_t fmt3Idx[MAX_PUCCH_NUM];
    
    PucFmt0AlgoParam fmt0AlgoParam; 
    PucFmt1AlgoParam fmt1AlgoParam; 
    PucFmt23AlgoParam fmt2AlgoParam;
    PucFmt23AlgoParam fmt3AlgoParam;
	
	PucParam pucParam[MAX_PUCCH_NUM];   /* 所有PUCCH的参数 */
}PucchPara;

typedef struct
{
    uint8_t pucchfmtpdunum[PUCCH_FORMAT_BUTT];                               /* 本小区pucch fmt0/1/2/3的PDU个数 */
    uint8_t pucchfmtpduIdxInner[PUCCH_FORMAT_BUTT][MAX_PUCCH_NUM];           /* 本小区pucch fmt0/1/2/3的PDU内部索引 */

    uint8_t pucchNumpersym[SYM_NUM_PER_SLOT];               		         /* 本小区按符号统计的PUCCH个数 */
    
    uint8_t pucchIndex[SYM_NUM_PER_SLOT][MAX_PUCCH_NUM];    		         /* 本小区按符号统计的PUCCH索引 */

    uint8_t pucchNum;  
    uint8_t pucchpduGroupNum;                                                /* 本小区PUCCH fmt1的组数 */                                            
    uint8_t pucchpduNumPerGroup[MAX_PUCCH_NUM];                              /* 本小区PUCCH fmt1每组PDU个数 */

    uint8_t pucchpduIndexinGroup[MAX_PUCCH_NUM][MAX_USER_NUM_PER_OCC];       /* 本小区PUCCH fmt1各组内用户对应的PDU索引值 */

    FapiNrMsgPucchPduInfo FapiPucchPduInfo[MAX_PUCCH_NUM];       	         /* 本小区pucch的PDU信息 */
}ArmPucParam;

typedef struct
{
    uint8_t  PduIdxInner;
    uint8_t  UL_CQI;					    /* SNR,取值范围[0,255],代表-64dB到63dB,步长0.5dB，无效值255 */
    uint8_t  rsvd[2];

    uint8_t	 SRindication;					/* SR检测结果，0:未检测到SR,1:检测到SR */
    uint8_t	 NumHarq;						/* HARQ比特个数，取值1或2 */
	uint8_t  HarqValue[2];					/* HARQ解调结果，0:ACK,1:NACK,2:DTX */
}PucchFmt01Rpt;

typedef struct
{
    uint8_t  PduIdxInner;
    uint8_t  UL_CQI;					    /* SNR,取值范围[0,255],代表-64dB到63dB,步长0.5dB，无效值255 */
    uint8_t  rsvd[2];
}PucchFmt23Rpt;

typedef struct
{
	uint8_t PucchFmt01Num;
	uint8_t PucchFmt23Num;
    uint8_t rsv[2];

	PucchFmt01Rpt pucchFmt01Rpt[MAX_PUCCH_NUM];
	PucchFmt23Rpt pucchFmt23Rpt[MAX_PUCCH_NUM];
}PucchRpt;

typedef struct
{
	uint8_t		SRindication;					/* SR检测结果，0:未检测到SR,1:检测到SR */
	uint8_t		SRconfidenceLevel;				/* SR检测置信度，0:置信,1:不置信,无效值255 */
    uint8_t     rsv[2];
}SRInfoFmt01;

typedef struct
{
	uint16_t	SrBitLen;						/* SR比特长度，取值范围[1,8] */
	uint8_t		SrPayload;						/* SR码流 */
    uint8_t		rsv;						
}SRInfoFmt234;//1DW

typedef struct
{
	uint8_t		NumHarq;						/* HARQ比特个数，取值1或2 */
	uint8_t		HarqconfidenceLevel;			/* HARQ检测置信度，0:置信,1:不置信,无效值255 */
	uint8_t 	HarqValue[2];					/* HARQ解调结果，0:ACK,1:NACK,2:DTX */
}HARQInfoFmt01;//1dw

typedef struct
{
	uint8_t		HarqCrc;						/* HARQ CRC结果，0:pass,1:fail,2:not present */
	uint8_t		rsv;
	uint16_t	HarqBitLen;						/* HARQ比特长度，取值范围[1,1706] */
	
    uint8_t 	HarqPayload[216];				/* HARQ码流 */
}HARQInfoFmt234;//55DW

typedef struct
{
	uint8_t		CsiPart1Crc;					/* CsiPart1 CRC结果，0:pass,1:fail,2:not present */
	uint8_t		rsv;
	uint16_t	CsiPart1BitLen;					/* CsiPart1比特长度，取值范围[1,1706] */
	
    uint8_t 	CsiPart1Payload[216];			/* CsiPart1码流 */
}CSIpart1Info;//55DW

typedef struct
{
	uint8_t		CsiPart2Crc;					/* CsiPart2 CRC结果，0:pass,1:fail,2:not present */
	uint8_t		rsv;
	uint16_t	CsiPart2BitLen;					/* CsiPart2比特长度，取值范围[1,1706] */
	
    uint8_t 	CsiPart2Payload[216];			/* CsiPart2码流 */
}CSIpart2Info;

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

	uint32_t Handle;  							/* ？ARM侧无法区分，待定 */
    
	uint16_t RNTI;    							/* UE的RNTI */
	uint16_t TA;								/* UE的TA值,取值范围[0,63],213协议4.2节,无效值65535 */
	
    uint16_t RSSI;								/* 取值范围[0,1280],步长0.1dB */
	uint16_t rsv2;

	SRInfoFmt234   srInfoFmt234;
	
	HARQInfoFmt234 harqInfoFmt234;
	
	CSIpart1Info   csipart1Info;
	
	CSIpart2Info   csipart2Info;
}PucFmt234Rpt;

typedef struct
{
	uint8_t PucFmt01Num;
	uint8_t PucFmt234Num;
    uint8_t rsv[2];

	PucFmt01Rpt  pucFmt01Rpt[MAX_PUCCH_NUM];
	PucFmt234Rpt pucFmt234Rpt[MAX_PUCCH_NUM];
}PucFmtRpt;
