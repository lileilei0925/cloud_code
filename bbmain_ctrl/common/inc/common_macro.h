#ifndef COMMON_MACRO_ 
#define COMMON_MACRO_

#include "common_typedef.h"

#define ERROR_CODE                   0

#define MAX_CELL_NUM                 4
#define MAX_RX_ANT_NUM               4
#define MAX_RX_ANT_PORT_NUM          4

#define MAX_PRACH_PDU_NUM            2
#define MAX_PUSCH_PDU_NUM            16
#define MAX_PUCCH_PDU_NUM            50
#define MAX_SRS_PDU_NUM              15

#define UL_PDU_TYPE_PRACH            0
#define UL_PDU_TYPE_PUSCH            1
#define UL_PDU_TYPE_PUCCH            2
#define UL_PDU_TYPE_SRS              3

#define RM_BIT_LENGTH_MIN            3
#define RM_BIT_LENGTH_MAX            11
#define POLAR_BIT_LENGTH_MIN         12
#define POLAR_BIT_LENGTH_MAX         359


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

enum TLV_BODY_TYPE
{
    PUSCH_UCI_INDICATION    = 0,  
    PUCCH_FMT01_INDICATION  = 1,  
	PUCCH_FMT23_INDICATION  = 2,
	TLV_BODY_TYPE_NUM
};

#define FRAME_DUPLEX_TYPE_FDD        0
#define FRAME_DUPLEX_TYPE_TDD        1
#define SLOT_NUM_PER_FRAME           20
#define SYM_NUM_PER_SLOT             14
#define N_SC_PER_PRB                 12

/* Prach  start*/
#define    UN_USED_ROOT_PER_FDM      1

#define    PRACH_ZC_LEN_139          139
#define    PRACH_ZC_LEN_839          839
#define    PRACH_FFT_LEN_256         1
#define    PRACH_FFT_LEN_1536        0

#define    PRACH_FORMAT_0            0
#define    PRACH_FORMAT_1            1
#define    PRACH_FORMAT_2            2
#define    PRACH_FORMAT_3            3
#define    PRACH_FORMAT_A1           4
#define    PRACH_FORMAT_A2           5
#define    PRACH_FORMAT_A3           6
#define    PRACH_FORMAT_B1           7
#define    PRACH_FORMAT_B2           255 
#define    PRACH_FORMAT_B3           255 
#define    PRACH_FORMAT_B4           8
#define    PRACH_FORMAT_C0           9
#define    PRACH_FORMAT_C2           10

#define    PRACH_FORMAT_NUM_139      7
#define    PRACH_FORMAT_NUM_839      4
#define    PRACH_FORMAT_NUM          PRACH_FORMAT_NUM_139 + PRACH_FORMAT_NUM_839
#define    PRACH_CONFIG_TABLE_SIZE   256
#define    MAX_PRACH_FDM_NUM         2
#define    MAX_PRACH_TDM_NUM         2
#define    MAX_PRACH_OCCASION_NUM    MAX_PRACH_FDM_NUM * MAX_PRACH_TDM_NUM
#define    MAX_DET_WIN_NUM           3
#define    MAX_ROOT_PER_OCCASION     32

#define    UNRESTRICTED              0
#define    RESTRICTED_TYPE_A         1
#define    RESTRICTED_TYPE_B         2
#define    PRACH_PREAMBLE_SEQ_NUM    64 
#define    MAX_PRACH_ROOT_NUM        32 
/* Prach  end*/

#define HOP_NUM                      2
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
#define NR_PUSCH_MAX_DRMS_NUM        3
#define NR_PUSCH_UE_PORT_NUM         4
#define NR_BASE_GRAPH_NUM            2
#define NR_RVID_NUM                  4

#define NR_DMRS_TYPE1_SC_PER_RB      6
#define NR_DMRS_TYPE2_SC_PER_RB      4

#define NR_PUSCH_MAX_UE_NUM_PER_SLOT 16
#define NR_PUSCH_MAX_PRG_NUM         137

#define NR_ULSCH_WITHOUT_UCI         0   /* PUSCH Data Only */
#define NR_UCI_HARQ_ACK              1   /* HARQ_ACK  */
#define NR_UCI_CSI_PART1             2   /* CSI_PART1 */
//#define NR_UCI_CSI_PART2             3   /* CSI_PART2 */
#define NR_UCI_WITH_ALL              4   /* HARQ_ACK + CSI_PART1 + CSI_PART2 */
#define NR_UCI_WITHOUT_HARQ_ACK      5   /* CSI-Part1 + CSI-Part2 */
#define NR_UCI_WITHOUT_PART2         6   /* HAQK-ACK + CSI-Part1 */
#define NR_ULSCH_WITH_PART2          7   /* Data + CSI-Part2 */
#define NR_ULSCH_WITHOUT_PART2       8   /* Data + UCI WithOut CSI-Part2 */

#define NR_PUSCH_CSIPART2_EXT        0
#define NR_PUSCH_ULSCH_EXT           1

#define MAX_BIT_FGH                 8*20*2  
#define MAX_BIT_V                   2*20    
#define MAX_BIT_NCS                 8*14*20 

#define _extu(v1, csta, cstb)     (((uint32_t)(v1) << (csta)) >> (cstb))
#define MAX_POLAR_CB_NUM             2
#endif