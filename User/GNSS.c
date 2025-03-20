

/***************************** Include Files *******************************/
#include "GNSS.h"

/************************** Function Definitions ***************************/

uint32_t GET_GNSS_REG(uint32_t address)
{
	uint32_t *data;
	data = (unsigned int *)address;
	return *data;
}


 
uint32_t Get_GNSS_TIME(uint32_t *BuffAddr)
{
	BuffAddr[0] = GET_GNSS_REG(REG_GNSS_YMD		);	
	BuffAddr[1] = GET_GNSS_REG(REG_GNSS_HMSM	);
	BuffAddr[2] = GET_GNSS_REG(REG_GNSS_USNS	);

}

