#pragma once
#include "../../../../common/inc/common_typedef.h"
#include "../../../../common/inc/common_macro.h"

typedef struct 
{
    uint16_t ptrsPortIndex;     /* PT-RS antenna ports index */
    uint8_t  ptrsDmrsPort;      /* DMRS port corresponding to PTRS */
    uint8_t  ptrsReOffset;      /* PT-RS resource element offset value taken from */
}PtrsPortPara;

typedef struct
{
    uint8_t  rvIndex;            /* Redundancy version index Value : 0->3*/
    uint8_t  harqProcessId;      /* HARQ process number Value: 0 ->15*/
    uint8_t  newData;            /* Value: 0: retransmission, 1: new data, i.e. initial transmission*/
    uint32_t nTBSzie;            /* Transmit block size */
    uint32_t numCb;              /* Number of CBs in the TB; Should be set to zero in any of the following conditions: 1) CBG is not supported 2) newData=1 (new transmission) 3) tbSize=0 */
    uint8_t  cbPresentAndPos[4]; /* CB is present in the current retx of the PUSCH. 1=PRESENT, 0=NOT PRESENT. uint8_t[ceil(numCb/8)] 暂不支持CBG传输，所以numCb为0 */
}PuschDataPara;

typedef struct 
{
    uint16_t harqAckBitLength;   /* Number of HARQ-ACK bits. Value: 0 -> 11 (Small block length). 12 ->1706 (Polar) */
    uint16_t csiPart1BitLength;  /* Number of CSI part1 bits. Value: 0 -> 11 (Small block length). 12 ->1706 (Polar) */
    uint16_t flagCsiPart2;       /* Number of CSI part2 bits. 0 -> No CSI part 2*/
    uint8_t  alphaScaling;       /* Alpha used to calculate number of coded modulation symbols per layer,Value:0 = 0.5,1 = 0.65,2 = 0.8,3 = 1 */
    uint8_t  betaOffsetHarqAck;  /* Beta Offset for HARQ-ACK bits. Value: 0->15 */
    uint8_t  betaOffsetCsi1;     /* Beta Offset for CSI-part1 bits.Value: 0->18 */
    uint8_t  betaOffsetCsi2;     /* Beta Offset for CSI-part2 bits.Value: 0->18 */
}PuschUciPara;

typedef struct 
{
    uint8_t      numPtrsPorts;                /* Number of UL PTRS ports Value: 1->2 */
    PtrsPortPara ptrsPortInfo[PTRS_PORT_NUM]; 
    uint8_t      ptrsTimeDensity;             /* PT-RS time density.Value: 0: 1, 1: 2, 2: 4 */
    uint8_t      ptrsFreqDensity;             /* PT-RS frequency density. Value: 0: 2, 1: 4 */
    uint8_t      ulPtrsPower;                 /* PUSCH to PT-RS power ratio per layer per RE. Value: 0: 0dB,1: 3dB,2: 4.77dB,3: 6dB*/
}PuschPtrsPara;

typedef struct 
{
    uint8_t  lowPaprGrpNum;          /* Group number for Low PAPR sequence generation.For DFTS-OFDM */
    uint16_t lowPaprSeqNum;          /* sequence number of Low PAPR sequence.  For DFTS-OFDM */
    uint8_t  ulptrsSampleDensity;    /* Number of PTRS groups. */
    uint8_t  ulptrsTimeDensity;      /* Number of samples per PTRS group.*/
}PuschDftOfdmPara;

typedef struct 
{
    uint16_t priority;                          /* Priority of the part 2 report */
    uint8_t  numPart1Params;                    /* Number of Part 1 parameters that influence the size of this part 2; values [1:4] in FAPI v3*/
    uint16_t paramOffsets[MAX_PART1_PAPR_NUM];  /* Ordered list of parameter offsets (offset from 0 = first bit of part1); The real size is numPart1Params */
    uint8_t  paramSizes[MAX_PART1_PAPR_NUM];    /* Bitsizes of part 1 param in the same order as paramOffsets; The real size is numPart1Params */
    uint16_t part2SizeMapIndex;                 /* Index of one of the maps configured Table 3-40, for determining the size of a part2, from the part 1 parameter values */
}Part2ReportPara;

/* Uci information for determining UCI Part1 to Part2 correspondence, added in FAPIv3 */
typedef struct 
{
    uint16_t        numPart2s;       /* Max number of UCI part2 that could be included in the CSI report. Value: 0 -> 100 */
    Part2ReportPara part2ReportPara[MAX_CSI_PART2_REPOET_NUM]; 
}Part2InfoAddInV3;

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
    uint16_t dmrsPort;                /* Bitmap occupying the 12 LSBs with: bit 0: antenna port 1000, bit 11: antenna port 1011,and for each bit. 0: DMRS port not used,1: DMRS port used*/
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
    Part2InfoAddInV3  part2InfoAddInV3;/**/
}L1PuschPduInfo;

typedef struct
{
    uint16_t       sfnNum;          /* system frame number [0: 1023] */
    uint8_t        slotNum;         /* slot number [0: 159]  */
    uint8_t        puschPduNum;     /* Number of PuschPdus that are parse from FAPI UlTTIRequset */
	L1PuschPduInfo l1PuschPduInfo[MAX_PUSCH_PDU_NUM]; /* pusch ue级参数，最大处理16Ue */      
} L1PuschParaPduInfo;





L1PuschParaPduInfo  g_puschParaInfoOut[MAX_CELL_NUM] = { 0 };/* Prach 参数本地buffer */
