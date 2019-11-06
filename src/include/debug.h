#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>
#include <ctype.h>

/* current print level */
#define DEBUG_LEVEL DEBUG
/* debug enablers, 1 indicate enable, 0 indecate disable */
#define DEBUGING_ENABLED 1

/* available print leveles */
#define ERROR 1
#define WARNING 2
#define INFO 3
#define DEBUG 4

#if DEBUGING_ENABLED
#define debug(level, fmt, args...) ({	\
	if (level<=DEBUG_LEVEL) {	\
		if (level == DEBUG || level == ERROR)	\
			printf(__FILE__":%d "fmt"", __LINE__, ## args);	\
		else	\
			printf(fmt, ## args);	\
			}})
#else
#define debug(level,fmt, args...)
#endif

/***************************************************************
 * @Function: dump_hex
 * @Arguments:
 *  fp:  file to dump to
 *  buf: buffer to dump
 *  len: length to dump
 * @Returns:void
 * @Description: Dumps buffer to fp in an easy-to-read farmat
 * ************************************************************/
static inline void dump_hex(FILE *fp, unsigned char const *buf, int len)
{
#if (DEBUGING_ENABLED)
	int i;
	int base;

	if (!fp)
		return;

	for (base = 0; base < len; base += 16) {
		for (i = base; i < base + 16; i++) {
			if (i < len) {
				fprintf(fp, "%02x ", (unsigned)buf[i]);
			} else {
				fprintf(fp, "	");
			}
		}
		fprintf(fp, "	");
		for (i = base; i < base + 16; i++) {
			if (i < len) {
				if (isprint(buf[i])) {
					fprintf(fp, "%c", buf[i]);
				} else {
					fprintf(fp, ".");
				}
			} else {
				break;
			}
		}
		fprintf(fp, "\n");
	}
#endif
}

#endif
