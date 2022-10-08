#pragma once
#include "../../../../common/inc/common_typedef.h"
#include "../../../../common/inc/common_macro.h"
#include "../../../../common/inc/fapi_mac2phy_interface.h"

#define MAX_SRS_PORTNUM         2
#define MAX_SRS_SYMNUM          4
#define MAX_SRS_SYMOFFSET       6
#define MAX_SRS_GROUPNUM        8
#define MAX_SRS_UENUM_PERGROUP  8
#define MAX_SRS_UENUM           8
#define MAX_SRS_RBNUM           272

enum uint8_t
{
    SRS_SYMBLO_NoMerge        = 0,
    SRS_SYMBLOMerge_TYPE1     = 1,      
    SRS_SYMBLOMerge_TYPE2     = 2, 
	SRS_SYMBLOMerge_TYPE3     = 3, 
    SRS_SYMBLOMerge_TYPESIZE
};

typedef struct
{
    uint8_t   freAgcSwitch;                   /* 频域agc操作开关，取值0或1，默认值0，表示agc功能关闭 */ 
	uint8_t   noiseSwitch;                    /* 窗内降噪开关，取值0或1，默认值0，表示窗内降噪功能关闭 */
	uint8_t   noiseThr;                       /* 窗内降噪门限，取值3或5 */ 
	uint8_t   hRBMeth;                        /* RB级H获取方法，测量H上报模块指示RE级H到RB级H的组合方式，取值0:平均，1:抽取 */ 
	
	uint8_t   numD;                           /* 重心法档位，测量TA模块指示参与重心法估计的信号抽头点数，取值1,2或3，默认取值1 */
	uint8_t   rsvd[3];
}SrsAlgoParam;

typedef struct
{
	uint8_t   pduIdxInner;					  /* DSP不使用，只需透传到对应UE的上报接口 */
	uint8_t   antPortsNum;		    		  /* 端口个数,取值1,2 */
	uint8_t   rsvd[2];
	
	uint8_t	  cyclicShift[MAX_SRS_PORTNUM];   /* SRS循环移位，梳配置为2时取值0-7，梳配置为4时取值0-11 */
}SrsUeParam;

typedef struct
{
	uint8_t   srsGroupUeNum;            /* 本组SRS UE个数，取值1-MAX_SRS_UENUM_PERGROUP */
	uint8_t   startSymIdx;				/* SRS起始符号索引，取值8-13 */
	uint8_t   symNum;                   /* SRS符号个数，取值1-4 */
	uint8_t   combNum;                  /* SRS的梳配置，取值2或4 */
	
	uint8_t   combOffset;               /* SRS的梳偏移值，梳配置为2时取值0或1，梳配置为4时取值0-3 */
	uint8_t   srsUeMergeType; 			/* 重复合并类型指示，0:符号不合并, 1:2符号，符号0和符号1合并, 2:4符号，符号0和符号1合并,符号2和符号3合并 3:4符号，符号0,1,2,3合并 */
	uint8_t   fftSize; 			        /* 变换域降噪使用的FFT点数 */
	uint8_t   rsvd;
	
	uint16_t  rbStart;  				/* 本符号SRS RB起始位置，取值0-268 */
	uint16_t  rbNum;    				/* 本符号SRS RB个数，根据211协议Table 6.4.1.4.3-1查表得到 */
	
	uint32_t  dataOffsetAddr;           /* SRS data在DDR中相对符号基地址的偏移 */
	
	uint32_t  *baseSeqAddr;    	        /* ZC基序列在DDR中的存放地址 */ 
	
	SrsUeParam srsUeParam[MAX_SRS_UENUM_PERGROUP];

}SrsGroupParam;

typedef struct
{	
    uint8_t srsGroupNum[MAX_SRS_SYMOFFSET];                      	     /* 各符号上存在的SRS分组个数，索引值0到(MAX_SRS_SYMOFFSET -1)分别对应符号索引8-13 */
    
	SrsAlgoParam srsAlgoParam;               							 /* SRS算法配置参数 */
	
	uint32_t  *dataStartAddr[MAX_SRS_SYMOFFSET];           				 /* SRS data在DDR中的存放地址，按符号的基地址 */
	
	SrsGroupParam  srsGroupParam[MAX_SRS_SYMOFFSET][MAX_SRS_GROUPNUM];   /* 按符号分组的SRS UE解调参数 */
}SrsPara;

typedef struct
{
    uint8_t srsGroupNum[MAX_SRS_SYMOFFSET];                      	     /* 各符号上存在的SRS分组个数，索引值0到(MAX_SRS_SYMOFFSET -1)分别对应符号索引8-13 */

    FapiNrMsgSrsPduInfo fapiSrsPduInfo[MAX_SRS_UENUM];       	         /* 本小区srs的PDU信息 */
}ArmSrsParam;

typedef struct
{
	uint8_t  pduIdxInner;					  /* DSP不使用，只需透传到对应UE的上报接口 */
	uint8_t  wideBandSNR;                     /* SRS的宽带SINR，0到255代表-64dB到63dB,step为0.5dB,0xff代表无效值 */
	uint16_t TA;							  /* 根据38.213协议Section 4.2，取值0-63 */

	uint8_t  rbSNR[MAX_SRS_SYMNUM][MAX_SRS_RBNUM];      /* SRS的子带SINR，0到255代表-64dB到63dB,step为0.5dB,0xff代表无效值 */
}DspSrsRst;

typedef struct
{
	uint16_t sfn;
	uint16_t slot;

	uint8_t  srsUeNum;
    uint8_t  rsvd[3];

	DspSrsRst dspSrsRst[MAX_SRS_UENUM];
}DspSrsRpt;

typedef struct
{
	uint16_t rbNum;                     /* SRS的子带RB个数，取值0-272 */
	uint8_t  rbSNR[MAX_SRS_RBNUM];      /* SRS的子带SINR，0到255代表-64dB到63dB,step为0.5dB,0xff代表无效值 */
}SrsSinrRst;

typedef struct
{
	uint32_t handle;
	
	uint16_t rnti;
	uint16_t TA;

	uint8_t  symNum;
	uint8_t  wideBandSNR;
	uint8_t  symNumRpt;
	uint8_t  rsvd;

    SrsSinrRst srsSinrRst[MAX_SRS_SYMNUM];
}ArmSrsRst;

typedef struct
{
	uint16_t sfn;
	uint16_t slot;
	uint8_t  pduNum;
    uint8_t  rsvd;

	ArmSrsRst srsRst[MAX_SRS_UENUM];
}ArmSrsRpt;