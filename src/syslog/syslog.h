#ifndef _YK_SYSLOG_H_
#define _YK_SYSLOG_H_

#include <syslog.h>

extern void syslog_print(char *msg, size_t len);
extern void syslog_printf(char *fmt, ...);               /* you must define _GNU_SOURCE */

#endif /* _YK_SYSLOG_H_ */
