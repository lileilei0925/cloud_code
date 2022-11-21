#pragma once
#include "../../../../common/inc/common_typedef.h"
#include "../../../../common/inc/common_macro.h"

/* P5 Prach configuration messages local structure*/
typedef struct {	
    uint16_t prachResConfigIndex;
    uint8_t  prachSequenceLength;
    uint8_t  prachSubCSpacing;
    uint8_t  ulBwpPuschScs;      
    uint8_t  restrictedSetConfig;
    uint8_t  numPrachFdOccasions;
    uint8_t  prachConfigIndex;
    uint16_t prachRootSequenceIndex[MAX_PRACH_FDM_NUM];
    uint8_t  numRootSequences[MAX_PRACH_FDM_NUM];
    uint8_t  prachZeroCorrConf[MAX_PRACH_FDM_NUM];
    int16_t  k1[MAX_PRACH_FDM_NUM];
    uint16_t numUnusedRootSequences[MAX_PRACH_FDM_NUM];
    uint16_t unusedRootSequences[MAX_PRACH_FDM_NUM][MAX_PRACH_ROOT_NUM];
    uint8_t  ssbPerRach;
    uint8_t  rsv[3];
}L1PrachConfigInfo;

/* P7 Prach slot messages local structure*/
typedef struct 
{
    uint32_t handle;
    uint16_t pduIndex;
    uint16_t phyCellID;
    uint8_t  prachTdOcasNum;
    uint8_t  prachFormat;
    uint8_t  PrachFdmIndex;   /* Frequency domain occasion index :msg1-FDM*/     
    uint8_t  prachStartSymb;  /* Starting symbol for the first PRACH TD occasion in the current PRACH FD occasion */
    uint16_t ncsValue;        /* Zero-correlation zone configuration number */
    uint16_t prachResCfgIndex;/* The PRACH configuration for which this PRACH PDU is signaled  */
    uint8_t  prachCfgScope;   /* 0: for PHY ID 0; 1: for current PHY ID */
    uint8_t  prachFdmNum;     /* Number of frequency domain occasions,starting with PrachFdmIndex */
    uint8_t  startPreambleIndex;
    uint8_t  preambleIndicesNum;
    uint16_t prgNum;
    uint16_t prgSize;
    uint8_t  trpScheme; /* This field shall be set to 0, to identify that this table is used */
    uint8_t  digitalBfNum;/* Number of logical antenna ports */
    uint8_t  rsv[2];
    uint16_t beamIndex[MAX_PRG_NUM][MAX_BF_PORT];
} L1PrachPduInfo;

typedef struct 
{   
    uint16_t       sfnIndex;        /* system frame number [0: 1023] */
    uint8_t        slotIndex;       /* SLOT number [0: 159] */ 
    uint8_t        prachPduNum;   /* Number of PrachPdus that are parse from FAPI UlTTIRequset */
    L1PrachPduInfo l1PrachPduInfo[MAX_PRACH_PDU_NUM];   /* FDM为2时 可以有2个prach PDU*/
} L1PrachParaPduInfo;

/* P5 + P7 Prach slot messages parsing structure*/
typedef struct
{
    uint8_t  tdOccasIdx;                         /* TD OCcasion index {0，1} */
    uint8_t  startSymb;                          /* 每个TD Occasion 的时域起始符号位置 */
    uint8_t  fdOccasIdx[MAX_PRACH_FDM_NUM];      /* FD OCcasion index {0，1} */
    uint16_t nK1[MAX_PRACH_FDM_NUM];             /* 根序列起始索引 */
    uint16_t rootSeqIndex[MAX_PRACH_FDM_NUM];    /* 根序列起始索引 */
    uint16_t numZcRootSeq[MAX_PRACH_FDM_NUM];    /* 根序列个数 */
    uint16_t rootSeqLength[MAX_PRACH_FDM_NUM];   /* 根序列长度 */
    uint32_t handle[MAX_PRACH_FDM_NUM];          /* 指示UL_TTI.request和RACH.indication关系 */
}PrachRxFdOcasInfo;

typedef struct
{
    uint16_t sfnIndex;                            /* system frame number [0: 1023] */
    uint8_t  slotIndex;                           /* slot number [0: 159]  */
    uint8_t  cellIdx;                           /* L1与L2之间的小区标识*/     
    uint8_t  rxAntNum;                          /* 接收天线数 */
    uint8_t  restrictedSetType;                 /* 0: unrestricted 1: restricted set type A 2: restricted set type B */
    uint8_t  prachFormat;                       /* */
    uint8_t  prachRaLength;                     /* Long Or Short PRACH; 0:839; 1:139 */ 
    uint16_t bandWidthUl;
    uint16_t prachResCfgIdx;
    uint8_t  prachScs;
    uint8_t  puschScs;
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
    uint8_t  rsv[3];
    PrachRxFdOcasInfo prachRxFdOcasInfo[MAX_PRACH_TDM_NUM];
} PrachRxParaLocal;

/* PrachLowPhyPara structure: prach LowphyPara HAC para */
typedef struct
{
    uint8_t  tdOccasIdx;                         /* TD OCcasion index {0，1} */
    uint8_t  startSymb;                          /* 每个TD Occasion 的时域起始符号位置 */
    uint8_t  fdOccasIdx[MAX_PRACH_FDM_NUM];      /* FD OCcasion index {0，1} */
    int32_t  prachPhaseStep[MAX_PRACH_FDM_NUM];  /* 每个FD OCcasion 的移频值 */
    uint32_t handle[MAX_PRACH_FDM_NUM];          /* 指示UL_TTI.request和RACH.indication关系 */
    uint32_t outPutIQBuffAddr[MAX_PRACH_FDM_NUM];/* TD occasion m下每个FD occasion Index 对应的IQ数据存放的数据起始地址 */ 
    uint16_t addrOffsetPerAnt[MAX_PRACH_FDM_NUM];/* 每个prach symbol的每一个天线的起始地址 startAddr = outPutIQBuffAddr + addrOffsetPerAnt*antIndx + addrOffsetPerAnt*AntNum*RepeatSymIdx */
}TdFdOcasInfoLowPhy;

typedef struct
{
    uint16_t sfnIndex;                            /* system frame number [0: 1023] */
    uint8_t  slotIndex;                           /* slot number [0: 159]  */
    uint8_t  cellIdx;                           /* L1与L2之间的小区标识*/     
    uint8_t  prachFeEn;                         /* PrachLowphy Enable Flag. 0: Disable 1: Enable */ 
    uint8_t  fftShifEn;                         /* FFTshit Enable Flag. 0：Disable; 1: Enable */
    uint8_t  fdRssiEn;                          /* Prach FD RSSI 测量使能，按天线输出每个天线上的频域RSSI测量值 */
    uint8_t  freqShiftEn;                       /* Frequency shift Enable Flag. 0：Disable; 1: Enable */
    uint8_t  vgaCompEn;                         /* VGA补偿 */
    uint8_t  cpRemoveEn;                        /* 去Cp Enable Flag， 0：Disable; 1: Enable */
    uint8_t  tdAgcCompEn;                       /* 时域AGC调整Enable Flag， 0：Disable; 1: Enable */
    uint8_t  outIQAlignEn;                      /* 839,139序列，是否需要64bit，128bit对齐，或 12SC对齐 */  
    uint8_t  rxAntNum;                          /* 天线数 */
    uint8_t  longOrShortRa;                     /* Long Or Short PRACH; 0:839; 1:139 */  
    int8_t   tdAgcTarget;                       /* 需要调整的AGC因子，btarget */
    uint8_t  repeatTimesInOcas;                 /* 同一个Occasion 内的重复次数 */
    uint16_t cutCpLen;                          /* 应该去掉的CP值，Arm计算获取*/
    uint16_t targetFFTSize;                     /* FFT size index. 0: 128; 1: 256; 2: 512; 3: 1024; 4: 2048; 5: 4096; 8: 192; 9:384; 10: 768; 11: 1536; 12: 3072 */  
    uint8_t  downSamplingEnBitMap;              /* 7级半带滤波器级联；优先级bit位从高到低；bit0：CoefHBF1；0：Disable; 1: Enable*/
    uint8_t  numTdOccas;                        /* 时域上的 PRACH Occasion数 */
    uint8_t  numFdOccas;                        /* 频域域上的 PRACH Occasion数 */
    uint8_t  rsv;
    TdFdOcasInfoLowPhy tdFdOcasInfoLowPhy[MAX_PRACH_TDM_NUM]; /* 每个slot内包含的TD OCcasion 以及 FD OCcasion 的起始位置信息*/
} PrachLowPhyHacPara;

/* PrachPreProcPara structure: Prach PreProcessing HAC para */
typedef struct
{
    uint32_t handle;             /* link  to  report */
    uint8_t  startSymb;          /*  */
    uint16_t rootSeqIndex;       /* 根序列起始索引 */
    uint16_t numZcRootSeq;       /* 根序列个数 */
    uint32_t inputZcBuffAddr;    /* 第一个根序列存放的数据起始地址 */
    //uint32_t IQbufferAddrIn;   /* 第一个PRACH频域IQ数据的起始地址 */
    //uint32_t AddrOffsetPerANt; /* 每个symbol每个天线的起始地址 */
    uint32_t symbComOutputAddr;  /* 符号合并结果输出首地址，每个prach时机的每个根序列输出一个合并后的PDP数据*/
    uint32_t SymbComOutputOffset;/* 符号合并输出的地址偏移 */
} PrachTdFdOcasRpp;

typedef struct
{
    uint16_t sfnIndex;             /* system frame number [0: 1023] */
    uint8_t  slotIndex;            /* slot number [0: 159]  */
    uint8_t  cellIdx;            /* L1与L2之间的小区标识*/
    uint8_t  rxAntNum;           /* Receive Antenna Number */
    uint8_t  prachLength;        /* 0：Long；1：short */
    uint8_t  ifftSize;           /* 0：256；1：1536 */
    uint8_t  numRepeatPerOcas;   /* 每个Occasion内重复的symb数 */
    uint8_t  symbComEn;          /* 符号合并使能 0:disable; 1:enable */
    uint8_t  numSymbForComb;     /* 需要合并的符号数 */
    uint8_t  symbComOutputEn;    /* 符号合并结果输出使能, 0：disable；1：enable*/
    uint8_t  numTdOccas;         /* occasion 数量 TDM * TDOccasion */
    uint8_t  numFdOccas;         /* occasion 数量 FDM * TDOccasion */
    uint8_t  rsv[3];
    PrachTdFdOcasRpp prachTdFdOcasRpp[MAX_PRACH_TDM_NUM][MAX_PRACH_FDM_NUM];
} PrachRPPHacPara;

typedef struct
{
    uint32_t pdpSeqAddr;                             /* 天线合并后PDP数据， RPP HAC中输出的每个occasion的PDP地址要与这里的时域&频域occasion输入PDP数据一一对应 */
    uint16_t pdpSeqPerZcOffSet[MAX_PRACH_ROOT_NUM];  /* 天线合并后的时域数据每个root sequence 数据的地址偏移值 */
    uint32_t handle[MAX_PRACH_FDM_NUM];              /* link  to  report */
    uint16_t numZcRootSeq[MAX_PRACH_FDM_NUM];        /* Zc根序列个数 */
    uint16_t fdmAddrOffset[MAX_PRACH_FDM_NUM];
} PrachTdOcasDsp;

typedef struct
{
    uint16_t sfnIndex;             /* system frame number [0: 1023] */
    uint8_t  slotIndex;            /* slot number [0: 159]  */
    uint8_t  cellIdx;            /* L1与L2之间的小区标识*/
    uint8_t  rxAntNum;           /* Receive Antenna Number */
    uint8_t  numWin;             /* 检测窗个数 */
    uint16_t nFftSzie;           /* FFT点数 */
    uint16_t nNcs;               /* 循环移位值Ncs */
    uint16_t nCv;                /* 循环偏移值 */
    uint16_t winStartEnd[3][2];  /* 非限制级只有1个窗，限制级A 3个窗和参数numWin相关；每个窗内第一个值为Start，第二值个为End */
    int32_t  thAlpha;            /* 门限系数 alpha*/
    int32_t  thSingleWin;        /* 单搜索窗检测门限 */
    int32_t  thMultieWin;        /* 多搜索窗检测门限 */
    uint32_t thA;                /* 绝对门限 */
    uint32_t thR;                /* 相对门限*/
    uint16_t zcSeqSize;          /* Zc序列长度 */
    uint16_t numExcursion;       /* 搜索窗内丢弃的点数 */
    uint8_t  numTdOCas;          /* 时域Occasion 数量 */
    uint8_t  rsv;
    uint16_t numFdmOcas;         /* 频域Occasion数量 */
    PrachTdOcasDsp prachTdOcasDsp[MAX_PRACH_TDM_NUM];
} PrachDetectDspPara;

typedef struct
{
    uint32_t handle;             /* link  to  report */
    uint32_t pdpSeqAddr;         /* 天线合并后PDP数据， RPP HAC中输出的每个occasion的PDP地址要与这里的时域&频域occasion输入PDP数据一一对应 */
    uint16_t pdpSeqPerZcOffSet;  /* 天线合并后的时域数据每个root sequence 数据的地址偏移值 */
} PrachTdOcasDsp2;

typedef struct
{
    uint16_t sfnIndex;             /* system frame number [0: 1023] */
    uint8_t  slotIndex;            /* slot number [0: 159]  */
    uint8_t  cellIdx;            /* L1与L2之间的小区标识*/
    uint16_t thAlpha;            /* 噪声门限系数alpha */
    int32_t  thSingleWin;        /* 单搜索窗检测门限 */
    int32_t  thMultieWin;        /* 多搜索窗检测门限 */
    uint8_t  numWin;             /* 检测窗个数 */
    uint16_t nFftSzie;           /* FFT点数 */
    uint16_t maxMainWinLength;   /* 算法中间参数，计算主副窗合并使用 */
    uint16_t maxLeftWinlength;   /* 算法中间参数，计算主副窗合并使用 */
    uint16_t maxRightWinLength;  /* 算法中间参数，计算主副窗合并使用 */
    uint8_t  zcRootSeqNum;       /* 根序列个数，长序列最大22个，短序列最大32个 */
    uint8_t  preambleNumPerRoot; /* 每个根序列包含的需要检测的preamble个数 */
    uint16_t winStartAndLength[64][3][2];/* 检测窗起始+长度，64个检测窗，每个检测窗包含3个子检测窗，1个主窗+1个左副窗+1个右副窗，每个窗包含起始和长度2个参数 */
    uint8_t  ulBwpPuschScs;      /* pusch信道的子载波间隔，μ值，计算TA使用 */
    uint8_t  numExcursion;       /* 搜索窗内丢弃的点数 */
    uint16_t zcSeqSize;          /* Zc序列长度 */
    uint8_t  numTdOCas;          /* 时域Occasion 数量 */
    uint16_t numFdmOcas;         /* 频域Occasion数量 */
    PrachTdOcasDsp2 prachTdOcasDsp[MAX_PRACH_TDM_NUM][MAX_PRACH_FDM_NUM];
} PrachDetectDspPara2;

typedef struct 
{
    uint16_t uwPreambleFormat;
    uint16_t uwRALen;
    uint8_t  repeatTimesOcas;
    uint8_t  rsv[3];
    uint32_t udRaCp;
} PrachPreambleLRA;

typedef struct 
{
    uint32_t prachScsValue;
    uint32_t puschScsValue; 
    uint8_t  nRaRB;
    uint8_t  raKbar;
    uint8_t  rsv[2];
} PrachRaRbAndKbar;

typedef struct 
{
    uint8_t prachConfigIdx;
    uint8_t preambleFormat[2];
    uint8_t uwStartSym;
    uint8_t occassionsInPrachSlot;
    uint8_t duration;
    uint8_t rsv[2];
} PrachConfigTable;

typedef struct 
{
    uint8_t numPdu;
    uint8_t rsv;
    uint8_t handle[MAX_PRACH_FDM_NUM];
} PrachPduHandle;


/* DSP输出接口 */
typedef struct 
{
    uint8_t  avgRssi;                                    /* Average value of RSSI in dB */
    uint8_t  avgSnr;                                     /* Average value of SNR in dB */
    uint8_t  numPreambles;                               /* Number of detected preambles in the PRACH occasion */
    uint8_t  rsv;
    uint8_t  preambleIndex[PRACH_PREAMBLE_SEQ_NUM];      /* Preamble Index */
    uint16_t timingAdvance[PRACH_PREAMBLE_SEQ_NUM];      /* Timing advance for PRACH */
    uint32_t preamblePwr[PRACH_PREAMBLE_SEQ_NUM];        /* Preamble Received power in dBm */
    uint8_t  preambleSnr[PRACH_PREAMBLE_SEQ_NUM];        /* Preamble SNR in dB */
} PrachDspDetection;

typedef struct 
{
    uint16_t sfnIndex;             /* system frame number [0: 1023] */
    uint8_t  slotIndex;            /* slot number [0: 159]  */
    uint8_t  cellIdx;            /* L1与L2之间的小区标识*/
    PrachDspDetection prachDspDetection[MAX_PRACH_TDM_NUM][MAX_PRACH_FDM_NUM];
} PrachDspCalcOut;
