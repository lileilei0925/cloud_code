#include "common_teypedef.h"
#include "common_macro.h"

/* P7 Prach slot messages are transmitted, or received, every slot */
typedef struct {
    uint8_t  trpScheme;                /* This field shall be set to 0, to identify that this table is used */
    uint16_t numPRGs;                  /* Number of PRGs spanning this allocation */
    uint16_t prgSize;                  /* Size in RBs of a precoding resource block group */
    uint8_t  digBfInterface;           /* Number of logical antenna ports (parallel streams) resulting from the Rx combining */
    uint16_t beamIndex[];              /* For number of PRGs and each digBFInterface. The size is numPRGs*digBfInterface */
} RxBeamformingPdu;

typedef struct {
    uint32_t handle;                   /* An opaque handle returned in the RACH.indication*/
    uint8_t  prachCfgScope;            /* 0: for PHY ID 0; 1: for current PHY ID*/
    uint16_t prachResCfgIndex;         /* The PRACH configuration for which this PRACH PDU is signaled */
    uint8_t  numFdRa;                  /* Number of frequency domain occasions, starting with indexFdRa*/
    uint8_t  startPreambleIndex;       /* Start of preamble logical index. Value:0-63; 255: all preambles from the PRACH configuration*/
    uint8_t  numPreambleIndices;       /* Number of preamble logical indices. Value:0-63; 255: all preambles from the PRACH configuration*/
} PrachParaAddInV3;

typedef struct {
    uint8_t numUe;                      /* Number of UE in this group For SU-MIMO, one group includes one UE only. For MU-MIMO, one group includes up to 12 UEs*/
    uint8_t pduIndex[];                 /* For each UE, an index of PDUs */;
} UeGroupInfo;

typedef struct {
    uint16_t         physCellID;        /* corresponding to N_cell_ID Value：0-1007 */
    uint8_t          numPrachOcas;      /* Number of time-domain PRACH occasions Value 1-7 */
    uint8_t          prachFormat;       /* RACH format information for the PRACH occasions Value 0=0;1=1;2=2;3=3;4=A1;5=A2;6=A3;7=B1;8=B4;9=C0;10=C2;11=A1/B1;12=A2/B2;13=A3/B3*/
    uint8_t          indexFdRa;         /* Frequency domain occasion index Value:0-7 */
    uint8_t          prachStartSymbol;  /* Starting symbol for the first PRACH TD occasion in the current PRACH FD occasion*/
    uint16_t         numCs;             /* Zero-correlation zone configuration number Value: 0-419*/
    RxBeamformingPdu rxBeamFormingPara; /* The beamforming PDU is included in the PRACH */
    PrachParaAddInV3 prachParaInV3;     /* PRACH Maintenance Parameters added in FAPIv3 */
} FapiNrMsgPrachPduInfo;

#if 0
/* P7 Pusch slot messages are transmitted, or received, every slot */
typedef struct {
    uint16_t       ptrsPortIndex;
    uint8_t        ptrsDmrsPort;
    uint8_t        ptrsReOffset;
}PtrsPortInfo;

typedef struct {
    uint8_t        rvIndex;            /* Redundancy version index Value : 0->3*/
    uint8_t        harqProcessId;      /* HARQ process number Value: 0 ->15*/
    uint8_t        newData;            /* Value: 0: retransmission, 1: new data, i.e. initial transmission*/
    uint32_t       nTBSzie;            /* Transmit block size */
    uint32_t       numCb;              /* Number of CBs in the TB */
    uint8_t        cbPresentAndPose[]; /* CB is present in the current retx of the PUSCH. 1=PRESENT, 0=NOT PRESENT. uint8_t[ceil(numCb/8)] */
}PuschDataInfo;

typedef struct {
    uint16_t       harqAckBitLength;   /* Number of HARQ-ACK bits. Value: 0 -> 11 (Small block length). 12 ->1706 (Polar) */
    uint16_t       csiPart1BitLength;  /* Number of CSI part1 bits. Value: 0 -> 11 (Small block length). 12 ->1706 (Polar) */
    uint16_t       flagCsiPart2;       /* Number of CSI part2 bits. 0 -> No CSI part 2*/
    uint8_t        alphaScaling;       /* Alpha used to calculate number of coded modulation symbols per layer,Value:0 = 0.5,1 = 0.65,2 = 0.8,3 = 1 */
    uint8_t        betaOffsetHarqAck;  /* Beta Offset for HARQ-ACK bits. Value: 0->15 */
    uint8_t        betaOffsetCsi1;     /* Beta Offset for CSI-part1 bits.Value: 0->18 */
    uint8_t        betaOffsetCsi2;     /* Beta Offset for CSI-part2 bits.Value: 0->18 */
}PuschUciInfo;

typedef struct {
    uint8_t        numPtrsPorts;       /* Number of UL PTRS ports Value: 1->2 */
    PtrsPortInfo   ptrsPortInfo[];     /*  */
    uint8_t        ptrsTimeDensity;    /* PT-RS time density.Value: 0: 1, 1: 2, 2: 4 */
    uint8_t        ptrsFreqDensity;    /* PT-RS frequency density. Value: 0: 2, 1: 4 */
    uint8_t        ulPtrsPower;        /* PUSCH to PT-RS power ratio per layer per RE. Value: 0: 0dB,1: 3dB,2: 4.77dB,3: 6dB*/
}PuschPtrsInfo;

typedef struct {
    uint8_t        lowPaprGrpNum;      /* Group number for Low PAPR sequence generation.For DFTS-OFDM */
    uint16_t       lowPaprSeqNum;      /* sequence number of Low PAPR sequence.  For DFTS-OFDM */
    uint8_t        ptrsSampleDensity;  /* Number of PTRS groups. */
    uint8_t        ptrsTimeDensity;    /* Number of samples per PTRS group.*/
}PuschDftOfdmInfo;

typedef struct {
    uint8_t        puschTransType;             /* puschTransTypeValidity = 1 有效. Value 0: */
    uint16_t       deltabwp0StartfromActiveBwp;
    uint16_t       initialUlBwpSize;
    uint8_t        groupOrSequenceHopping;
    /* Frequency Domain Allocation */
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
    uint16_t        pduBitmap;              /* Bit 0: puschData; Bit 1:puschUci; Bit 2: puschPtrs; Bit 3: dftsOfdm; All other bits reserved*/
    uint16_t        ueRnti;                 /* The RNTI used for identifying the UE when receiving the PDU*/
    uint32_t        handle;                 /* An opaque handling returned in the Rx_Data.indication and/or UCI.indication message*/
    /*BWP*/
    uint16_t        bwpSize;                /* Number of contiguous PRBs allocated to the BWP */
    uint16_t        bwpStart;               /* This field signals the bandwidth part start RB index from reference CRB*/
    uint8_t         subCarrierSpacing;      /* Value: 0-4*/
    uint8_t         cyclicPrefix;           /* Cyclic prefix type */
    /*PUSCH information always included*/
    uint16_t        targetCodeRate;         /* This is the number of information bits per 1024 coded bits expressed in 0.1 bit units */
    uint8_t         qamModOrder;            /* Value: 2,4,6,8 if transform precoding is disabled; Value: 1,2,4,6,8 if transform precoding is enabled */
    uint8_t         mcsIndex;               /* MCS index, Value: 0-31 */
    uint8_t         mcsTable;               /* 0:notqam256; 1:qam256; 2:qam64LowSE; 3:notqam256-withTransformPrecoding; 4:qam64LowSE-withTransformPrecoding */
    uint8_t         transformPrecoding;     /* Value: 0: enabled; 1: disabled*/
    uint16_t        nIdPusch;               /* parameter nID */
    uint8_t         nrOfLayers;             /* Number of layers. Value: 1-4 */
    /*DMRS*/
    uint16_t        ulDmrsSymbPos;          /* Bitmap occupying the 14 LSBs.  bit 0: first symbol and for each bit. 0: no DMRS, 1: DMRS */
    uint8_t         dmrsCfgType;            /* UL DMRS config type. 0: type 1, 1: type 2 */
    uint16_t        dmrsScrambleId;         /* PUSCH DMRS Scrambling-ID. It is only valid when the transform precoding for PUSCH is disabled */
    uint16_t        puschDmrsidentity;      /* PUSCH DMRS ID */
    uint8_t         nSCID;                  /* DMRS sequence initialization, It is only valid when the transform precoding for PUSCH is disabled */
    uint8_t         numDmrsCdmGrpsNoData;   /* Number of DM-RS CDM groups without data */
    uint16_t        dmrsPort;               /* Bitmap occupying the 12 LSBs with: bit 0: antenna port 1000, bit 11: antenna port 1011,and for each bit. 0: DMRS port not used,1: DMRS port used*/
    /* Pusch Allocation in frequency domain */
    uint8_t         resourceAlloc;          /* Resource Allocation Type. 0: Type 0, 1: Type 1 */
    uint8_t         rbBitmap[36];           /* For resource allocation type 0. This bitmap is in units of VRBs. LSB of byte 0 of the bitmap represents VRB0 */
    uint16_t        rbStart;                /* For resource allocation type 1. The starting resource block corresponds to VRB0 for this PUSCH */
    uint16_t        rbSize;                 /* For resource allocation type 1. The number of resource block within for this PUSCH */
    uint8_t         vrbToPrbMapping;        /* VRB-to-PRB-mapping. Value: 0: non-interleaved */ 
    uint8_t         intraSlotFreqhopping;   /* For resource allocation type 1. Value: 0: disabled 1: enabled*/
    uint16_t        txDirectCurrentLocation;/* The uplink Tx Direct Current location for the carrier */
    uint8_t         ulFreqShift7p5Khz;      /* Indicates whether there is 7.5 kHz shift or not. Value: 0:false 1:true */
    /* Resource Allocation in time domain */
    uint8_t         startSymbIndex;         /* Start symbol index of PUSCH mapping from the start of the slot */
    uint8_t         nrOfSymbols;            /* PUSCH duration in symbols */
    /* Optional Data only included if indicated in pduBitmap */
    PuschDataInfo   puschDataInfo;          /* Optional puschData information */
    PuschUciInfo    puschUciInfo;           /* Optional puschUci information */
    PuschPtrsInfo   puschPtrsInfo;          /* Optional puschPtrs information */
    PuschDftInfo    puschDftOfdmInfo;       /* Optional dftsOfdm information */
    /* Beamforming */
    RxBeamformingPdu rxBeamFormingPara;     /* The beamforming PDU is included in the PRACH */
    PuschNewInV3    puschNewInV3;           /* PUSCH Maintenance Parameters added in FAPIv3 */
    UciInfoAddInV3  uciInfoAddInV3;         /* Optional puschUci added in FAPIv3 */
}FapiNrMsgPuschPduInfo;


/* P7 Pucch slot messages are transmitted, or received, every slot */
typedef struct {
    uint8_t        maxCodeRate;
    uint8_t        ulBwpId;
}PucchNewInV3;

typedef struct {
    /* 0: PRACH PDU; 1: PUSCH PDU; 2: PUCCH PDU; 3: SRS PDU */
    uint16_t        nRnti;        
    uint32_t        handle;
    uint16_t        bwpSize;
    uint16_t        bwpStart;
    uint8_t         subcarrierSpacing;
    uint8_t         cyclicPrefix;
    uint8_t         formatType;
    uint8_t         multiSlotTxIndicator;
    uint8_t         pi2BpskFlag;
    uint16_t        prbStart;
    uint16_t        prbSize;
    uint8_t         StartSymbolIndex;
    uint8_t         symbolNum;
    
    uint8_t         intraSlotFreqHopping;
    uint16_t        secondHopPRB;
    uint8_t         pucchGroupHopping;

    uint8_t         obsolete8bit;    /* This flag is obsolete in FAPIv3 */
    uint16_t        nIdPucchHopping; /* The parameter nID used for sequence hopping */
    uint16_t        initCyclicShift;
    uint16_t        nIdPucchScrambling;
    uint8_t         tdOccIdx;
    uint8_t         preDftOccIdx;
    uint8_t         preDftOccLen;
    uint8_t         addDmrsFlag;
    uint16_t        dmrsScramblingId;
    uint8_t         dmrsCyclicShift;
    uint8_t         srFlag;
    uint16_t        bitLenHarq;
    uint16_t        csiPart1BitLength;
   
    BeamformingInfo beamFormingInfo;    /* The beamforming PDU is included in the PRACH */
    PucchNewInV3    pucchNewInV3;
    UciInfoAddInV3  uciInfoAddInV3;
}FapiPucchPduInfo;


/* P7 Srs slot messages are transmitted, or received, every slot */
typedef struct {
    uint16_t        nRnti;
    uint32_t        handle;
    uint16_t        bwpSize;
    uint16_t        bwpStart;
    uint8_t         subcarrierSpacing;
    uint8_t         cyclicPrefix;
    uint8_t         numAntPorts;
    uint8_t         numSymbols;
    uint8_t         numRepetitions;
    uint8_t         startSymbol;
    uint8_t         nCsrs;         /* SRS bandwidth config index corresponding FAPI V3  configIndex*/
    uint16_t        sequenceId;
    uint8_t         nBsrs;         /* SRS bandwidth index, corresponding FAPI V3  bandwidthIndex*/
    uint8_t         combSize;      /* Transmission comb size Ktc*/
    uint8_t         combOffset;
    uint8_t         cyclicShift;
    uint8_t         freqPosition;  /* Frequency domain position nRRC*/
    uint16_t        freqShift;     /* Frequency domain shift nshift */
    uint8_t         freqHopping;   /* Frequency hopping bhop */
    uint8_t         groupOrSeqHopping;
    uint8_t         resourceType;
    uint16_t        nTsrs;
    uint16_t        nToffset;
    BeamformingInfo beamForming;   /* The beamforming PDU is included in the PRACH */
}FapiSrsPduInfo;

typedef struct {
    uint16_t pduType;  /* 0: PRACH PDU; 1: PUSCH PDU; 2: PUCCH PDU; 3: SRS PDU */  
    uint16_t pduSize;
    union
    {
      FapiNrMsgPrachPduInfo prachPduInfo;
      FapiNrMsgPrachPduInfo puschPduInfo; 
      FapiNrMsgPrachPduInfo pucchPduInfo; 
      FapiNrMsgPrachPduInfo srsPduInfo; 
    };
}FapiNrMsgUlTtiPduCfgInfo;

typedef struct {
    uint16_t         numSFN;
    uint16_t         numSlot;
    uint16_t         numPdus;
    uint8_t          ulTypes;                          
    uint16_t         pdusPerType[MAX_UL_PDU_TYPES];  
    uint8_t          ueGroupNum; 
    FapiUlTtiPduInfo ulTtiPduInfo[MAX_UL_PDU_NUM];     
    FapiUeGroupInfo  ueGroupInfo[MAX_GROUP_NUM];     
}FapiUlTtiRequest;
#endif 

/* RACH.indication Message, There can be more than one RACH.indication message per slot*/
typedef struct {	
    uint8_t         preambleIndex;      /* Preamble Index */
    uint16_t        timingAdvance;      /* Timing advance for PRACH */
    uint32_t        preamblePwr;        /* Preamble Received power in dBm */
    uint8_t         preambleSnr;        /* Preamble SNR in dB */
}PrachMeasPerId;	
	
typedef struct {	
    uint16_t        handle;             /* Handle for the UL_TTI.request RACH PDU to which this report is linked */
    uint8_t         startSymbolIndex;   /* The index of first symbol of the PRACH TD occasion */
    uint8_t         slotIndex;          /* The index of first slot of the PRACH TD occasion in a system frame */
    uint8_t         raIndex;            /* The index of the received PRACH frequency domain */
    uint8_t         avgRssi;            /* Average value of RSSI in dB */
    uint8_t         avgSnr;             /* Average value of SNR in dB */
    uint8_t         numPreambles;       /* Number of detected preambles in the PRACH occasion */
    PrachMeasPerId  prachMesaPerId[];   /* For each preamble */
}PrachMeasPerPdu;	
	
typedef struct {	
    uint16_t        numSFN;             /* system frame number 0->1023 */
    uint16_t        numSlot;            /* SLOT number 0->159 */
    uint8_t         numPdus;            /* Number of Measurement PDUs included in this message*/
    PrachMeasPerPdu prachMeasPerPdu[];  /* For each Measurement PDU */
}FapiPrachIndication;	
