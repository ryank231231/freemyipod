#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

#include "global.h"

#define TIME_AFTER(a, b) ((long)(b) - (long)(a) < 0)
#define TIME_BEFORE(a, b) TIME_AFTER(b, a)
#define TIMEOUT_SETUP(a) (read_usec_timer() + a)
#define TIMEOUT_EXPIRED(a) TIME_AFTER(read_usec_timer(), a)

extern void time_init();
extern unsigned int read_usec_timer();
extern int64_t read_usec_timer64();
extern void udelay(unsigned int microseconds);

#endif
