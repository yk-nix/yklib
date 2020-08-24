#ifndef _YK_TERMIOS_H_
#define _YK_TERMIOS_H_

/* echo mode for getting password from stdin */
#define PASSWORD_ECHO_HIDE          1
#define PASSWORD_ECHO_STAR          2
extern char *yk_get_password(char *buf, size_t size, int echo_mode);
extern char *yk_get_password_echo_char(char *buf, size_t size, char ch);
#define yk_term_get_password  yk_get_password
#define yk_term_get_password_echo_char yk_get_password_echo_char

/* set terminal attributes */
#define YK_IGNBRK	1
#define YK_BRKINT	2
#define YK_IGNPAR	3
#define YK_PARMRK	4
#define YK_INPCK	5
#define YK_ISTRIP	6
#define YK_INLCR	7
#define YK_ICRNL	8
#define YK_IGNCR	9
#define YK_IUCLC	10
#define YK_IXON		11
#define YK_IXANY	12
#define YK_IXOFF	13
#define YK_OPOST	100
#define YK_OLCUC	101
#define YK_OCRNL	102
#define YK_ONLCR	103
#define YK_ONOCR	104
#define YK_ONLRET	105
#define YK_OFILL	106
#define YK_NLDLY	107
#define YK_CRDLY	108
#define YK_TABDLY	110
#define YK_BSDLY	111
#define YK_VTDLY	112
#define YK_FFDLY	113
#define YK_CSIZE	200
#define YK_CREAD	201
#define YK_CSTOPB	202
#define YK_PARENB	203
#define YK_PARODD	204
#define YK_HUPCL	205
#define YK_CLOCAL	206
#define YK_ISIG		300
#define YK_ICANON	301
#define YK_ECHO		302
#define YK_ECHOE	303
#define YK_ECHONL	304
#define YK_NOFLSH	305
#define YK_IEXTEN	306
#define YK_VEOF		400
#define YK_VEOL		401
#define YK_VERASE	402
#define YK_VINTR	403
#define YK_VKILL	404
#define YK_VMIN		405
#define YK_VQUIT	406
#define YK_VSTART	407
#define YK_VSTOP	408
#define YK_VTIME	409
extern int yk_term_attr_set(int fd, int attr, int v);
extern void yk_term_attr_dump(int fd);

extern yk_dict_t yk_term_speed_names[];
extern yk_dict_t yk_term_attr_names[];
extern yk_dict_t yk_term_csize_names[];

#endif /* _YK_TERMIOS_H_ */
