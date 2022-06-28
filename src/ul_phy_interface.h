#include "../inc/common_teypedef.h"
#include "../inc/common_macro.h"


typedef struct
{
    uint16_t pduType;
    uint16_t pduSize;
}PduStruct;

typedef struct 
{
    /* PRACH Configuration Index */
    uint16_t prachCfgIndex;
    /* PRACH prach-frequency-start */
    uint16_t prachFdmStart;
    uint16_t antNum;
    uint8_t  preambleFormat;
    uint8_t  prachScs;
    uint16_t nLRA;
    uint32_t nNu;  
    uint16_t nN_CP_RA;
    uint16_t nRepeat;
    uint8_t  restrictedSetType;
    uint16_t prachRootSeqIdx;  
    uint16_t zeroCorrelationZoneConfig;
    uint16_t nN_cs;
    uint16_t nRootSeq; // the number of root seq. PRACH_PREAMBLE_SEQ_NUM is the max value.
    uint16_t nSlotPRACH; // Number of PRACH slots in a subframe.
    uint16_t nOccasions; // Number of PRACH occasions in a RACH slot.
    uint16_t nFFTSize;
    uint16_t nTA; // calculate from N_TA = TA* 16*64* Tc /2^u
    uint16_t startingSym; // start symbol of first occasion in a RACH slot.
    //RootSequenceTable sRootSeqTable[PRACH_PREAMBLE_SEQ_NUM]; // store the root number. PRACH_PREAMBLE_SEQ_NUM is the max value.
}PrachRxParaStruct;
