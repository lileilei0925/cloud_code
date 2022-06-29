#include "../inc/common_teypedef.h"
#include "../inc/common_macro.h"

typedef struct
{
    uint16_t pduType;
    uint16_t pduSize;
} PduHeadInfo;

typedef enum
{
    F_1_25KHz = 0,
    F_5KHz    = 1,
    F_15KHz   = 2 ,
    F_30KHz   = 3,
    F_60KHz   = 4,
    F_120KHz  = 5,
    F_ERROR
} RAScSpacingEum;

typedef enum
{
    FORMAT_0    = 0,
    FORMAT_1    = 1,
    FORMAT_2    = 2,
    FORMAT_3    = 3,
    FORMAT_A1   = 4,
    FORMAT_A2   = 5,
    FORMAT_A3   = 6,
    FORMAT_B1   = 7,
    FORMAT_B4   = 8,
    FORMAT_C0   = 9,
    FORMAT_C2   = 10,
    FORMAT_A1B1 = 11,
    FORMAT_A2B2 = 12,
    FORMAT_A3B3 = 13,
    FORMAT_ERROR
} PrachFormatEnum;

typedef struct 
{
    uint8_t prachConfigIdx;
    uint8_t preambleFmrt[2];
    uint8_t x;
    uint8_t y[2];
    uint8_t slotNr[40];
    uint8_t slotNrNum;
    uint8_t startingSym;
    uint8_t nrofPrachInSlot;
    uint8_t occassionsInPrachSlot;
    uint8_t duration;
} PrachConfigTableStruct;

/* P5 Prach config messages local structure */
typedef struct {	
    uint16_t prachResCfgIndex;
    uint8_t  prachLength;
    uint8_t  prachScs;
    uint8_t  PuschScs;      
    uint8_t  restrictedSetType;/* 0: unrestricted, 1: restricted set type A, 2: restricted set type B */
    uint8_t  PrachFdmNum;      /* Number of RACH frequency domain occasions */
    uint8_t  prachCfgIndex;
    uint8_t  SsbPerRach;
    
    uint16_t prachRootIndex[MAX_PRACH_FDM_NUM];
    uint8_t  rootSequenceNum[MAX_PRACH_FDM_NUM];
    int16_t  k1[MAX_PRACH_FDM_NUM];
    uint8_t  prachZeroCorrCfg[MAX_PRACH_FDM_NUM];
    uint16_t unusedRootNum[MAX_PRACH_FDM_NUM];
    uint16_t unusedRootSequence[MAX_PRACH_FDM_NUM][UN_USED_ROOT_PER_FDM];
}PrachCfgParaInfo;

/* P7 Prach slot messages local structure*/
typedef struct 
{
    uint16_t phyCellID;
    uint8_t  prachTdOcasNum;
    uint8_t  prachFormat;
    uint8_t  PrachFdmIndex; /* Frequency domain occasion index */     
    uint8_t  prachStartSymb;/* Starting symbol for the first PRACH TD occasion in the current PRACH FD occasion */
    uint16_t ncsValue;      /* Zero-correlation zone configuration number */

    uint32_t handle;
    uint8_t  prachCfgScope;/* 0: for PHY ID 0; 1: for current PHY ID */
    uint16_t prachResCfgIndex;/* The PRACH configuration for which this PRACH PDU is signaled  */
    uint8_t  prachFdmNum;  /* Number of frequency domain occasions,starting with PrachFdmIndex */
    uint8_t  startPreambleIndex;
    uint8_t  numPreambleIndices;

    uint8_t  trpScheme; /* This field shall be set to 0, to identify that this table is used */
    uint16_t prgNum;
    uint16_t prgSize;
    uint8_t  antPortNum;/* Number of logical antenna ports */
    uint16_t beamIndex[MAX_PRG_NUM][MAX_BF_PORT];
} PrachPduParaInfo;
