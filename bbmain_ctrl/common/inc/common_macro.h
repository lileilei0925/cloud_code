#ifndef COMMON_MACRO_ 
#define COMMON_MACRO_

#include "common_typedef.h"

#define MAX_PRACH_PDU_NUM            2
#define MAX_PUSCH_PDU_NUM            50
#define MAX_PUCCH_PDU_NUM            50
#define MAX_SRS_PDU_NUM              50

#define UL_PDU_TYPE_PRACH            0
#define UL_PDU_TYPE_PUSCH            1
#define UL_PDU_TYPE_PUCCH            2
#define UL_PDU_TYPE_SRS              3

enum UCI_TYPE
{
    PUCCH_UCI_PART1    =  0,
    PUCCH_CSI_PART2    =  1,      
    PUSCH_ACK          =  2,
    PUSCH_CSI_PART1    =  3, 
    PUSCH_CSI_PART2    =  4,    
    UCI_TYPE_NUM
 };

enum RM_TYPE
{
    REPETITION   =  0,
    PUNTURING    =  1,      
    SHORTENING   =  2,
    RM_TYPE_NUM
 };

#define FRAME_DUPLEX_TYPE_FDD        0
#define FRAME_DUPLEX_TYPE_TDD        1

#define MAX_CELL_NUM                 4
#define MAX_RX_ANT_NUM               4
#define MAX_RX_ANT_PORT_NUM          4

#define SLOT_NUM_PER_FRAME           20
#define SYM_NUM_PER_SLOT             14
#define HOP_NUM                      2
#define N_SC_PER_PRB                 12
#define MAX_PRG_NUM                  137
#define MAX_BF_PORT                  4
#define MAX_GROUP_NUM                8
#define MAX_UE_NUM_PER_GROUP         12
#define MAX_UL_PDU_NUM               200
#define MAX_UL_PDU_TYPES             5
#define MAX_DL_PDU_TYPES             5
#define MAX_PREAMBLES_NUM            64
#define MAX_PTRS_PORT_NUM            2
#define MAX_UCI_PART2_NUM            100
#define MAX_PART1_PAPR_NUM           4
#define PTRS_PORT_NUM                2
#define PTRS_PORT_NUM                2
#define MAX_CSI_PART2_REPOET_NUM     4

#define MAX_POLAR_CB_NUM             2

#define _extu(v1, csta, cstb)     (((uint32_t)(v1) << (csta)) >> (cstb))

#endif