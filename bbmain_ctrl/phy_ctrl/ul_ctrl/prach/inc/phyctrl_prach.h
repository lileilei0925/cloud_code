#pragma once
#include "../../../../common/inc/common_typedef.h"
#include "../../../../common/inc/common_macro.h"

typedef struct 
{
    uint16_t preambleFormat;  /* prach Foramt格式 */
    uint16_t raLen;           /* prach preamble length */
    uint32_t repeatTimesOcas; /* 一个occasion内的重复次数 */
    uint32_t raCp;            /* cp length */
} PrachPreambleLRA;

typedef struct 
{
    uint32_t prachScsValue;   /* prach 子载波间隔 */
    uint32_t puschScsValue;   /* pusch 子载波间隔 */
    uint16_t nRaRB;           /* prach 占的Rb数量 */
    uint16_t raKbar;          /* prach RB 内偏移值 */
} PrachRaRbAndKbar;

typedef struct 
{
    uint8_t prachConfigIdx;        /* prach 配置索引，可以从P5获取 */
    uint8_t preambleFormat[2];     /* prach Foramt 格式 */
    uint8_t startSymbol;           /* prach 的第一个TD Occasion 在slot的起始符号 */
    uint8_t occassionsInPrachSlot; /* 当前配置下的TD occasion 数 */
    uint8_t duration;              /* 每个TD Occasion 持续的符号数 */  
    uint8_t rsv[2];
} PrachConfigTable;

/* P5 Prach configuration messages local structure*/
typedef struct {	
    uint16_t prachResConfigIndex; /* A number uniquely identifying the PRACH resource configuration */
    uint8_t  prachSequenceLength; /* RACH sequence length. Long or Short sequence length. 0 = Long sequence; 1 = Short sequence */
    uint8_t  prachSubCSpacing;    /* Subcarrier spacing of PRACH. 0: 15 kHz 1: 30 kHz 2: 60 kHz 3: 120 kHz 4: 1.25 kHz 5: 5 kHz */
    uint8_t  ulBwpPuschScs;       /* The PUSCH channel subcarrier spacing of UL BWP which associated with this PRACH Configuration 0 -> 3 */
    uint8_t  restrictedSetConfig; /* PRACH restricted set config 0: unrestricted 1: restricted set type A 2: restricted set type B */
    uint8_t  prachFdOccasionNum;  /* Number of RACH frequency domain occasions */
    uint8_t  prachConfigIndex;    /* PRACH configuration index. Value: from 0 to 255 */
    uint16_t prachRootSequenceIndex[MAX_PRACH_FDM_NUM];/* Starting logical root sequence index 𝑖 equivalent to higher layer parameter prach-RootSequenceIndex Value: 0 -> 837 */
    uint8_t  rootSequenceNum[MAX_PRACH_FDM_NUM];       /* Number of root sequences for a particular FD occasion that are required to generate the necessary number of preambles */
    uint8_t  prachZeroCorrConf[MAX_PRACH_FDM_NUM];     /* PRACH Zero CorrelationZone Config which is used to dervive Ncs */
    int16_t  k1[MAX_PRACH_FDM_NUM];                    /* Frequency offset from UL bandwidth part for each FD */
    //uint16_t unUsedRootSequenceNum[MAX_PRACH_FDM_NUM];/* Not in use at present */
    //uint16_t unUsedRootSequences[MAX_PRACH_FDM_NUM][MAX_PRACH_ROOT_NUM];/* Not in use at present */
    //uint8_t  ssbPerRach; /* Not in use at present */
} L1PrachConfigInfo;

/* P7 Prach slot messages local structure */
typedef struct 
{
    uint32_t handle;            /* An opaque handle returned in the RACH.indication */
    uint16_t phyCellID;         /* 10 bits corresponding to N Cell ID */
    uint16_t prachResCfgIndex;  /* The PRACH configuration for which this PRACH PDU is signaled  */
    uint8_t  prachCfgScope;     /* 0: for PHY ID 0; 1: for current PHY ID */
    uint8_t  prachTdOcasNum;    /* Number of time-domain PRACH occasions within a PRACH slot */
    uint8_t  prachFormat;       /* RACH format information for the PRACH occasions signaled in this PDU */
    uint8_t  PrachFdmIndex;     /* Frequency domain occasion index :msg1-FDM*/   
    uint8_t  prachFdmNum;       /* Number of frequency domain occasions,starting with PrachFdmIndex */  
    uint8_t  prachStartSymb;    /* Starting symbol for the first PRACH TD occasion in the current PRACH FD occasion */
    uint16_t ncsValue;          /* Zero-correlation zone configuration number */
    uint8_t  startPreambleIndex;/* Start of preamble logical index to monitor in the PRACH occasions signaled in this slot. 0 – 63; 255:all preambles */
    uint8_t  preambleIndicesNum;/* Number of preamble logical indices, starting with startPreambleIndex, to monitor 0 – 63; 255:all preambles */
    /* Beamforming parameter */ 
    uint16_t prgNum;            /* Number of PRGs spanning this allocation */
    uint16_t prgSize;           /* Size in RBs of a precoding resource block group */  
    uint8_t  trpScheme;         /* This field shall be set to 0, to identify that this table is used */
    uint8_t  digitalBfNum;      /* Number of logical antenna ports */
    uint16_t beamIndex[];
} L1PrachPduInfo;

typedef struct 
{   
    uint16_t sfnIndex;    /* system frame number [0: 1023] */
    uint8_t  slotIndex;   /* SLOT number [0: 159] */ 
    uint8_t  prachPduNum; /* Number of PrachPdus that are parse from FAPI UlTTIRequset */
    L1PrachPduInfo l1PrachPduInfo[MAX_PRACH_PDU_NUM];   
} L1PrachParaPduInfo;

/* P5 + P7 Prach slot messages parsing structure*/
typedef struct
{
    uint8_t  tdOccasIdx;                       /* TD OCcasion index {0，1} */
    uint8_t  startSymb;                        /* 每个TD Occasion 的时域起始符号位置 */
    uint8_t  fdOccasIdx[MAX_PRACH_FDM_NUM];    /* FD OCcasion index {0，1} */
    uint16_t nK1[MAX_PRACH_FDM_NUM];           /* 根序列起始索引 */
    uint16_t rootSeqIndex[MAX_PRACH_FDM_NUM];  /* 根序列起始索引 */
    uint16_t numZcRootSeq[MAX_PRACH_FDM_NUM];  /* 根序列个数 */
    uint16_t rootSeqLength[MAX_PRACH_FDM_NUM]; /* 根序列长度 */
    uint32_t handle[MAX_PRACH_FDM_NUM];        /* 指示UL_TTI.request和RACH.indication关系 */
} PrachRxFdOcasInfo;

#if 0
typedef struct
{
    uint16_t sfnIndex;           /* system frame number [0: 1023] */
    uint8_t  slotIndex;          /* slot number [0: 159]  */
    uint8_t  cellIdx;            /* L1与L2之间的小区标识*/     
    uint8_t  rxAntNum;           /* 接收天线数 */
    uint8_t  restrictedSetType;  /* 0: unrestricted 1: restricted set type A 2: restricted set type B */
    uint8_t  puschScs;
    uint8_t  prachFormat;        /* RACH format information for the PRACH occasions signaled in this PDU */
    uint8_t  prachRaLength;      /* Long Or Short PRACH; 0:839; 1:139 */ 
    uint16_t bandWidthUl;        /*  */
    uint16_t prachResCfgIdx;
    uint8_t  prachScs;
    uint8_t  nRaRB;
    uint8_t  raKbar;
    uint32_t prachScsValue;
    uint32_t puschScsValue; 
    uint32_t downSampleValue;
    uint16_t prachConfigIndex;
    uint16_t nNcs;                    
    uint16_t nCpLen;
    uint16_t nfftSize;                          /* FFT 点数 */
    uint16_t prachZcSize;                       /* 839 Or 139 */
    uint8_t  repeatTimesInOcas;                 /* 同一个Occasion 内的重复次数 */
    uint8_t  numTdOccas;                        /* 时域上的 PRACH Occasion数 */
    uint8_t  numFdOccas;                        /* 频域域上的 PRACH Occasion数 */
    PrachRxFdOcasInfo prachRxFdOcasInfo[MAX_PRACH_TDM_NUM];
} PrachRxParaLocal;
#endif

/* PrachLowPhyPara structure: prach LowphyPara HAC para */
typedef struct
{
    uint32_t handle;          /* 指示UL_TTI.request和RACH.indication关系 */
    int32_t  prachPhaseStep;  /* 每个FD OCcasion 的移频值 */
    uint32_t *outPutIQBuffAddr;/* 每个FD occasion Index 对应的IQ数据存放的数据起始地址 */ 
    uint32_t addrOffsetPerAnt;/* 每个prach symbol的每一个天线的起始地址 startAddr = outPutIQBuffAddr + addrOffsetPerAnt*antIndx + addrOffsetPerAnt*AntNum*RepeatSymIdx */
} FdOcasInfoLowPhy;

typedef struct
{
    uint16_t sfnIndex;             /* system frame number [0: 1023] */
    uint8_t  slotIndex;            /* slot number [0: 159]  */
    uint8_t  cellIdx;              /* L1与L2之间的小区标识*/     
    uint8_t  rxAntNum;             /* 天线数 */
    uint8_t  prachLowPhyEn;        /* Lowphy使能开关 */
    uint8_t  prachFormat;          /* Prach Format格式 */
    uint8_t  vgaCompEn;            /* VGA补偿 */
    uint8_t  agcUintDb;            /* agc分辨率 */
    uint8_t  agcAutoSelect;        /* 自动手动选择 0:手动， 1：自动*/
    uint8_t  gainManualRatio;      /* 手动放大倍数 */
    uint8_t  downSamplingEnBitMap; /* 6级半带滤波器级联；优先级bit位从高到低；bit0：CoefHBF1；0：Disable; 1: Enable */
    uint8_t  timeDagcEn;           /* 时域DAGC开关 */           
    int8_t   tdAgcTarget;          /* 目标AGC因子，算法配置值 */
    uint8_t  targetFFTSize;        /* FFT点数，0：256,1:1536 */     
    uint8_t  fftIfftFlag;          /* 0:DFT/FFT, 1:IDFT/IFFT */
    uint8_t  raLengthFlag;         /* 序列长度指示，0:839,1:139 */
    uint8_t  rssiCalcEn;           /* Rssi计算使能 */
    uint8_t  startSymbol;          /* 第一个Occasion的起始符号 */
    uint8_t  tdOccasNum;           /* 时域occasion */
    uint8_t  repeatTimesInOcas;    /* Occasion内的重复次数，1,2,4,6，12 */
    uint8_t  fdOccasNum;           /* 时域occasion */
    uint16_t rsv;
    FdOcasInfoLowPhy fdOcasInfoLowPhy[MAX_PRACH_FDM_NUM]; /* 每个slot内包含的FD OCcasion 的起始位置信息*/
} PrachLowPhyHacPara;

/* PrachPreProcPara structure: Prach PreProcessing HAC para */
typedef struct
{
    uint32_t handle;           /* link to report */
    uint16_t rootSeqIndex;     /* 根序列起始索引 */
    uint16_t rootSeqNum;       /* 根序列个数 */
    uint32_t *inPutIqDataAddr; /* 每个FD occasion Index 对应的IQ数据存放的数据起始地址 */ 
    uint32_t inPutOffsetPerAnt;/* 每个prach symbol的每一个天线的起始地址 startAddr = outPutIQBuffAddr + addrOffsetPerAnt*antIndx + addrOffsetPerAnt*AntNum*RepeatSymIdx */
    uint32_t *outPutPdpAddr;   /* 每个FD下每个TD Occasion的每个根序列输出一个合并后的PDP数据*/
    uint32_t outPutPdpOffset;  /* 符号合并输出的地址偏移 */
} PrachFdOcasRpp;

typedef struct
{
    uint16_t sfnIndex;         /* system frame number [0: 1023] */
    uint8_t  slotIndex;        /* slot number [0: 159]  */
    uint8_t  cellIdx;          /* L1与L2之间的小区标识*/
    uint8_t  rxAntNum;         /* Receive Antenna Number */
    uint8_t  prachLength;      /* 0：Long；1：short */
    uint8_t  ifftSize;         /* 0：256；1：1536 */
    uint8_t  repeatNumPerOcas; /* 每个Occasion内重复的symb数 */
    uint8_t  symbComEn;        /* 符号合并使能 0:disable; 1:enable */
    uint8_t  symbNumForComb;   /* 需要合并的符号数 */
    uint8_t  tdOccasNum;       /* occasion 数量 TDM * TDOccasion */
    uint8_t  fdOccasNum;       /* occasion 数量 FDM * FDOccasion */
    uint32_t *rootSeqBaseAddr; /* 根序列基地址，DDR预存，139和839序列各一个地址 */
    PrachFdOcasRpp prachFdOcasRpp[MAX_PRACH_FDM_NUM];
} PrachRPPHacPara;

/* Dsp Para structure: Prach PreProcessing HAC para */
typedef struct
{
    uint16_t maxWinLength;                     /*root 内同一类型的最大窗长*/
    uint16_t rsv;
    int16_t  winStart[PRACH_PREAMBLE_SEQ_NUM]; /* window start point */ 
    int16_t  winEnd[PRACH_PREAMBLE_SEQ_NUM];   /* window end point */
    int16_t  winLen[PRACH_PREAMBLE_SEQ_NUM];   /*每个window的长度 = start - end*/
} NrPrachDetWin;

typedef struct
{
    uint16_t preambleNum;                  /* 每个pdp根内需检测的preamble个数，也即检测窗的个数 */
    uint16_t rsv;
    NrPrachDetWin detWin[MAX_DET_WIN_NUM]; /* non restrict set has only one main window restrict A set has 3 */
} NrPrachRootDetWin;

typedef struct
{
    uint32_t handle;        /* link to report */
    uint16_t excursionNum;  /* excurted point number in detect window */
    uint16_t zcRootSeqNum;  /* pdp 根的个数*/
    uint16_t *pdpSeqAddr;   /* 每个FD Occasion的首地址*/
    uint16_t pdpSeqOffset;  /* 每个FD Occasion下每个TD Occasion的偏移*/
    NrPrachRootDetWin prachRootDetWin[MAX_ROOT_PER_OCCASION]; /* 每个pdp 根的搜索窗配置*/
} NrPrachFdOccasPara;

typedef struct
{
    uint8_t  restrictA;    /* 当前cell是否配置restrictA */
    uint8_t  tdOccasNum;   /* time domain Occasion num */
    uint8_t  fdOccasNum;   /* frequency domain Occasion num */
    uint8_t  ulBwpPuschScs;/* 当前cell pusch 子载波间隔*/
    uint16_t thAlpha;      /* threshold alpha, 0.6 ->  0x4ccd*/
    uint16_t thSingleWin;  /* 单搜索窗噪声过滤门限*/
    uint16_t thMultiWin;   /* 多搜索窗噪声过滤门限*/
    uint16_t nFftSize;     /* FFT num */
    uint16_t zcSeqSize;    /* Zc length */
}NrPrachCellCfgPara;

typedef struct
{
    uint16_t sfnIndex;     /* system frame number [0: 1023] */
    uint8_t  slotIndex;    /* slot number [0: 159]  */
    uint8_t  cellIdx;      /* L1与L2之间的小区标识*/
    NrPrachCellCfgPara prachCellCfgPara; /* 小区级参数配置 */
    NrPrachFdOccasPara prachFdOccasPara[MAX_PRACH_FDM_NUM];  /* 每个fdm内  pdp根的地址配置 */
} PrachDetectDspPara;

/* DSP output*/
typedef struct
{
    uint32_t handle;                                   /* link  to  report */
    uint8_t  avgSnr;                                   /* Average value of SNR in dB */
    uint8_t  numPreambles;                             /* Number of detected preambles in the PRACH occasion */
    uint16_t lowPhyAgc;                                /*lowPhy透传的agc因子*/
    uint8_t  preambleIndex[PRACH_PREAMBLE_SEQ_NUM];    /* Preamble Index */
    uint16_t preambleSnrBase[PRACH_PREAMBLE_SEQ_NUM];  /* Preamble SNR 整数部分 */
    uint16_t preambleSnrScale[PRACH_PREAMBLE_SEQ_NUM]; /* preamble SNR 定标小数位宽*/
    uint16_t timingAdvance[PRACH_PREAMBLE_SEQ_NUM];    /* Timing advance for PRACH */
    uint16_t preamblePwr[PRACH_PREAMBLE_SEQ_NUM];      /* Preamble Received power,定标(0, 16, 3) */
} NrPrachOccasResult;

typedef struct
{
    uint16_t sfnIndex;    /* system frame number [0: 1023] */
    uint8_t  slotIndex;   /* slot number [0: 159] */
    uint8_t  cellIdx;     /* L1与L2之间的小区标识*/
    uint8_t  detectedOccasionNum;/*occasion num that has detected preamble*/
    uint8_t  detectedIndex[MAX_PRACH_OCCASION_NUM][2];   /*occasion's index which has detected preamble,first is tdm idx, second is fdm idx)*/
    NrPrachOccasResult prachDspDetection[MAX_PRACH_TDM_NUM][MAX_PRACH_FDM_NUM];
} NrPrachDetectResult;