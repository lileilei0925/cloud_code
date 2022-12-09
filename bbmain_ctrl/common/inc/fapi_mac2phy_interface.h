#pragma once
#include "common_typedef.h"
#include "common_macro.h"

/* P7 Prach slot messages are transmitted, or received, every slot */
typedef struct 
{
    uint8_t  trpScheme;                /* This field shall be set to 0, to identify that this table is used */
    uint16_t numPRGs;                  /* Number of PRGs spanning this allocation */
    uint16_t prgSize;                  /* Size in RBs of a precoding resource block group */
    uint8_t  digBfInterface;           /* Number of logical antenna ports (parallel streams) resulting from the Rx combining */
    uint16_t beamIndex[];              /* For number of PRGs and each digBFInterface. The size is numPRGs*digBfInterface */
} RxBeamformingInfo;

typedef struct 
{
    uint32_t handle;                   /* An opaque handle returned in the RACH.indication*/
    uint8_t  prachCfgScope;            /* 0: for PHY ID 0; 1: for current PHY ID*/
    uint16_t prachResCfgIndex;         /* The PRACH configuration for which this PRACH PDU is signaled */
    uint8_t  numFdRa;                  /* Number of frequency domain occasions, starting with indexFdRa*/
    uint8_t  startPreambleIndex;       /* Start of preamble logical index. Value:0-63; 255: all preambles from the PRACH configuration*/
    uint8_t  numPreambleIndices;       /* Number of preamble logical indices. Value:0-63; 255: all preambles from the PRACH configuration*/
} PrachParaAddInV3;

typedef struct 
{
    uint8_t numUe;                      /* Number of UE in this group For SU-MIMO, one group includes one UE only. For MU-MIMO, one group includes up to 12 UEs*/
    uint8_t pduIndex[];                 /* For each UE, an index of PDUs */;
} FapiNrUeGroupInfo;

typedef struct 
{
    uint16_t          physCellID;        /* corresponding to N_cell_ID Value：0-1007 */
    uint8_t           numPrachOcas;      /* Number of time-domain PRACH occasions Value 1-7 */
    uint8_t           prachFormat;       /* RACH format information for the PRACH occasions Value 0=0;1=1;2=2;3=3;4=A1;5=A2;6=A3;7=B1;8=B4;9=C0;10=C2;11=A1/B1;12=A2/B2;13=A3/B3*/
    uint8_t           indexFdRa;         /* Frequency domain occasion index Value:0-7 */
    uint8_t           prachStartSymbol;  /* Starting symbol for the first PRACH TD occasion in the current PRACH FD occasion*/
    uint16_t          numCs;             /* Zero-correlation zone configuration number Value: 0-419*/
    RxBeamformingInfo rxBeamFormingInfo;/* The beamforming PDU is included in the PRACH */
    PrachParaAddInV3  prachParaInV3;    /* PRACH Maintenance Parameters added in FAPIv3 */
} FapiNrMsgPrachPduInfo;

/* P7 Pusch slot messages are transmitted, or received, every slot */
typedef struct 
{
    uint16_t       ptrsPortIndex;     /* PT-RS antenna ports index */
    uint8_t        ptrsDmrsPort;      /* DMRS port corresponding to PTRS */
    uint8_t        ptrsReOffset;      /* PT-RS resource element offset value taken from */
}PtrsPortInfo;

typedef struct
{
    uint8_t        rvIndex;            /* Redundancy version index Value : 0->3*/
    uint8_t        harqProcessId;      /* HARQ process number Value: 0 ->15*/
    uint8_t        newData;            /* Value: 0: retransmission, 1: new data, i.e. initial transmission*/
    uint32_t       nTBSzie;            /* Transmit block size */
    uint32_t       numCb;              /* Number of CBs in the TB */
    uint8_t        cbPresentAndPose[]; /* CB is present in the current retx of the PUSCH. 1=PRESENT, 0=NOT PRESENT. uint8_t[ceil(numCb/8)] */
}PuschDataInfo;

typedef struct 
{
    uint16_t       harqAckBitLength;   /* Number of HARQ-ACK bits. Value: 0 -> 11 (Small block length). 12 ->1706 (Polar) */
    uint16_t       csiPart1BitLength;  /* Number of CSI part1 bits. Value: 0 -> 11 (Small block length). 12 ->1706 (Polar) */
    uint16_t       flagCsiPart2;       /* Number of CSI part2 bits. 0 -> No CSI part 2*/
    uint8_t        alphaScaling;       /* Alpha used to calculate number of coded modulation symbols per layer,Value:0 = 0.5,1 = 0.65,2 = 0.8,3 = 1 */
    uint8_t        betaOffsetHarqAck;  /* Beta Offset for HARQ-ACK bits. Value: 0->15 */
    uint8_t        betaOffsetCsi1;     /* Beta Offset for CSI-part1 bits.Value: 0->18 */
    uint8_t        betaOffsetCsi2;     /* Beta Offset for CSI-part2 bits.Value: 0->18 */
}PuschUciInfo;

typedef struct 
{
    uint8_t        numPtrsPorts;                /* Number of UL PTRS ports Value: 1->2 */
    PtrsPortInfo   ptrsPortInfo[PTRS_PORT_NUM]; 
    uint8_t        ptrsTimeDensity;             /* PT-RS time density.Value: 0: 1, 1: 2, 2: 4 */
    uint8_t        ptrsFreqDensity;             /* PT-RS frequency density. Value: 0: 2, 1: 4 */
    uint8_t        ulPtrsPower;                 /* PUSCH to PT-RS power ratio per layer per RE. Value: 0: 0dB,1: 3dB,2: 4.77dB,3: 6dB*/
}PuschPtrsInfo;

typedef struct 
{
    uint8_t        lowPaprGrpNum;               /* Group number for Low PAPR sequence generation.For DFTS-OFDM */
    uint16_t       lowPaprSeqNum;               /* sequence number of Low PAPR sequence.  For DFTS-OFDM */
    uint8_t        ulptrsSampleDensity;         /* Number of PTRS groups. */
    uint8_t        ulptrsTimeDensity;           /* Number of samples per PTRS group.*/
}PuschDftOfdmInfo;

typedef struct 
{
    uint8_t        puschTransType;             /* puschTransTypeValidity = 1 有效. Value 0: */
    uint16_t       deltabwp0fromActiveBwp;     /* The value of Bwp[0].Start - Bwp[i].start */
    uint16_t       initialUlBwpSize;
    uint8_t        groupOrSequenceHopping;     /* 0: neither, neither group or sequence hopping is enabled; 1: enable, enable group hopping and disable sequence hopping; 2: disable, disable group hopping and enable sequence hopping*/
    /* Frequency Domain Allocation */
    uint16_t       puschSecondHopPRB;          /* Index of the first PRB after intra-slot frequency hopping, as indicated by the value of Rbstart for i=1 */
    uint8_t        ldpcBaseGraph;              /* 1: LDPC base graph 1; 2: LDPC base graph 2 */
    uint32_t       tbSizeLbrmBytes;            /* Parameter TBSLBRM from 3GPP TS 38.212 section 5.4.2.1*/
}PuschParaAddInV3;

typedef struct 
{
    uint16_t       priority;                          /* Priority of the part 2 report */
    uint8_t        numPart1Params;                    /* Number of Part 1 parameters that influence the size of this part 2; values [1:4] in FAPI v3*/
    uint16_t       paramOffsets[MAX_PART1_PAPR_NUM];  /* Ordered list of parameter offsets (offset from 0 = first bit of part1); The real size is numPart1Params */
    uint8_t        paramSizes[MAX_PART1_PAPR_NUM];    /* Bitsizes of part 1 param in the same order as paramOffsets; The real size is numPart1Params */
    uint16_t       part2SizeMapIndex;                 /* Index of one of the maps configured Table 3-40, for determining the size of a part2, from the part 1 parameter values */
}Part2ReportInfo;

/* Uci information for determining UCI Part1 to Part2 correspondence, added in FAPIv3 */
typedef struct 
{
    uint16_t        numPart2s;               /* Max number of UCI part2 that could be included in the CSI report. Value: 0 -> 100 */
    Part2ReportInfo part2ReportInfo[1];
}UciInfoAddInV3;

typedef struct 
{
    uint16_t         pduBitmap;              /* Bit 0: puschData; Bit 1:puschUci; Bit 2: puschPtrs; Bit 3: dftsOfdm; All other bits reserved*/
    uint16_t         ueRnti;                 /* The RNTI used for identifying the UE when receiving the PDU*/
    uint32_t         handle;                 /* An opaque handling returned in the Rx_Data.indication and/or UCI.indication message*/
    /*BWP*/
    uint16_t         bwpSize;                /* Number of contiguous PRBs allocated to the BWP */
    uint16_t         bwpStart;               /* This field signals the bandwidth part start RB index from reference CRB*/
    uint8_t          subCarrierSpacing;      /* Value: 0-4*/
    uint8_t          cyclicPrefix;           /* Cyclic prefix type */
    /*PUSCH information always included*/
    uint16_t         targetCodeRate;         /* This is the number of information bits per 1024 coded bits expressed in 0.1 bit units */
    uint8_t          qamModOrder;            /* Value: 2,4,6,8 if transform precoding is disabled; Value: 1,2,4,6,8 if transform precoding is enabled */
    uint8_t          mcsIndex;               /* MCS index, Value: 0-31 */
    uint8_t          mcsTable;               /* 0:notqam256; 1:qam256; 2:qam64LowSE; 3:notqam256-withTransformPrecoding; 4:qam64LowSE-withTransformPrecoding */
    uint8_t          transformPrecoding;     /* Value: 0: enabled; 1: disabled*/
    uint16_t         nIdPusch;               /* parameter nID */
    uint8_t          nrOfLayers;             /* Number of layers. Value: 1-4 */
    /*DMRS*/
    uint16_t         ulDmrsSymbPos;          /* Bitmap occupying the 14 LSBs.  bit 0: first symbol and for each bit. 0: no DMRS, 1: DMRS */
    uint8_t          dmrsCfgType;            /* UL DMRS config type. 0: type 1, 1: type 2 */
    uint16_t         dmrsScrambleId;         /* PUSCH DMRS Scrambling-ID. It is only valid when the transform precoding for PUSCH is disabled */
    uint16_t         puschDmrsId;            /* PUSCH DMRS ID */
    uint8_t          nSCID;                  /* DMRS sequence initialization, It is only valid when the transform precoding for PUSCH is disabled */
    uint8_t          numDmrsCdmGrpsNoData;   /* Number of DM-RS CDM groups without data */
    uint16_t         dmrsPort;               /* Bitmap occupying the 12 LSBs with: bit 0: antenna port 1000, bit 11: antenna port 1011,and for each bit. 0: DMRS port not used,1: DMRS port used*/
    /* Pusch Allocation in frequency domain */
    uint8_t          resourceAlloc;          /* Resource Allocation Type. 0: Type 0, 1: Type 1 */
    uint8_t          rbBitmap[36];           /* For resource allocation type 0. This bitmap is in units of VRBs. LSB of byte 0 of the bitmap represents VRB0 */
    uint16_t         rbStart;                /* For resource allocation type 1. The starting resource block corresponds to VRB0 for this PUSCH */
    uint16_t         rbSize;                 /* For resource allocation type 1. The number of resource block within for this PUSCH */
    uint8_t          vrbToPrbMapping;        /* VRB-to-PRB-mapping. Value: 0: non-interleaved */ 
    uint8_t          intraSlotFreqhopping;   /* For resource allocation type 1. Value: 0: disabled 1: enabled*/
    uint16_t         txDCLocation;           /* The uplink Tx Direct Current location for the carrier */
    uint8_t          ulFreqShift7p5Khz;      /* Indicates whether there is 7.5 kHz shift or not. Value: 0:false 1:true */
    /* Resource Allocation in time domain */
    uint8_t          startSymbIndex;         /* Start symbol index of PUSCH mapping from the start of the slot */
    uint8_t          nrOfSymbols;            /* PUSCH duration in symbols */
    /* Optional Data only included if indicated in pduBitmap */
    PuschDataInfo    puschDataInfo;          /* Optional puschData information */
    PuschUciInfo     puschUciInfo;           /* Optional puschUci information */
    PuschPtrsInfo    puschPtrsInfo;          /* Optional puschPtrs information */
    PuschDftOfdmInfo puschDftOfdmInfo;       /* Optional dftsOfdm information */
    /* Beamforming */
    RxBeamformingInfo rxBeamFormingInfo;     /* The beamforming PDU is included in the PRACH */
    PuschParaAddInV3  puschParaAddInV3;      /* PUSCH Maintenance Parameters added in FAPIv3 */
    UciInfoAddInV3    uciInfoAddInV3;        /* Optional puschUci added in FAPIv3 */
}FapiNrMsgPuschPduInfo;

/* P7 Pucch slot messages are transmitted, or received, every slot */
typedef struct 
{
    uint8_t        maxCodeRate;
    uint8_t        ulBwpId;
}PucchParaAddInV3;

typedef struct 
{
    uint16_t        ueRnti;                /* The RNTI used for identifying the UE when receiving the PDU */
    uint32_t        handle;                /* An opaque handling returned in the UCI.indication message */
    /* BWP */
    uint16_t        bwpSize;               /* Number of contiguous PRBs allocated to the BWP */
    uint16_t        bwpStart;              /* This field signals the bandwidth part start RB index from reference CRB*/
    uint8_t         subcarrierSpacing;     /* Value: 0-4*/
    uint8_t         cyclicPrefix;          /* Cyclic prefix type */
    uint8_t         formatType;            /* PUCCH Format Type */
    uint8_t         multiSlotTxIndicator;  /* This field is to flush, keep or combine the buffer used for multiple-slot PUCCH transmissions  */
    uint8_t         pi2BpskFlag;           /* indicates that the UE uses pi/2 BPSK for UCI symbols instead of QPSK for PUCCH */
    /* Pucch Allocation in frequency domain */
    uint16_t        prbStart;              /* The starting PRB within the BWP for this PUCCH, or first PRB prior to hopping */
    uint16_t        prbSize;               /* The number of PRBs within this PUCCH */
    uint8_t         StartSymbolIndex;      /* Start symbol index of PUCCH from the start of the slot */
    uint8_t         numSymbols;            /* PUCCH duration in symbols  */
    /* Hopping information */
    uint8_t         intraSlotFreqHopping;  /* Intra-slot Frequency hopping for a PUCCH resource */
    uint16_t        secondHopPRB;          /* Index of the first PRB after frequency hopping */
    uint8_t         groupOrSequenceHopping;/* 0: neither, neither group or sequence hopping is enabled; 1: enable, enable group hopping and disable sequence hopping; 2: disable, disable group hopping and enable sequence hopping*/
    uint8_t         obsolete8bit;          /* This flag is obsolete in FAPIv3 */
    uint16_t        nIdPucchHopping;       /* The parameter nID used for sequence hopping */
    uint16_t        initCyclicShift;       /* Initial cyclic shift (M0) used as part of frequency hopping */
    uint16_t        nIdPucchScrambling;    /* parameter nID in [3GPP TS 38.211 sec 6.3.2.5.1 and 6.3.2.6.1] */
    uint8_t         tdOccIdx;              /* An index of an orthogonal cover code */
    uint8_t         preDftOccIdx;          /* An index of an orthogonal cover code */
    uint8_t         preDftOccLen;          /* A length of an orthogonal cover code */
    uint8_t         addDmrsFlag;           /* flag for additional DMRS; 0: disable;  1:enable */
    uint16_t        dmrsScramblingId;      /* Scrambling-ID-0 */
    uint8_t         dmrsCyclicShift;       /* Cyclic shift index for DMRS, M0 */
    uint8_t         srFlag;                /* Indicates whether there is an SR opportunity in the UCI */
    uint16_t        bitLenHarq;            /* Bit length of HARQ payload Valid for all formats */
    uint16_t        csiPart1BitLength;     /* Bit length of CSI part 1 payload. Valid for formats 2, 3 and 4 */

    RxBeamformingInfo rxBeamFormingInfo;   /* The beamforming PDU is included in the PRACH */
    PucchParaAddInV3  pucchParaAddInV3;    /* PUCCH Maintenance Parameters added in FAPIv3 */
    UciInfoAddInV3    uciInfoAddInV3;      /* Optional pucchUci added in FAPIv3 */
}FapiNrMsgPucchPduInfo;

/* P7 Srs slot messages are transmitted, or received, every slot */
typedef struct 
{
    uint16_t        ueRnti;                /* The RNTI used for identifying the UE when receiving the PDU */
    uint32_t        handle;                /* An opaque handling returned in the UCI.indication message */
    /* BWP */
    uint16_t        bwpSize;               /* Number of contiguous PRBs allocated to the BWP */
    uint16_t        bwpStart;              /* This field signals the bandwidth part start RB index from reference CRB*/
    uint8_t         subcarrierSpacing;     /* Value: 0-4*/
    uint8_t         cyclicPrefix;          /* Cyclic prefix type */
    uint8_t         numAntPorts;           /* Number of antenna ports 0:1Port; 1:2Port; 2:4Port */
    uint8_t         numSymbols;            /* Number of symbols,   0:1symbol; 1:2symbol; 2:4symbol */
    uint8_t         numRepetitions;        /* Repetition factor 𝑅, 0: 1; 1: 2; 2: 4 */
    uint8_t         timeStartPosition;     /* Starting position in the time domain 𝑙0 */
    uint8_t         configIndex;           /* SRS bandwidth config index Csrs */
    uint16_t        sequenceId;            /* SRS sequence ID */
    uint8_t         bandwidthIndex;        /* SRS bandwidth index 𝐵srs */
    uint8_t         combSize;              /* Transmission comb size 𝐾TC */
    uint8_t         combOffset;            /* Transmission comb offset Ktc_bar */
    uint8_t         cyclicShift;           /* Cyclic shift Ncs */
    uint8_t         frequencyPosition;     /* Frequency domain position nRRC */
    uint16_t        frequencyShift;        /* Frequency domain shift n_shift */
    uint16_t        frequencyHopping;      /* Frequency hopping bhop */
    uint8_t         groupOrSequenceHopping;/* 0: neither, neither group or sequence hopping is enabled; 1: enable, enable group hopping and disable sequence hopping; 2: disable, disable group hopping and enable sequence hopping*/
    uint8_t         resourceType;          /* Type of SRS resource allocation, 0: aperiodic, 1: semi-persistent, 2: periodic  */
    uint16_t        Toffset;               /* Slot offset value  */

    RxBeamformingInfo rxBeamFormingInfo;   /* The beamforming PDU is included in the PRACH */
}FapiNrMsgSrsPduInfo;

typedef struct 
{
    uint16_t pduType;  /* 0: PRACH PDU; 1: PUSCH PDU; 2: PUCCH PDU; 3: SRS PDU */  
    uint16_t pduSize;
    union
    {
      FapiNrMsgPrachPduInfo prachPduInfo;
      FapiNrMsgPuschPduInfo puschPduInfo; 
      FapiNrMsgPucchPduInfo pucchPduInfo; 
      FapiNrMsgSrsPduInfo   srsPduInfo; 
    };
}FapiNrMsgUlTtiPduInfo;

typedef struct
{
    uint16_t              numSFN;
    uint16_t              numSlot;
    uint16_t              numPdus;                       /* Number of PDUs that are included in this message. */
    uint8_t               numUlTypes;                    /* Maximum number of UL PDU types supported by UL_TTI.request. nUlPduTypes = 5 in this release.*/    
    uint16_t              pdusPerType[MAX_UL_PDU_TYPES]; /* Number of PDUs of each type that are included in this message. 0:prach; 1:pusch; 2:pucch_01; 3: pucch_234; 4:srs*/ 
    uint8_t               ueGroupNum;                    /* Number of UE Groups included in this message */
    FapiNrMsgUlTtiPduInfo ulTtiPduInfo[MAX_UL_PDU_NUM];  /* PDU info in this message */   
    FapiNrUeGroupInfo     ueGroupInfo[MAX_GROUP_NUM];    /* Group info in this message */   
}FapiNrUlTtiRequest;

/* RACH.indication Message, There can be more than one RACH.indication message per slot*/
typedef struct 
{	
    uint8_t         preambleIndex;      /* Preamble Index */
    uint16_t        timingAdvance;      /* Timing advance for PRACH */
    uint32_t        preamblePwr;        /* Preamble Received power in dBm */
    uint8_t         preambleSnr;        /* Preamble SNR in dB */
}PrachMeasPerId;	
	
typedef struct 
{	
    uint16_t        handle;             /* Handle for the UL_TTI.request RACH PDU to which this report is linked */
    uint8_t         startSymbolIndex;   /* The index of first symbol of the PRACH TD occasion */
    uint8_t         slotIndex;          /* The index of first slot of the PRACH TD occasion in a system frame */
    uint8_t         raIndex;            /* The index of the received PRACH frequency domain */
    uint8_t         avgRssi;            /* Average value of RSSI in dB */
    uint8_t         avgSnr;             /* Average value of SNR in dB */
    uint8_t         numPreambles;       /* Number of detected preambles in the PRACH occasion */
    PrachMeasPerId  prachMesaPerId[];   /* For each preamble */
}PrachMeasPerPdu;	
	
typedef struct 
{	
    uint16_t        numSFN;             /* system frame number 0->1023 */
    uint16_t        numSlot;            /* SLOT number 0->159 */
    uint8_t         numPdus;            /* Number of Measurement PDUs included in this message*/
    PrachMeasPerPdu prachMeasPerPdu[];  /* For each Measurement PDU */
}FapiNrPrachIndication;	

typedef struct
{
	uint8_t		SRindication;					/* SR检测结果，0:未检测到SR,1:检测到SR */
	uint8_t		SRconfidenceLevel;				/* SR检测置信度，0:置信,1:不置信,无效值255 */
}SRInfoFmt01;

typedef struct
{
	uint16_t	SrBitLen;						/* SR比特长度，取值范围[1,8] */
	uint8_t		SrPayload;						/* SR码流 */	
    uint8_t		rsvd;
}SRInfoFmt23;

typedef struct
{
	uint8_t		NumHarq;						/* HARQ比特个数，取值1或2 */
	uint8_t		HarqconfidenceLevel;			/* HARQ检测置信度，0:置信,1:不置信,无效值255 */
	uint8_t 	HarqValue[2];					/* HARQ解调结果，0:ACK,1:NACK,2:DTX */
}HARQInfoFmt01;

typedef struct
{
	uint8_t		HarqCrc;						/* HARQ CRC结果，0:pass,1:fail,2:not present */
	uint16_t	HarqBitLen;						/* HARQ比特长度，取值范围[1,1706] */
	
    uint8_t 	HarqPayload[48];				/* HARQ码流 */
}HARQInfoFmt23;

typedef struct
{
	uint8_t		CsiPart1Crc;					/* CsiPart1 CRC结果，0:pass,1:fail,2:not present */
	uint16_t	CsiPart1BitLen;					/* CsiPart1比特长度，取值范围[1,1706] */
	
    uint8_t 	CsiPart1Payload[48];			/* CsiPart1码流 */
}CSIpart1Info;

typedef struct
{
	uint8_t		CsiPart2Crc;					/* CsiPart2 CRC结果，0:pass,1:fail,2:not present */
	uint16_t	CsiPart2BitLen;					/* CsiPart2比特长度，取值范围[1,1706] */
	
    uint8_t 	CsiPart2Payload[48];			/* CsiPart2码流 */
}CSIpart2Info;

typedef struct
{
	uint8_t  pduBitmap;       					/* bit0:not used,bit1:HARQ,bit2:CSI Part 1,bit3:CSI Part 2,其他比特位清0。0:存在,1:不存在 */
    
    uint32_t Handle;
    
    uint16_t RNTI;    							/* UE的RNTI */
    uint8_t  UL_CQI;							/* SNR,取值范围[0,255],代表-64dB到63dB,步长0.5dB，无效值255 */
    
    uint16_t TA;								/* UE的TA值,取值范围[0,63],213协议4.2节,无效值65535 */
    uint16_t RSSI;								/* 取值范围[0,1280],步长0.1dB */
	
	HARQInfoFmt23  harqInfoFmt23;
	
	CSIpart1Info   csipart1Info;
	
	CSIpart2Info   csipart2Info;
}FapiNrPushUciIndication;

typedef struct
{
	uint8_t  pduBitmap;       					/* bit0:SR,bit1:HARQ,其他比特位清0。0:存在,1:不存在 */
    
    uint32_t Handle;
    
    uint16_t RNTI;    							/* UE的RNTI */
    uint16_t PucchFormat;                       /* PUCCH格式，0：格式0,1：格式1 */
    uint8_t  UL_CQI;							/* SNR,取值范围[0,255],代表-64dB到63dB,步长0.5dB，无效值255 */
    
    uint16_t TA;								/* UE的TA值,取值范围[0,63],213协议4.2节,无效值65535。固定填无效值65535 */
    uint16_t RSSI;								/* 取值范围[0,1280],步长0.1dB,无效值65535。固定填无效值65535 */
	
    SRInfoFmt01    srInfoFmt01;

	HARQInfoFmt01  harqInfoFmt01;
}FapiNrPucchFmt01Indication;

typedef struct
{
	uint8_t  pduBitmap;       					/* bit0:SR,bit1:HARQ,bit2:CSI Part 1,bit3:CSI Part 2,其他比特位清0。0:存在,1:不存在 */
    
    uint32_t Handle;
    
    uint16_t RNTI;    							/* UE的RNTI */
    uint16_t PucchFormat;                       /* PUCCH格式，0：格式2,1：格式3 */
    uint8_t  UL_CQI;							/* SNR,取值范围[0,255],代表-64dB到63dB,步长0.5dB，无效值255 */
    
    uint16_t TA;								/* UE的TA值,取值范围[0,63],213协议4.2节,无效值65535 */
    uint16_t RSSI;								/* 取值范围[0,1280],步长0.1dB */
	
    SRInfoFmt23    srInfoFmt23;

	HARQInfoFmt23  harqInfoFmt23;
	
	CSIpart1Info   csipart1Info;
	
	CSIpart2Info   csipart2Info;
}FapiNrPucchFmt23Indication;