#ifndef _YK_SIGNAL_H_
#define _YK_SIGNAL_H_

#include <features.h>
__BEGIN_DECLS

#include <signal.h>
#include <stdarg.h>

static inline void sigset_init(sigset_t *s, ...) {
	va_list args;
	sigemptyset(s);
	va_start(args, s); 
	while(1) { 
		int signo = va_arg(args, int); 
		if(signo <= 0) 
			break;
		sigaddset(s, signo);
	} 
	va_end(args);
}

#define DEFINE_SIGSET(name,...) \
sigset_t name; \
do { \
	sigset_init(&name, ##__VA_ARGS__); \
}while(0)

void yk_signal_print_list(void);
#define yk_signal_list  yk_signal_print_list
void yk_signal_dump_sigset(sigset_t *s);
void yk_signal_register_sigset(sigset_t *s);

__END_DECLS
#endif /* _YK_SIGNAL_H */
