#pragma once
#include "../../../../common/inc/common_typedef.h"
#include "../../../../common/inc/common_macro.h"

typedef struct 
{
    uint16_t ptrsPortIndex;     /* PT-RS antenna ports index */
    uint8_t  ptrsDmrsPort;      /* DMRS port corresponding to PTRS */
    uint8_t  ptrsReOffset;      /* PT-RS resource element offset value taken from */
} PtrsPortPara;

typedef struct
{
    uint8_t  rvIndex;            /* Redundancy version index Value : 0->3*/
    uint8_t  harqProcessId;      /* HARQ process number Value: 0 ->15*/
    uint8_t  newData;            /* Value: 0: retransmission, 1: new data, i.e. initial transmission*/
    uint32_t tbSize;            /* Transmit block size */
    uint32_t cbNum;              /* Number of CBs in the TB; Should be set to zero in any of the following conditions: 1) CBG is not supported 2) newData=1 (new transmission) 3) tbSize=0 */
    uint8_t  cbPresentAndPos[4]; /* CB is present in the current retx of the PUSCH. 1=PRESENT, 0=NOT PRESENT. uint8_t[ceil(cbNum/8)] 暂不支持CBG传输，所以cbNum为0 */
} PuschDataPara;

typedef struct 
{
    uint16_t harqAckBitLength;   /* Number of HARQ-ACK bits. Value: 0 -> 11 (Small block length). 12 ->1706 (Polar) */
    uint16_t csiPart1BitLength;  /* Number of CSI part1 bits. Value: 0 -> 11 (Small block length). 12 ->1706 (Polar) */
    uint16_t flagCsiPart2;       /* Number of CSI part2 bits. 0 -> No CSI part 2*/
    uint8_t  alphaScaling;       /* Alpha used to calculate number of coded modulation symbols per layer,Value:0 = 0.5,1 = 0.65,2 = 0.8,3 = 1 */
    uint8_t  betaOffsetHarqAck;  /* Beta Offset for HARQ-ACK bits. Value: 0->15 */
    uint8_t  betaOffsetCsi1;     /* Beta Offset for CSI-part1 bits.Value: 0->18 */
    uint8_t  betaOffsetCsi2;     /* Beta Offset for CSI-part2 bits.Value: 0->18 */
} PuschUciPara;

typedef struct 
{
    uint8_t      ptrsPortsNum;                /* Number of UL PTRS ports Value: 1->2 */
    PtrsPortPara ptrsPortInfo[PTRS_PORT_NUM]; 
    uint8_t      ptrsTimeDensity;             /* PT-RS time density.Value: 0: 1, 1: 2, 2: 4 */
    uint8_t      ptrsFreqDensity;             /* PT-RS frequency density. Value: 0: 2, 1: 4 */
    uint8_t      ulPtrsPower;                 /* PUSCH to PT-RS power ratio per layer per RE. Value: 0: 0dB,1: 3dB,2: 4.77dB,3: 6dB*/
} PuschPtrsPara;

typedef struct 
{
    uint8_t  lowPaprGrpNum;          /* Group number for Low PAPR sequence generation.For DFTS-OFDM */
    uint16_t lowPaprSeqNum;          /* sequence number of Low PAPR sequence.  For DFTS-OFDM */
    uint8_t  ulptrsSampleDensity;    /* Number of PTRS groups. */
    uint8_t  ulptrsTimeDensity;      /* Number of samples per PTRS group.*/
} PuschDftOfdmPara;

typedef struct 
{
    uint16_t priority;                          /* Priority of the part 2 report */
    uint8_t  part1ParamsNum;                    /* Number of Part 1 parameters that influence the size of this part 2; values [1:4] in FAPI v3*/
    uint16_t paramOffsets[MAX_PART1_PAPR_NUM];  /* Ordered list of parameter offsets (offset from 0 = first bit of part1); The real size is part1ParamsNum */
    uint8_t  paramSizes[MAX_PART1_PAPR_NUM];    /* Bitsizes of part 1 param in the same order as paramOffsets; The real size is part1ParamsNum */
    uint16_t part2SizeMapIndex;                 /* Index of one of the maps configured Table 3-40, for determining the size of a part2, from the part 1 parameter values */
} Part2ReportPara;

/* Uci information for determining UCI Part1 to Part2 correspondence, added in FAPIv3 */
typedef struct 
{
    uint16_t        part2sNum;       /* Max number of UCI part2 that could be included in the CSI report. Value: 0 -> 100 */
    Part2ReportPara part2ReportPara[MAX_CSI_PART2_REPOET_NUM]; 
} Part2InfoAddInV3;

typedef struct
{
	uint16_t pduIndex;          
    uint16_t pduBitMap;               /* Bit 0: puschData; Bit 1:puschUci; Bit 2: puschPtrs; Bit 3: dftsOfdm; All other bits reserved*/
    uint16_t ueRnti;                  /* The RNTI used for identifying the UE when receiving the PDU*/
    uint32_t handle;                  /* An opaque handling returned in the Rx_Data.indication and/or UCI.indication message*/
    /*BWP*/
    uint16_t bwpSize;                 /* Number of contiguous PRBs allocated to the BWP */
    uint16_t bwpStart;                /* This field signals the bandwidth part start RB index from reference CRB*/
    uint8_t  subCarrierSpacing;       /* Value: 0-4*/
    uint8_t  cyclicPrefix;            /* Cyclic prefix type */
    /*PUSCH information always included*/
    uint16_t targetCodeRate;          /* This is the number of information bits per 1024 coded bits expressed in 0.1 bit units */
    uint8_t  qamModOrder;             /* Value: 2,4,6,8 if transform precoding is disabled; Value: 1,2,4,6,8 if transform precoding is enabled */
    uint8_t  mcsIndex;                /* MCS index, Value: 0-31 */
    uint8_t  mcsTable;                /* 0:notqam256; 1:qam256; 2:qam64LowSE; 3:notqam256-withTransformPrecoding; 4:qam64LowSE-withTransformPrecoding */
    uint8_t  transformPrecoding;      /* Value: 0: enabled; 1: disabled*/
    uint16_t nIdPusch;                /* parameter nID */
    uint8_t  nrOfLayers;              /* Number of layers. Value: 1-4 */
    /*DMRS*/
    uint16_t ulDmrsSymbPos;           /* Bitmap occupying the 14 LSBs.  bit 0: first symbol and for each bit. 0: no DMRS, 1: DMRS */
    uint8_t  dmrsCfgType;             /* UL DMRS config type. 0: type 1, 1: type 2 */
    uint16_t dmrsScrambleId;          /* PUSCH DMRS Scrambling-ID. It is only valid when the transform precoding for PUSCH is disabled */
    uint16_t puschDmrsId;             /* PUSCH DMRS ID */
    uint8_t  nSCID;                   /* DMRS sequence initialization, It is only valid when the transform precoding for PUSCH is disabled */
    uint8_t  numCdmGrpsNoData;        /* Number of DM-RS CDM groups without data */
    uint16_t dmrsPorts;               /* Bitmap occupying the 12 LSBs with: bit 0: antenna port 1000, bit 11: antenna port 1011,and for each bit. 0: DMRS port not used,1: DMRS port used*/
    /*Pusch Allocation in frequency domain*/
    uint8_t  resourceAlloc;           /* Resource Allocation Type. 0: Type 0, 1: Type 1 */
    uint8_t  rbBitmap[36];            /* For resource allocation type 0. This bitmap is in units of VRBs. LSB of byte 0 of the bitmap represents VRB0 */
    uint16_t rbStart;                 /* For resource allocation type 1. The starting resource block corresponds to VRB0 for this PUSCH */
    uint16_t rbSize;                  /* For resource allocation type 1. The number of resource block within for this PUSCH */
    uint8_t  vrbToPrbMapping;         /* VRB-to-PRB-mapping. Value: 0: non-interleaved */ 
    uint8_t  intraSlotHopping;        /* For resource allocation type 1. Value: 0: disabled 1: enabled*/
    uint16_t txDCLocation;            /* The uplink Tx Direct Current location for the carrier */
    uint8_t  ulFreqShift7p5Khz;       /* Indicates whether there is 7.5 kHz shift or not. Value: 0:false 1:true */
    /*Resource Allocation in time domain*/
    uint8_t  startSymbIndex;          /* Start symbol index of PUSCH mapping from the start of the slot */
    uint8_t  nrOfSymbols;             /* PUSCH duration in symbols */
    /* Beamforming strcture*/
    uint8_t  trpScheme;               /* This field shall be set to 0, to identify that this table is used */
    uint16_t prgNum;                  /* Number of PRGs spanning this allocation */
    uint16_t prgSize;                 /* Size in RBs of a precoding resource block group */
    uint8_t  digitalBfNum;            /* Number of logical antenna ports */
    uint16_t beamIndex[MAX_PRG_NUM][MAX_BF_PORT];
    /* PuschParaAddInV3 */
    uint8_t  puschTransType;          /* puschTransTypeValidity = 1 有效. Value 0: */
    uint16_t deltabwp0fromActiveBwp;  /* The value of Bwp[0].Start - Bwp[i].start */
    uint16_t initialUlBwpSize;
    uint8_t  groupOrSequenceHopping;  /* 0: neither, neither group or sequence hopping is enabled; 1: enable, enable group hopping and disable sequence hopping; 2: disable, disable group hopping and enable sequence hopping*/
    uint16_t puschSecondHopPRB;       /* Index of the first PRB after intra-slot frequency hopping, as indicated by the value of Rbstart for i=1 */
    uint8_t  ldpcBaseGraph;           /* 1: LDPC base graph 1; 2: LDPC base graph 2 */
    uint32_t tbSizeLbrmBytes;         /* Parameter TBSLBRM from 3GPP TS 38.212 section 5.4.2.1*/

    PuschDataPara    puschDataPara;    /**/
    PuschUciPara     puschUciPara;     /**/
    PuschPtrsPara    puschPtrsPara;    /**/ 
    PuschDftOfdmPara puschDftOfdmPara; /**/
    Part2InfoAddInV3 part2InfoAddInV3; /**/
} L1PuschPduInfo;

typedef struct
{
    uint16_t       sfnIndex;          /* system frame number [0: 1023] */
    uint8_t        slotIndex;         /* slot number [0: 159]  */
    uint8_t        puschPduNum;     /* Number of PuschPdus that are parse from FAPI UlTTIRequset */
	L1PuschPduInfo l1PuschPduInfo[MAX_PUSCH_PDU_NUM]; /* pusch ue级参数，最大处理16Ue */      
} L1PuschParaPduInfo;

typedef struct 
{
    uint16_t sfnIndex;             /* system frame number [0: 1023] */
    uint8_t  slotIndex;            /* slot number [0: 159]  */
    uint8_t  removeCpEn;         /* 去CP使能开关 */
    uint8_t  freqShift7p5kEn;    /* 7.5KHz频偏补偿使能开关 */
    uint8_t  phaseCompEn;        /* 上行相位补偿使能开关 */
    uint8_t  dAgcEn;             /* DAGC补偿使能开关 */
    uint8_t  fftEn;              /* 上行FFT使能开关 */
    uint8_t  freqRearrangementEn;/* 上行频域重排开关 */
    uint16_t fftSize;            /* 支持点数 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096 */
    uint8_t  carrierType;        /* 载波类型，0：NR， 1：LTE */
    uint16_t carrierBw;          /* 载波带宽，0:5MHz，1:10MHz，2:15Mhz，3:20MHz，4:25MHz，5:30Mhz，6:400MHz，7:50Mhz，8:60MHz，9:70MHz，10:80Mhz，11:90MHz，12:100Mhz */
    uint8_t  cpType;             /* CP类型，Normal/Extended */
    uint16_t cpLen1;             /* 半子帧首符号去CP的偏移值 */
    uint16_t cpLen2;             /* 半子帧非首符号去CP的偏移值 */
    uint8_t  numerologiesMul;    /* NR载波的传输参数集参数，协议值{0,1,2,3,4},产品支持{0,1} */
    uint32_t phaseCompCoeff[28]; /* 一个子帧内的所有OFDM符号相位补偿系数，15KHz时14个符号，30KHz时28个符号*/
    uint32_t carrierFreqf0;      /* 上变频的频点 单位为Hz*/
    uint8_t  agcUintdB;          /* agc分辨率，与ADC配置一致，范围0~7dB，间隔1dB，默认值为2 */
    uint8_t  autoSelect;         /* 0：手动配置补偿适用于无VGA因子场景，1：配置补偿适用于VGA因子场景 */  
    uint8_t  gainManualRatio;    /* 手动设置放大倍数，默认值4，对应浮点值1 */
    uint8_t  bTarget;            /* 目标DAGC因子 */
    uint16_t validScsStart;      /* 当前符号有效子载波起始位置 validScsStart = (fftsize - 载波的Rb数)/2 + K0 */
    uint16_t validScsEnd;        /* 当前符号有效子载波结束位置 validScsEnd = (fftsize - 载波的Rb数)/2 + K0 - 1 */
    uint8_t  antBitmapAxc;       /* AntIndex 和 CPRI AXC 映射关系 */
    /* RSSI 测量相关，分symb统计模式和RB统计模式。symb：一个symb一个值，支持同时统计多小区多天线，RB：一个RB一个值，每次只统计一个小区一个天线一个符号*/
    uint8_t  pwrTrigBitmap0;     /* 资源0功率统计使能，最多8个载波，每bit表示一个载波是否使能 */
    uint8_t  pwrTrigBitmap1;     /* 资源1功率统计使能，最多8个载波，每bit表示一个载波是否使能 */
    uint8_t  timeFreqFlag;       /* 0:时域功率统计，1：频域功率统计 */
    uint8_t  pwrValMode;         /* 功率统计模式 0: offset统计模式，1：symb统计模式， 2: RB统计模式 */
    uint8_t  pwrMeasTrigMode;    /* 0: 10ms（无线帧起始） 为触发信号， 1： slot（时隙起始）为触发信号，offset偏移模式下生效 */  
    uint8_t  pwrMeasMode;        /* 频域RSSI0统计求和，0：数字功率求和（Power）， 1：幅值求和（Abs） */ 
    uint8_t  pwrMeasPeriod;      /* 频域RSSI0 周期统计模式，0：one slot，1：连续统计（以10ms为周期）*/
    uint8_t  pwrMeasSlotIndex;   /* 功率计算slot号，有效值0~19， 0xFF：index无效，统计最近的slot*/
    uint8_t  pwrMeasSymbIndex;   /* 功率计算的起始符号，有效值0~13 */
    uint16_t pwrMeasSfnIndex;    /* 功率计算的sfn（10ms帧号），有效值0~1023 */
    uint8_t  pwrMeasSymbSize;    /* 功率计算的符号数，有效值0~13，代表1~14个symbol */ 
    uint32_t pwrMeasAccuLen;     /* 累加长度 */
    uint32_t pwrMeasStartDelay;  /* RSSI0启动时延，等待累计开始的长度 */
    uint32_t *outDataAddrPerAnt[MAX_RX_ANT_NUM];/* 每天线RSSI测量输出到SMEM起始地址 */
} PuschLowPhyCellPara;

typedef struct 
{
    uint16_t sfnIndex;                 /* system frame number [0: 1023] */
    uint8_t  slotIndex;                /* slot number [0: 159]  */
    uint8_t  pwrStartSymb;           /* 功率计算的起始symbol号 */
    uint8_t  pwrSymbSize;            /* 功率计算的symb个数 */
    uint32_t hac0PwrAccuValue[344];  /* hac0功率统计值 */
    uint32_t hac1PwrAccuValue[344];  /* hac1功率统计值 */
    uint8_t  hac0PwrAccuDone;        /* hac0功率统计完成标志 */
    uint8_t  hac1PwrAccuDone;        /* hac1功率统计完成标志 */
    uint32_t pwrMeasStartDelayLen;   /* RSSI0 启动延时 */
    int32_t  pwrRssiDbm[MAX_RX_ANT_NUM][SYM_NUM_PER_SLOT];
    int32_t  pwrRssiDbfs[MAX_RX_ANT_NUM][SYM_NUM_PER_SLOT];
    int32_t  avePwrRssiSlot[MAX_RX_ANT_NUM];
    int32_t  avePwrRssiInterSlot[MAX_RX_ANT_NUM];
} PuschLowPhyCellReport;

typedef struct 
{
    uint8_t  cellNum;                 /* 小区数 */
    PuschLowPhyCellPara puschLowPhyCellPara[MAX_CELL_NUM]; /* 单小区下PUSCH LowPhy 参数配置结构体 */
    PuschLowPhyCellReport puschLowPhyHacReport[MAX_CELL_NUM];/* 单小区下PUSCH LowPhy RSSI测量输出结构体 */
} PuschLowPhyHacPara;

typedef struct
{
    uint8_t  rxAntNum;/* 小区接收天线数取值为[1,2,4] */
    uint8_t  rsv0[2];
    uint8_t  rxAntIdx[MAX_RX_ANT_NUM];/* 小区接收天线索引 */
    uint32_t minCPT;/* 载波时隙中较小的cp长度用于变换域降噪窗的计算参数 */
    uint32_t nfft;  /* 载波系统NFFT点数，用于变换域降噪窗的计算参数*/
    /* PUSCH算法参数 */
    uint8_t  foeSwitch;      /* 频偏估计开关 */
    uint8_t  ruuAvgTimeFlag; /* 主要针对两列导频计中，计算完最后一列导频后的RUU是否求平均，可配置 */
    uint8_t  puschEqMode;    /* 0:MRC,1:IRC,2:adaptive */
    uint8_t  rsv1;
    uint16_t ruuAvgRbGrpNum; /* RUU频域计算颗粒度，单位RB */
    uint16_t targetAgc;      /* 频域目标AGC */
}NrPuschCellPara;

/* 信道估计接口 */
typedef struct
{
    uint16_t cdmIdx;         /* CDM GROUP索引 */
    int8_t   portNum;        /* cdmIdx 包含的端口数，取值范围1或者2 */
    int8_t   startPortIdx;   /* cdmIdx下包含端口号的起始索引 取值范围0-5 */
}PuschCdmPara;

typedef struct
{
    uint8_t  ueIndex;
    uint8_t  dftPrecEn;          /* (transform_precoding_enable),0:CP波形,1:DFT波形*/
    uint8_t  dmrsCfgType;        /* DMRS configuration type,0:type1,1:type2*/
    uint8_t  rsv0;
    int16_t  sinr;               /* Q(1,16,8) 主要用于外推平滑 */
    int16_t  ta;                 /* 用于后续解频域OCC优化的参数 */
    uint8_t  userTaskType;       /* 用户在当前符号上的任务类型， 1：表示调度的是第一个符号，需要做频域dagc;2:表示DMRS符号，需要做信道估计；3:表示调度的既是第一个符号，又是DMRS符号，需要做频域dagc和信道估计*/
    uint8_t  totalDmrsSymbNum;   /* 该UE总的DMRS符号个数，当前方案除了msg3可携带3个DMRS符号，常规上行子帧最大2个DMRS符号*/
    uint8_t  dmrsPositionIdx;    /* UE DMRS 符号索引，0:首符号，1：中间符号，2：第三个符号*/
    uint8_t  cheRbgNum;          /* 连续的RBG个数，即每个UE包含的RB簇个数 */  
    uint8_t  startRbgIdx;        /* ue起始RB簇索引*/
    uint8_t  validCdmNum;        /* UE使用的端口数实际占用几个CDM数*/
    uint16_t rbsize;             /* 所有RBG簇的RB数总和 */ 
    PuschCdmPara puschCdmPara[2];/* UE的CDM信息 */
    uint32_t *puschBaseSeq;      /* UE当前符号上的DMRS基序列 */
    uint32_t puschBaseSeqLength; /* UE当前符号上的DMRS基序列长度 */
} NrPuschUePara;

typedef struct
{
    uint16_t rbStart;
    uint16_t rbNum;
    uint16_t idftSize;
} NrPuschCePara;

typedef struct
{
    /* for the moment */
    uint16_t uePdp[16];
    uint16_t ueSinr[16];
    uint16_t ueTa[16];
    uint16_t ueRssi[16];
    uint16_t ueFreqOff[16];
    uint16_t prbNi[273];
} NrPuschMeasPara;

typedef struct
{
    uint8_t         validUeNum;/*当前符号调度的有效用户数*/
    uint8_t         rsv[3];
    NrPuschUePara   puschUePara[NR_PUSCH_MAX_UE_NUM_PER_SLOT];   /* UE相关参数 */
    NrPuschCePara   puschCePara[NR_PUSCH_MAX_PRG_NUM];           /* 频域资源位置参数 */
    //NrPuschMeasPara puschMeasPara[NR_PUSCH_MAX_UE_NUM_PER_SLOT]; /* PUSCH测量参数 */
    uint32_t       *puschFreqData;  /* PUSCH频域Data地址，每符号给一个地址，按照天线偏移 */
    int32_t        *puschFreqAgc;   /* PUSCH频域Agc 地址，每符号给一个地址，按照天线偏移 */
    uint32_t       *puschCheResult; /*信道估计输出指针，dsp_hac的目的地址*/
    uint32_t       *puschRuuResult; /*信道估计输出指针，dsp_hac的目的地址*/
    int32_t        *puschCheAgc;    /*信道估计输出AGC，主要来源是变换域ifft和FFT的输出，dsp_hac的目的地址*/
}NrPuschSymPara;

typedef struct 
{
    uint16_t sfnIndex;     /* system frame number [0: 1023] */
    uint8_t  slotIndex;    /* slot number [0: 159]  */
    NrPuschCellPara puschCellPara;
    NrPuschSymPara  puschSymPara[SYM_NUM_PER_SLOT];
} PuschDcePara;

/* PN序列接口 */
typedef struct 
{
    uint32_t cinit;     /* pn序列的初始值 */
    uint16_t seqLen;
    uint16_t seqStart;
    uint32_t *outPutAddr;
} PnGenParaPerSeq;

typedef struct 
{
    uint16_t sfnIndex;     /* system frame number [0: 1023] */
    uint8_t  slotIndex;    /* slot number [0: 159]  */
    uint8_t  pnSeqNum;     /* 序列个数 */
    PnGenParaPerSeq PnGenParaPerSeq[48]; /* 按照最大16Ue*3Dmrs开参数结构体 */
} PuschPnGenPara;

/* DEQ&DEMAP 接口 */
typedef struct 
{
    uint16_t ackRvdExsitFlag;
    uint16_t ackRvdStartRe;
    uint16_t ackRvdReNum;
    uint16_t ackRvdDistance;
    uint16_t ackExsitFlag;
    uint16_t ackStartRe;
    uint16_t ackReNum;
    uint16_t ackDistance;
    uint16_t csiPart1ExsitFlag;
    uint16_t csiPart1StartRe;
    uint16_t csiPart1ReNum;
    uint16_t csiPart1Distance;
} PuschAckAndCsiPart1Info;

typedef struct 
{
   uint8_t  dataSymbNum;
   uint8_t  dmrsSymbNum;
   uint8_t  firstDmrsIndex;
   uint8_t  secondDmrsIndex;
   uint8_t  thirdDmrsIndex;
   uint8_t  cbNum;
   uint8_t  tbCrcType;
   uint8_t  iLs;
   uint16_t ulKd;
   uint16_t ulKr;
   uint16_t cbFillingLen;
   uint16_t ulZc;
   uint16_t ulN;
   uint16_t rbSize;
   uint16_t hackAckbit;
   uint16_t enCodeAckRe;
   uint16_t csiPart1bit;
   uint16_t enCodeCsiPart1Re;
   uint16_t rbNumAllRbg;
   PuschAckAndCsiPart1Info puschAckAndCsiPart1Info[SYM_NUM_PER_SLOT];
} PuschDemapParaTemp;

typedef struct
{
    uint8_t  ueIndex;              /* UE索引 */
    uint8_t  layerNum;             /* 传输层数 */
    uint8_t  transPrecodeEn;       /* (transform_precoding_enable),0:CP波形,1:DFT波形 */
    uint8_t  rbBitmap[35];         /* FreqData的位图*/
    uint16_t betaRuu;              /* 对角加载因子 */
    uint8_t  dmrsSymbFlag;         /* dmrs符号的数量 */
    uint8_t  dmrsSymbBitMap;       /* UE DMRS 符号索引，0:首符号，1：中间符号，2：第三个符号 */
    uint16_t equInterCoeff[2];     /* 差值系数 DmrsNum * SlotSymbNum, 按照符号配置，每符号最多3个值,实际配置2个值*/
    uint8_t  timeInterMode;        /* 时域插值模式，0：平推，1：插值 */
    uint8_t  codeWordNum;          /* 码字个数, NR单码字，LTE最多2码字 */
    uint8_t  deModType;            /* 调制模式，0~5 */
    uint16_t dmrsReBitmap;         /* 每RB内 data re的bitmap，用于Re抽取 */
    //uint16_t rbNumAllRbg;        /* UE所有RBG包含的全部RB个数之和 */
    uint16_t freqCompCoeff;        /* 频偏相位补偿值，UeNum*Symbol */
    /* UCI占位符信息指示 */
    uint8_t  palcehoderFlag;       /* X&Y 占位符， 0：无； 1：1bit UCI； 2:2bit UCI */
    uint16_t uciReStart;           /* UCI所占的REbitmap */
    uint16_t uciReDistance;        /* 2个UCI RE的间距 */
    uint16_t uciReNum;             /* UCI RE的数目 */
    uint16_t scrambleReOffset;     /* Ue每符号之前的，多符号多流的RE累加值，用于计算解扰序列的起始word和bit位置 */
    /* UCI复用信息指示 */
    uint8_t  ackFlag;              /* 当前符号存再ACK */
    uint16_t ackReStart;           /* ACK的起始RE索引 */
    uint16_t ackReDistance;        /* 2个ACK RE的间距  */
    uint16_t ackReNum;             /* ACK的RE数目 */
    uint8_t  csiPart1Flag;         /* 当前符号存再Part1 */
    uint16_t csiPart1ReStart;      /* part1的起始RE索引 */
    uint16_t csiPart1ReDistance;   /* 2个ACK RE的间距  */
    uint16_t csiPart1ReNum;        /* CSIPart1的RE数目 */
    /* Dmrs输入地址，Ue级，扰码会在生成时按照ue产生 */
    uint32_t *dmrsHInBaseAddr1;    /* 第1个DMRS H输入的基地址 */
    uint32_t *dmrsHInBaseAddr2;    /* 第2个DMRS H输入的基地址 */
    uint32_t *dmrsHInBaseAddr3;    /* 第3个DMRS H输入的基地址 */
    uint32_t *ruuInBaseAddr;       /* Ruu输入的基地址 RuuRbg*DmrsSymb*ant*ant */
    /* 扰码输入地址，Ue级，扰码会在生成时按照ue产生 */
    uint32_t *scrambBaseAddrC0In;  /* UE当前符号上码字0的扰码基地址，NR单码字，LTE表示码字0的基地址 */
    uint32_t *scrambBaseAddrC1In;  /* UE当前符号上码字1的扰码基地址，NR不使用，LTE表示码字1的基地址 */
    /* 输出地址，按照ue输出解复用后的数据和平均后的postSinr */
    uint32_t *postSinrAverOut;     /* postSinr输出基地址 */
    uint32_t *demuxAckOut;         /* demap ACK LLR输出地址*/
    uint32_t *demuxpart1Out;       /* demap csipart1 LLR输出地址*/
    uint32_t *demuxpart2DataOut;   /* demap csipart2+data LLR输出地址*/
} PuschDeqAndDemapUePara;

typedef struct 
{
    uint8_t          validUeNum;   /* 当前符号调度的有效用户数 */
    uint8_t          rsv[3];
    PuschDeqAndDemapUePara puschDeqAndDemapUePara[NR_PUSCH_MAX_UE_NUM_PER_SLOT];
} PuschDeqAndDemapSymPara;

typedef struct 
{
    uint16_t sfnIndex;            /* system frame number [0: 1023] */
    uint8_t  slotIndex;           /* slot number [0: 159]  */
    uint8_t  cellIndex;           /* 小区索引，最大支持4小区 */
    uint8_t  rxAntNum;            /* 接收天线数 [1,2,4] */
    uint8_t  freqCompEn;          /* 频偏相位补偿使能开关 */
    uint8_t  ruuAvgRbGrpNum;      /* RUU频域计算颗粒度，单位RB */
    uint8_t  xCut;                /* 算法配置参数，默认值3 */
    uint8_t  ySat;                /* 算法配置参数，默认值5 */
    uint8_t  demodThreshold;      /* 算法配置参数，默认值40 */
    uint8_t  ueNum;               /* 当前slot调度的总ue数  */  
    uint32_t *rxDataAddr[MAX_RX_ANT_NUM];  /* 频域数输入基地址 */
    PuschDeqAndDemapSymPara puschDeqAndDemapSymPara[SYM_NUM_PER_SLOT];
} PuschDeqAndDemapHacPara;

/* LDPC 接口 */
typedef struct 
{
    uint8_t  ueIndex;          /* UE 索引 */
    uint8_t  qamMode;          /* 调制阶数 */
    uint8_t  layerNum;         /* 层数 */
    uint8_t  ldpcBgId;         /* LDPC校验矩阵类型索引，通过baseGraph和ldpciLS联合确定*/
    uint8_t  harqCombFlag;     /* HARQ合并标志 */
    uint8_t  tbCrcType;        /* TB CRC类型：0：不进行CRC，1:24A，2:24B，3:24C，4:16，5:11，6:6，7:8 */
    uint16_t cbNum;            /* 码块数 */                                                                                                                                                                                                         
    uint16_t cbFillBitLen;     /* 每个码块的填充bit数，范围：0-696 */
    uint16_t ldpcKd;           /* 每个码块除填充bit外的长度，如果分块则包含CB-CRC长度24 */
    uint16_t ldpcZc;           /* 提升因子 38.212 Table 5.3.2-1 */
    uint16_t ldpcK0;           /* 速率匹配的起始位置 */
    uint16_t ldpcNcb;          /* 速率匹配缓存大小 */
    uint16_t rBgUsedBitmap[3]; /* Harq合并后参与译码扩展校验行对应的bitmap，长度42 */ 
    uint8_t  cbRmLen0Num;      /* 使用第一类码块速率匹配长度的码块个数 */
    uint8_t  rsv[3];
    uint32_t cbRmLen0;         /* 第一类码块速率匹配后的长度 */
    uint32_t cbRmLen1;         /* 第二类码块速率匹配后的长度 */
    uint32_t tbSize;           /* 添加TB Size */ 
    uint32_t harqInBufOffset;
    uint32_t harqOutBufOffset;
    uint32_t tbDataOutOffset;
    uint32_t tbCrcOutOffset;
    uint32_t *llrDemapDataout; /* 解复用后的数据，作为LDPC的输入 */
} PuschLdpcUePara;

typedef struct 
{
    uint16_t sfnIndex;                                  /* system frame number [0: 1023] */
    uint8_t  slotIndex;                                 /* slot number [0: 159]  */
    uint8_t  ueNum;                                     /* 当前小区的UE数 */
    uint8_t  harqCompressType;                          /* LLR压缩模式 */
    uint8_t  reviseFactor;                              /* 修正因子，默认值0.75 */
    uint8_t  maxIterNum;                                /* 最大迭代次数 */
    uint8_t  decodeAlgType;                             /* 译码算法选择 */
    PuschLdpcUePara puschLdpcUePara[MAX_PUSCH_PDU_NUM]; /*  */ 
    uint32_t *llrInBaseaddr;                            /* 输入数据地址 */
    uint32_t *harqInBaseAddr;                           /* HARQ LLR 输入基地址 */
    uint32_t *harqOutBaseAddr;                          /* HARQ LLR 输出基地址 */
    uint32_t *tbDataOutBaseAddr;                        /* 译码数据硬bit 输出基地址 DDR */
    uint32_t *tbCrcOutBaseAddr;                         /* 译码CRC 输出基地址 */
} PuschLdpcDecoderHacPara;

typedef struct 
{
    uint32_t handle;
    uint16_t nRnti;
    uint8_t  harqId;
    uint16_t pduLength;
    uint8_t  ulCQI;
    uint16_t timeingAdvance;
    uint16_t rssi;
    uint8_t  tbCrcStatus;
    uint16_t cbNum;
    uint8_t  cbCrcStatus[20];
} PuschRxPduInfo;

typedef struct 
{
    uint16_t sfnIndex;                                    /* system frame number [0: 1023] */
    uint8_t  slotIndex;                                   /* slot number [0: 159]  */
    uint16_t pduNum;                                     /*  */
    PuschRxPduInfo puschRxPduInfo[MAX_PUSCH_PDU_NUM];
} RxDataIndication;

typedef struct 
{
    uint32_t handle;
    uint16_t nRnti;
    uint8_t  harqId;
    uint8_t  tbCrcStatus;
    uint16_t cbNum;
    uint8_t  cbCrcStatus[20];
    uint8_t  ulCQI;
    uint16_t timeingAdvance;
    uint16_t rssi;
} PuschCrcPduInfo;

typedef struct 
{
    uint16_t sfnIndex;                                    /* system frame number [0: 1023] */
    uint8_t  slotIndex;                                   /* slot number [0: 159]  */
    uint16_t crcNum;                                     /*  */
    PuschCrcPduInfo puschCrcPduInfo[MAX_PUSCH_PDU_NUM];
} CRCIndication;

typedef struct 
{
    uint32_t handle;
    uint16_t nRnti;
    uint8_t  harqId;
    uint8_t  tbCrcStatus;
    uint16_t cbNum;
    uint8_t  cbCrcStatus[20];
    uint8_t  ulCQI;
    uint16_t timeingAdvance;
    uint16_t rssi;
} PuschUciPduInfo;

typedef struct 
{
    uint16_t sfnIndex;                                    /* system frame number [0: 1023] */
    uint8_t  slotIndex;                                   /* slot number [0: 159]  */
    uint16_t uciNum;                                     /*  */
    PuschCrcPduInfo puschCrcPduInfo[MAX_PUSCH_PDU_NUM];
} UCIIndication;