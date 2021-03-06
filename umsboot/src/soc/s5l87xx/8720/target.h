#ifndef __SOC_S5L87XX_8720_TARGET_H__
#define __SOC_S5L87XX_8720_TARGET_H__

#include "soc/s5l87xx/target.h"
#include "cpu/arm/old/v6/arm1176jzf-s/target.h"

#define CACHEALIGN_BITS 5
#define DMAALIGN_BITS 5
#define DEFAULT_SRAM_SIZE 0x2c000
#define DEFAULT_SDRAM_SIZE 0x02000000
#define DEFAULT_PAGETABLE_BASEADDR 0x2202c000
#define SYNOPSYSOTG_TURNAROUND 5
#define SYNOPSYSOTG_AHB_BURST_LEN 5
#define SYNOPSYSOTG_CLOCK 0x11

#endif
