#include "common_teypedef.h"
#include "common_macro.h"

/* P5 Prach Configuration Message */
typedef struct {	
    uint16_t        prachRootSequenceIndex;
    uint8_t         numRootSequences;
    int16_t         k1;
    uint8_t         prachZeroCorrConf;
    uint16_t        numUnusedRootSequences;
    uint16_t        unusedRootSequences[MAX_PREAMBLES_NUM]; /* Unused root sequence or sequences per FD occasion.Required for noiseestimation. */
}PrachFdOccasion;	
	
typedef struct {	
    uint16_t        prachResConfigIndex;
    uint8_t         prachSequenceLength;
    uint8_t         prachSubCSpacing;
    uint8_t         ulBwpPuschScs;      
    uint8_t         restrictedSetConfig;
    uint8_t         numPrachFdOccasions;
    uint8_t         prachConfigIndex;
    PrachFdOccasion prachFdOccasion[MAX_PRACH_FDM_NUM];
    uint8_t         ssbPerRach;
}PrachConfigRequest;	


/* P7 Prach slot messages are transmitted, or received, every slot */
typedef struct {
    uint8_t        trpScheme; 
    uint16_t       numPRGs;
    uint16_t       prgSize;
    uint8_t        digBfInterface;
    uint16_t       beamIndex[MAX_PRG_NUM][MAX_BF_PORT];
}Beamforming;

typedef struct {
    uint8_t        ueNum;
    uint8_t        pduIndexPerUe[MAX_UE_NUM_PER_GROUP];
}UeGroupInfo;

typedef struct {
    uint32_t       handle;
    uint8_t        prachCfgScope;
    uint16_t       prachResCfgIndex;
    uint8_t        numFdRa;
    uint8_t        startPreambleIndex;
    uint8_t        numPreambleIndices;
}PrachNewInV3;

typedef struct {
    uint16_t       phyCellID;
    uint8_t        prachTdOcasNum;
    uint8_t        prachFormat;
    uint8_t        fdRaIndex;      
    uint8_t        prachStartSymb;
    uint16_t       ncsValue;
    Beamforming    beamForming;   /* The beamforming PDU is included in the PRACH */
    PrachNewInV3   prachNewInV3;  /* PRACH Maintenance Parameters added in FAPIv3 */
}PrachPduInfo;


/* P7 Pusch slot messages are transmitted, or received, every slot */
typedef struct {
    uint16_t       ptrsPortIndex;
    uint8_t        ptrsDmrsPort;
    uint8_t        ptrsReOffset;
}PtrsPortInfo;

typedef struct {
    uint8_t        rvIndex;
    uint8_t        harqProcessId;
    uint8_t        newData;
    uint32_t       nTBSzie;
    uint32_t       numCb;
    uint8_t        cbPresentAndPose;
}PuschDataInfo;

typedef struct {
    uint16_t       harqAckBitLength;
    uint16_t       csiPart1BitLength;
    uint16_t       flagCsiPart2;
    uint8_t        alphaScaling;
    uint8_t        betaOffsetHarqAck;
    uint8_t        betaOffsetCsi1;
    uint8_t        betaOffsetCsi2;
}PuschUciInfo;

typedef struct {
    uint8_t        numPtrsPorts;
    PtrsPortInfo   ptrsPortInfo[MAX_PTRS_PORT_NUM];
    uint8_t        ptrsTimeDensity;
    uint8_t        ptrsFreqDensity;
    uint8_t        ulPtrsPower;
}PuschPtrsInfo;

typedef struct {
    uint8_t        lowPaprGrpNum;
    uint16_t       lowPaprSeqNum;
    uint8_t        ptrsSampleDensity;
    uint8_t        ptrsTimeDensity;
}PuschDftInfo;

typedef struct {
    uint8_t        puschTransType;
    uint16_t       deltabwp0StartfromActiveBwp;
    uint16_t       initialUlBwpSize;
    uint8_t        groupOrSequenceHopping;
    uint16_t       puschSecondHopPRB;
    uint8_t        ldpcBaseGraph;
    uint32_t       tbSizeLbrmBytes;
}PuschNewInV3;

typedef struct {
    uint16_t       priority;
    uint8_t        numPart1Params;   /* Number of Part 1 parameters that influence the size of this part 2; values [1:4] in FAPI v3*/
    uint16_t       paramOffsets[MAX_PART1_PAPR_NUM];  /* numPart1Params  values [1:4] in FAPI v3*/
    uint8_t        paramSizes[MAX_PART1_PAPR_NUM];
    uint16_t       part2SizeMapIndex;
}Part2ReportInfo;

/* Uci information for determining UCI Part1 to Part2 correspondence, added in FAPIv3 */
typedef struct {
    uint16_t        numPart2s;
    Part2ReportInfo part2ReportInfo;
}UciInfoAddInV3;

typedef struct {
    uint16_t       pduBitmap;
    uint16_t       nRnti;
    uint32_t       handle;
    uint16_t       bwpSize;
    uint16_t       bwpStart;
    uint8_t        subCarrierSpacing;
    uint8_t        cyclicPrefix;

    uint16_t       targetCodeRate;
    uint8_t        qamModOrder;
    uint8_t        mcsIndex;
    uint8_t        mcsTable;
    uint8_t        transformPrecoding;
    uint16_t       nIdPusch;
    uint8_t        numLayers;
    
    uint16_t       ulDmrsSymbPos;
    uint8_t        dmrsCfgType;
    uint16_t       dmrsScrambleId;
    uint16_t       puschDmrsidentity;
    uint8_t        nSCID;
    uint8_t        numDmrsCdmGrps;
    uint16_t       dmrsPort;

    uint8_t        resourceAlloc;
    uint8_t        rbBitmap;
    uint16_t       rbStart;
    uint16_t       rbSize;
    uint8_t        vrbToPrbMapping;
    uint8_t        intraSlotFreqhopping;
    uint16_t       txDcLocation;
    uint8_t        ulFreqShift7p5Khz;
    uint8_t        startSymbIndex;
    uint8_t        numSymbs;

    PuschDataInfo  puschDataInfo;    /* Optional puschData information */
    PuschUciInfo   puschUciInfo;     /* Optional puschUci information */
    PuschPtrsInfo  puschPtrsInfo;    /* Optional puschPtrs information */
    PuschDftInfo   puschDftOfdmInfo; /* Optional dftsOfdm information */
    Beamforming    beamForming;      /* The beamforming PDU is included in the PRACH */
    PuschNewInV3   puschNewInV3;     /* PUSCH Maintenance Parameters added in FAPIv3 */
    UciInfoAddInV3 uciInfoAddInV3;   /* Optional puschUci added in FAPIv3 */
}PuschPduInfo;


/* P7 Pucch slot messages are transmitted, or received, every slot */
typedef struct {
    uint8_t        maxCodeRate;
    uint8_t        ulBwpId;
}PucchNewInV3;

typedef struct {
    /* 0: PRACH PDU; 1: PUSCH PDU; 2: PUCCH PDU; 3: SRS PDU */
    uint16_t       nRnti;        
    uint32_t       handle;
    uint16_t       bwpSize;
    uint16_t       bwpStart;
    uint8_t        subcarrierSpacing;
    uint8_t        cyclicPrefix;
    uint8_t        formatType;
    uint8_t        multiSlotTxIndicator;
    uint8_t        pi2BpskFlag;
    uint16_t       prbStart;
    uint16_t       prbSize;
    uint8_t        StartSymbolIndex;
    uint8_t        symbolNum;
    
    uint8_t        intraSlotFreqHopping;
    uint16_t       secondHopPRB;
    uint8_t        pucchGroupHopping;

    uint8_t        obsolete8bit;    /* This flag is obsolete in FAPIv3 */
    uint16_t       nIdPucchHopping; /* The parameter nID used for sequence hopping */
    uint16_t       initCyclicShift;
    uint16_t       nIdPucchScrambling;
    uint8_t        tdOccIdx;
    uint8_t        preDftOccIdx;
    uint8_t        preDftOccLen;
    uint8_t        addDmrsFlag;
    uint16_t       dmrsScramblingId;
    uint8_t        dmrsCyclicShift;
    uint8_t        srFlag;
    uint16_t       bitLenHarq;
    uint16_t       csiPart1BitLength;
   
    Beamforming    beamForming;    /* The beamforming PDU is included in the PRACH */
    PucchNewInV3   pucchNewInV3;
    UciInfoAddInV3 uciInfoAddInV3;
}PucchPduInfo;


/* P7 Srs slot messages are transmitted, or received, every slot */
typedef struct {
    uint16_t       nRnti;
    uint32_t       handle;
    uint16_t       bwpSize;
    uint16_t       bwpStart;
    uint8_t        subcarrierSpacing;
    uint8_t        cyclicPrefix;
    uint8_t        numAntPorts;
    uint8_t        numSymbols;
    uint8_t        numRepetitions;
    uint8_t        startSymbol;
    uint8_t        nCsrs;         /* SRS bandwidth config index corresponding FAPI V3  configIndex*/
    uint16_t       sequenceId;
    uint8_t        nBsrs;         /* SRS bandwidth index, corresponding FAPI V3  bandwidthIndex*/
    uint8_t        combSize;      /* Transmission comb size Ktc*/
    uint8_t        combOffset;
    uint8_t        cyclicShift;
    uint8_t        freqPosition;  /* Frequency domain position nRRC*/
    uint16_t       freqShift;     /* Frequency domain shift nshift */
    uint8_t        freqHopping;   /* Frequency hopping bhop */
    uint8_t        groupOrSeqHopping;
    uint8_t        resourceType;
    uint16_t       nTsrs;
    uint16_t       nToffset;
    Beamforming    beamForming;   /* The beamforming PDU is included in the PRACH */
}SrsPduInfo;


typedef struct {
    /* 0: PRACH PDU; 1: PUSCH PDU; 2: PUCCH PDU; 3: SRS PDU */
    uint16_t       pduType;        
    uint16_t       pduSize;
    union
    {
      PrachPduInfo   prachPduInfo;
      PuschPduInfo   puschPduInfo; 
      PucchPduInfo   pucchPduInfo; 
      SrsPduInfo     srsPduInfo; 
    };
}UlTtiPduInfo;

typedef struct {
    uint16_t        numSFN;
    uint16_t        numSlot;
    uint16_t        numPdus;
    uint8_t         ulTypes;                          
    uint16_t        pdusPerType[MAX_UL_PDU_TYPES];  
    uint8_t         ueGroupNum; 
    UlTtiPduInfo    ulTtiPduInfo[MAX_UL_PDU_NUM];     
    UeGroupInfo     ueGroupInfo[MAX_GROUP_NUM];     
}UlTtiRequest;


/* RACH.indication Message, There can be more than one RACH.indication message per slot*/
typedef struct {	
    uint8_t         preambleIndex;
    uint16_t        timingAdvance;
    uint32_t        preamblePwr;
    uint8_t         preambleSnr;      
}PrachMeasPerId;	
	
typedef struct {	
    uint16_t        handle;
    uint8_t         startSymbolIndex;
    uint8_t         slotIndex;
    uint8_t         raIndex;      
    uint8_t         avgRssi;
    uint8_t         avgSnr;
    uint8_t         numPreambles;
    PrachMeasPerId  prachMesaPerId[MAX_PREAMBLES_NUM];
}PrachMeasPerPdu;	
	
typedef struct {	
    uint16_t        numSFN;
    uint16_t        numSlot;
    uint8_t         numPdus;
    PrachMeasPerPdu prachMeasPerPdu[MAX_UL_PDU_NUM];
}PrachIndication;	
