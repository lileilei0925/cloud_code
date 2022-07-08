#include "../inc/common_teypedef.h"
#include "../inc/common_macro.h"


#define    MAX_PRACH_FDM_NUM                 2
#define    UN_USED_ROOT_PER_FDM              1

#define    PRACH_ZC_LEN_139                  139
#define    PRACH_ZC_LEN_839                  839

#define    PRACH_FORMAT_0                    0
#define    PRACH_FORMAT_1                    1
#define    PRACH_FORMAT_2                    2
#define    PRACH_FORMAT_3                    3
#define    PRACH_FORMAT_A1                   4
#define    PRACH_FORMAT_A2                   5
#define    PRACH_FORMAT_A3                   6
#define    PRACH_FORMAT_B1                   7
#define    PRACH_FORMAT_B4                   8
#define    PRACH_FORMAT_C0                   9
#define    PRACH_FORMAT_C2                   10
#define    PRACH_FORMAT_NUM_139              7
#define    PRACH_FORMAT_NUM_839              4


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

/* P5 Carrier configuration messages local structure*/
typedef struct 
{
    uint16_t bandWidthDl;
    uint32_t absoluteFreqDl;
    uint16_t k0MiuDl[5];
    uint16_t gridSizeDl[5];
    uint16_t txAntNum;
    uint16_t bandWidthUl;
    uint32_t absoluteFreqUl;
    uint16_t k0MiuUl[5];
    uint16_t gridSizeUl[5];
    uint16_t rxAntNum;
    uint8_t  freqShift7p5;
    uint8_t  powerProfile;
    uint8_t  PowerOffsetRsIndex;
} L1CarrierCfgInfo;

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
    int16_t  k1[MAX_PRACH_FDM_NUM];
    uint8_t  prachZeroCorrConf[MAX_PRACH_FDM_NUM];
    uint16_t numUnusedRootSequences[MAX_PRACH_FDM_NUM];
    uint16_t unusedRootSequences[MAX_PRACH_FDM_NUM][MAX_PREAMBLES_NUM];
    uint8_t  ssbPerRach;
}L1PrachConfigInfo;

/* P7 Prach slot messages local structure*/
typedef struct 
{
    uint16_t pduIndex;
    uint16_t phyCellID;
    uint8_t  prachTdOcasNum;
    uint8_t  prachFormat;
    uint8_t  PrachFdmIndex;   /* Frequency domain occasion index :msg1-FDM*/     
    uint8_t  prachStartSymb;  /* Starting symbol for the first PRACH TD occasion in the current PRACH FD occasion */
    uint16_t ncsValue;        /* Zero-correlation zone configuration number */

    uint32_t handle;
    uint8_t  prachCfgScope;   /* 0: for PHY ID 0; 1: for current PHY ID */
    uint16_t prachResCfgIndex;/* The PRACH configuration for which this PRACH PDU is signaled  */
    uint8_t  prachFdmNum;     /* Number of frequency domain occasions,starting with PrachFdmIndex */
    uint8_t  startPreambleIndex;
    uint8_t  preambleIndicesNum;

    uint8_t  trpScheme; /* This field shall be set to 0, to identify that this table is used */
    uint16_t prgNum;
    uint16_t prgSize;
    uint8_t  digitalBfNum;/* Number of logical antenna ports */
    uint16_t beamIndex[MAX_PRG_NUM][MAX_BF_PORT];
} L1PrachPduInfo;

typedef struct 
{
    uint16_t       sfnNum;        /* system frame number [0: 1023] */
    uint16_t       slotNum;       /* SLOT number [0: 159] */
    uint16_t       prachPduNum;   /* Number of PrachPdus that are parse from FAPI UlTTIRequset */
    L1PrachPduInfo l1prachPduInfo[MAX_PRACH_PDU_NUM];   /* FDM为2时 可以有2个prach PDU*/
} L1PrachParaInfo;

/* PrachLowPhyPara structure: prach LowphyPara config para */
typedef struct
{
    uint8_t  cellIdx;                           /* L1与L2之间的小区标识*/
    uint8_t  numRxAnt;  
    /* FDM-x 移频参数 */
    uint8_t  freqShiftEn;                       /* Frequency shift Enable Flag. 0：Disable; 1: Enable */
    uint8_t  indexFdRa;                         /*  */
    uint8_t  prachFdmNum;                       /*  */ 
    uint32_t sampleRateCarry;                   /*  */
    int32_t  freqShiftValue[MAX_PRACH_FDM_NUM]; /*  */
    /* 时域AGC参数 */
    uint8_t  tdDagcEn;                          /* 时域AGC调整Enable Flag， 0：Disable; 1: Enable */
    int8_t   tdDagcTarget;                      /* 需要调整的AGC因子，btarget */
    uint8_t  repeatTimesInOcas;                 /* 同一个Occasion 内的重复次数 */
    uint8_t  nFFTSample;                        /* 降采样后的采样率下，每一个重复的Prach时域样点个数*/
    /* 降采样参数 */  
    uint8_t  downSamplingEnBitMap;              /* 7级半带滤波器级联；优先级bit位从高到低；bit0：CoefHBF1；0：Disable; 1: Enable*/
    /* 去CP参数 */
    uint8_t  cpRemoveEn;                        /* 去Cp Enable Flag， 0：Disable; 1: Enable */
    uint16_t cutCpValue;                        /* 应该去掉的CP值，Arm计算获取*/
    /* FFT 参数 */
    uint8_t  fftSizeIndex;                      /* FFT size index. 0: 128; 1: 256; 2: 512; 3: 1024; 4: 2048; 5: 4096; 8: 192; 9:384; 10: 768; 11: 1536; 12: 3072 */  
    uint8_t  prachTdOcasNum;       
    uint8_t  prachStartSymb[7];    
} PrachLowPhyParaPerPdu;

typedef struct
{
    uint16_t              sfnNum;                             /* system frame number [0: 1023] */
    uint8_t               slotNum;                            /* slot number [0: 159]  */
    uint8_t               prachFeEn;                          /* PrachLowphy Enable Flag. 0: Disable 1: Enable */
    PrachLowPhyParaPerPdu lowPhyParaPerPdu[MAX_PRACH_PDU_NUM]; /*  */  
} PrachLowPhyHacPara;

typedef struct 
{
    uint16_t   uwPreambleFormat;
    uint16_t   uwRALen;
    uint8_t    repeatTimesOcas;
    uint32_t   udRaCp;
} PrachPreambleLRA;

/* 3GPP 38.211 Table - 6.3.3.1-2 */
PrachPreambleLRA g_PreambleforLRa139[PRACH_FORMAT_NUM_139] =
{
    {PRACH_FORMAT_A1, 139,  2,  288 },
    {PRACH_FORMAT_A2, 139,  4,  576 },
    {PRACH_FORMAT_A3, 139,  6,  864 },
    {PRACH_FORMAT_B1, 139,  2,  216 },
    {PRACH_FORMAT_B4, 139, 12,  936 },
    {PRACH_FORMAT_C0, 139,  1,  1240},
    {PRACH_FORMAT_C2, 139,  4,  2048}
};

PrachPreambleLRA g_PreambleforLRa839[PRACH_FORMAT_NUM_839] =
{
    { PRACH_FORMAT_0, 839,  1,  3168},
    { PRACH_FORMAT_1, 839,  2, 21024},
    { PRACH_FORMAT_2, 839,  4,  4688},
    { PRACH_FORMAT_3, 839,  4,  2976}
};

typedef struct 
{
    uint32_t prachScsValue;
    uint32_t puschScsValue; 
    uint8_t nRaRB;
    uint8_t raKbar;
} PrachRaRbAndKbar;

/* 3GPP 38.211 Table - 6.3.3.2-1 */
PrachRaRbAndKbar g_PreambleforFraKbar[6][4] =
{
    // delta-Fra = 15
    {
        {15000, 15000, 12, 2},      /* Delta-f for PUSCH = 15 */
        {15000, 30000, 6,  2},      /* Delta-f for PUSCH = 30 */
        {15000, 60000, 3,  2},      /* Delta-f for PUSCH = 60 */
        {0,     0,     0,  0},      /* Delta-f for PUSCH = 120 */
    },
    // delta-Fra = 30
    {
        {30000, 15000, 24, 2},      /* Delta-f for PUSCH = 15 */
        {30000, 30000, 12, 2},      /* Delta-f for PUSCH = 30 */
        {30000, 60000, 6,  2},      /* Delta-f for PUSCH = 60 */
        {0,     0,     0,  0},      /* Delta-f for PUSCH = 120 */
    },
    // delta-Fra = 60
    {
        {0,     0,      0,  0},      /* Delta-f for PUSCH = 15 */
        {0,     0,      0,  0},      /* Delta-f for PUSCH = 30 */
        {60000, 60000,  12, 2},      /* Delta-f for PUSCH = 60 */
        {60000, 120000, 6,  2},      /* Delta-f for PUSCH = 120 */
    },
    // delta-Fra = 120
    {
        {0,      0,      0,  0},     /* Delta-f for PUSCH = 15 */
        {0,      0,      0,  0},     /* Delta-f for PUSCH = 30 */
        {120000, 60000,  24, 2},     /* Delta-f for PUSCH = 60 */
        {120000, 120000, 12, 2},     /* Delta-f for PUSCH = 120 */
    },
    // delta-Fra = 1.25
    {
        {1250, 15000, 6, 7  },       /* Delta-f for PUSCH = 15 */
        {1250, 30000, 3, 1  },       /* Delta-f for PUSCH = 30 */
        {1250, 60000, 2, 133},       /* Delta-f for PUSCH = 60 */
        {0,    0,     0, 0  },       /* Delta-f for PUSCH = 120 */
    },
    // delta-Fra = 5
    {
        {5000, 15000, 24, 12},       /* Delta-f for PUSCH = 15 */
        {5000, 30000, 12, 10},       /* Delta-f for PUSCH = 30 */
        {5000, 60000, 6,  7},        /* Delta-f for PUSCH = 60 */
        {0,    0,     0,  0},        /* Delta-f for PUSCH = 120 */
    },
};

uint32_t  g_DownSamplingValue839[4] ={1920000, 1920000, 1920000, 7680000};   /* 839 序列降采样后的采样率，根据Format格式查找 */
uint32_t  g_DownSamplingValue139[4] ={3840000, 7680000, 15360000, 30720000}; /* 139 序列降采样后的采样率，根据子载波间隔查找  */