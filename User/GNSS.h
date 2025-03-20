
#ifndef GNSS_H
#define GNSS_H


/****************** Include Files ********************/
#include "stdint.h"
#include "STAR.h"

#define	GNSS_IP_BASEADDR AHB_TARGEXP0_BASE	//GNSS IP基地址，也就是AHB总线0的地址
/****************** Memory Map ********************/
// 8'h00: rdata =  year_month_day_w32; 		//0x60000010
// 8'h04: rdata =  hour_min_sec_ms_w32;		//0x60000014
// 8'h0c: rdata =  {12'h000,us_ns_data};	//0x60000018
#define REG_GNSS_YMD	(GNSS_IP_BASEADDR+0x10)
#define REG_GNSS_HMSM	(GNSS_IP_BASEADDR+0x14)
#define REG_GNSS_USNS	(GNSS_IP_BASEADDR+0x18)

#define GNSS_DATA_LENGTH		3	//


uint32_t GET_GNSS_REG(uint32_t address);
uint32_t Get_GNSS_TIME(uint32_t *BuffAddr);


#endif // GNSS_H
