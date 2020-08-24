#include <stdio.h>
#include <string.h>
#include <termios.h>

#include <yk_lib.h>
#include <yk_termios.h>

static inline void setflag(int *ft, int flag, int on_off) {
	switch(on_off) {
	case FLAG_ON:
		*ft |= flag;
		break;
	case FLAG_OFF:
		*ft &= ~flag;
		break;
	default:
		return;
	}
	return;
}

static int _yk_tcsetflag(struct termios *t, int flag, int v) {
	switch(flag) {
	case YK_IGNBRK:
		setflag(&t->c_iflag, IGNBRK, v);
		break;
	case YK_BRKINT:
		setflag(&t->c_iflag, BRKINT, v);
		break;
	case YK_IGNPAR:
		setflag(&t->c_iflag, IGNPAR, v);
		break;
	case YK_PARMRK:
		setflag(&t->c_iflag, PARMRK, v);
		break;
	case YK_INPCK:
		setflag(&t->c_iflag, INPCK, v);
		break;
	case YK_ISTRIP:
		setflag(&t->c_iflag, ISTRIP, v);
		break;
	case YK_ICRNL:
		setflag(&t->c_iflag, ICRNL, v);
		break;
	case YK_IGNCR:
		setflag(&t->c_iflag, IGNCR, v);
		break;
	case YK_IUCLC:
		setflag(&t->c_iflag, IUCLC, v);
		break;
	case YK_IXON:
		setflag(&t->c_iflag, IXON, v);
		break;
	case YK_IXOFF:
		setflag(&t->c_iflag, IXOFF, v);
		break;
	case YK_IXANY:
		setflag(&t->c_iflag, IXANY, v);
		break;
	case YK_OPOST:
		setflag(&t->c_oflag, OPOST, v);
		break;
	case YK_OLCUC:
		setflag(&t->c_oflag, OLCUC, v);
		break;
	case YK_OCRNL:
		setflag(&t->c_oflag, OCRNL, v);
		break;
	case YK_ONLCR:
		setflag(&t->c_oflag, ONLCR, v);
		break;
	case YK_ONOCR:
		setflag(&t->c_oflag, ONOCR, v);
		break;
	case YK_ONLRET:
		setflag(&t->c_oflag, ONLRET, v);
		break;
	case YK_OFILL:
		setflag(&t->c_oflag, OFILL, v);
		break;
	case YK_CSIZE:
		t->c_cflag |= v;
		break;
	case YK_CREAD:
		setflag(&t->c_cflag, CREAD, v);
		break;
	case YK_CSTOPB:
		setflag(&t->c_cflag, CSTOPB, v);
		break;
	case YK_PARODD:
		setflag(&t->c_cflag, PARODD, v);
		break;
	case YK_HUPCL:
		setflag(&t->c_cflag, HUPCL, v);
		break;
	case YK_CLOCAL:
		setflag(&t->c_cflag, HUPCL, v);
		break;
	case YK_ISIG:
		setflag(&t->c_lflag, ISIG, v);
		break;
	case YK_ICANON:
		setflag(&t->c_lflag, ICANON, v);
		break;
	case YK_ECHO:
		setflag(&t->c_lflag, ECHO, v);
		break;
	case YK_ECHOE:
		setflag(&t->c_lflag, ECHOE, v);
		break;
	case YK_ECHONL:
		setflag(&t->c_lflag, ECHONL, v);
		break;
	case YK_NOFLSH:
		setflag(&t->c_lflag, NOFLSH, v);
		break;
	case YK_IEXTEN:
		setflag(&t->c_lflag, IEXTEN, v);
		break;
	case YK_VEOF:
		t->c_cc[VEOF] = v;
		break;
	case YK_VEOL:
		t->c_cc[VEOL] = v;
		break;
	case YK_VERASE:
		t->c_cc[VERASE] =v;
		break;
	case YK_VINTR:
		t->c_cc[VINTR] = v;
		break;
	case YK_VKILL:
		t->c_cc[VKILL] = v;
		break;
	case YK_VMIN:
		t->c_cc[VMIN] = v;
		break;
	case YK_VQUIT:
		t->c_cc[VQUIT] = v;
		break;
	case YK_VSTART:
		t->c_cc[VSTART] = v;
		break;
	case YK_VSTOP:
		t->c_cc[VSTOP] = v;
		break;
	case YK_VTIME:
		t->c_cc[VTIME] = v;
		break;
	default:
		return EXIT_FAILED;
	}
	return EXIT_SUCCESS;
}

int yk_term_attr_set(int fd, int flag, int v) {
	struct termios t, old;
	memset((void*)&old, 0, sizeof(old));
	if(tcgetattr(fd, &old)) {
		yk_fprintf(stderr, "yk_tcsetflag: tcgetattr: %m\n");
		return EXIT_FAILED;
	}
	memcpy((void*)&t, (void*)&old, sizeof(struct termios));
	if(_yk_tcsetflag(&t, flag, v) != EXIT_SUCCESS) {
		yk_fprintf(stderr, "yk_tcsetflag: unkown FALG\n");
		return EXIT_FAILED;
	}
	if(tcsetattr(fd, TCSANOW, &t)) {
		yk_fprintf(stderr, "yk_tcsetflag: tcsetattr: %m\n");
		return EXIT_FAILED;
	}
	return EXIT_SUCCESS;
}

yk_dict_t yk_term_attr_names[] = {
	{YK_IGNBRK, "ignbrk", NULL, ""},
	{YK_BRKINT, "brkint", NULL, ""},
	{YK_IGNPAR, "ignpar", NULL, ""},
	{YK_PARMRK, "parmrk", NULL, ""},
	{YK_INPCK,  "inpck",  NULL, ""},
	{YK_ISTRIP, "istrip", NULL, ""},
	{YK_INLCR,  "inlcr",  NULL, ""},
	{YK_ICRNL,  "icrnl",  NULL, ""},
	{YK_IGNCR,  "igncr",  NULL, ""},
	{YK_IUCLC,  "iuclc",  NULL, ""},
	{YK_IXON,   "ixon",   NULL, ""},
	{YK_IXOFF,  "ixoff",  NULL, ""},
	{YK_IXANY,  "ixany",  NULL, ""},
	{YK_OPOST,  "opost",  NULL, ""},
	{YK_OLCUC,  "olcuc",  NULL, ""},
	{YK_OCRNL,  "ocrnl",  NULL, ""},
	{YK_ONLCR,  "onlcr",  NULL, ""},
	{YK_ONOCR,  "onocr",  NULL, ""},
	{YK_ONLRET, "onlret", NULL, ""},
	{YK_OFILL,  "ofill",  NULL, ""},
	{YK_NLDLY,  "nldly",  NULL, ""},
	{YK_CRDLY,  "crdly",  NULL, ""},
	{YK_TABDLY, "tabdly", NULL, ""},
	{YK_BSDLY,  "bsdly",  NULL, ""},
	{YK_VTDLY,  "vtdly",  NULL, ""},
	{YK_FFDLY,  "ffdly",  NULL, ""},
	{YK_CSIZE,  "csize",  NULL, ""},
	{YK_CREAD,  "cread",  NULL, ""},
	{YK_CSTOPB, "cstopb", NULL, ""},
	{YK_PARENB, "parenb", NULL, ""},
	{YK_PARODD, "parodd", NULL, ""},
	{YK_HUPCL,  "hupcl",  NULL, ""},
	{YK_CLOCAL, "clocal", NULL, ""},
	{YK_ISIG,   "isig",   NULL, ""},
	{YK_ICANON, "icanon", NULL, ""},
	{YK_ECHO,   "echo",   NULL, ""},
	{YK_ECHOE,  "echoe",  NULL, ""},
	{YK_ECHONL, "echonl", NULL, ""},
	{YK_NOFLSH, "noflsh", NULL, ""},
	{YK_IEXTEN, "iexten", NULL, ""},
	{YK_VEOF,   "eof",    NULL, ""},
	{YK_VEOL,   "eol",    NULL, ""},
	{YK_VERASE, "erase",  NULL, ""},
	{YK_VINTR,  "intr",   NULL, ""},
	{YK_VKILL,  "kill",   NULL, ""},
	{YK_VQUIT,  "quit",   NULL, ""},
	{YK_VSTART, "start",  NULL, ""},
	{YK_VSTOP,  "stop",   NULL, ""},
	{YK_VTIME,  "time",   NULL, ""},
	{YK_KEY_INVALID, "",  NULL, ""}
};

yk_dict_t yk_term_speed_names[] = {
	{B0,      "B0",      NULL, ""},
	{B50,     "B50",     NULL, ""},
	{B75,     "B75",     NULL, ""},
	{B110,    "B110",    NULL, ""},
	{B134,    "B134",    NULL, ""},
	{B150,    "B150",    NULL, ""},
	{B200,    "B200",    NULL, ""},
	{B300,    "B300",    NULL, ""},
	{B600,    "B600",    NULL, ""},
	{B1200,   "B1200",   NULL, ""},
	{B1800,   "B1800",   NULL, ""},
	{B2400,   "B2400",   NULL, ""},
	{B4800,   "B4800",   NULL, ""},
	{B9600,   "B9600",   NULL, ""},
	{B19200,  "B19200",  NULL, ""},
	{B38400,  "B38400",  NULL, ""},
	{B57600,  "B57500",  NULL, ""},
	{B115200, "B115200", NULL, ""},
	{B230400, "B230400", NULL, ""},
	{YK_KEY_INVALID, "", NULL, ""}
};
yk_dict_t yk_term_csize_names[] = {
	{CS5, "cs5", NULL, ""},
	{CS6, "cs6", NULL, ""},
	{CS7, "cs7", NULL, ""},
	{CS8, "cs8", NULL, ""},
	{YK_KEY_INVALID, "", NULL, ""}
};

static void _yk_term_attr_print(struct termios *t, int flag) {
	switch(flag) {
	case YK_IGNBRK:
		fprintf(stdout, "%s%s", t->c_iflag & IGNBRK ? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_BRKINT:
		fprintf(stdout, "%s%s", t->c_iflag & BRKINT? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_IGNPAR:
		fprintf(stdout, "%s%s", t->c_iflag & IGNPAR? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_PARMRK:
		fprintf(stdout, "%s%s", t->c_iflag & PARMRK? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_INPCK:
		fprintf(stdout, "%s%s", t->c_iflag & INPCK? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_ISTRIP:
		fprintf(stdout, "%s%s", t->c_iflag & ISTRIP? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_INLCR:
		fprintf(stdout, "%s%s", t->c_iflag & INLCR? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_ICRNL:
		fprintf(stdout, "%s%s", t->c_iflag & ICRNL? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_IUCLC:
		fprintf(stdout, "%s%s", t->c_iflag & IUCLC? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_IGNCR:
		fprintf(stdout, "%s%s", t->c_iflag & IGNCR? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_IXON:
		fprintf(stdout, "%s%s", t->c_iflag & IXON? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_IXOFF:
		fprintf(stdout, "%s%s", t->c_iflag & IXOFF? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_IXANY:
		fprintf(stdout, "%s%s", t->c_iflag & IXANY? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_OPOST:
		fprintf(stdout, "%s%s", t->c_oflag & OPOST? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_OLCUC:
		fprintf(stdout, "%s%s", t->c_oflag & OLCUC? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_OCRNL:
		fprintf(stdout, "%s%s", t->c_oflag & OCRNL? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_ONLCR:
		fprintf(stdout, "%s%s", t->c_oflag & ONLCR? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_ONOCR:
		fprintf(stdout, "%s%s", t->c_oflag & ONOCR? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_ONLRET:
		fprintf(stdout, "%s%s", t->c_oflag & ONLRET? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_OFILL:
		fprintf(stdout, "%s%s", t->c_oflag & OFILL? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_CSIZE:
		if(t->c_cflag & CS5) 
			fprintf(stdout, "%s", yk_dict_id2name(yk_term_csize_names, CS5));
		else if(t->c_cflag & CS6)
			fprintf(stdout, "%s", yk_dict_id2name(yk_term_csize_names, CS6));
		else if(t->c_cflag & CS7)
			fprintf(stdout, "%s", yk_dict_id2name(yk_term_csize_names, CS7));
		else if(t->c_cflag & CS8)
			fprintf(stdout, "%s", yk_dict_id2name(yk_term_csize_names, CS8));
		break;
	case YK_CREAD:
		fprintf(stdout, "%s%s", t->c_cflag & CREAD? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_CSTOPB:
		fprintf(stdout, "%s%s", t->c_cflag & CSTOPB? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_PARENB:
		fprintf(stdout, "%s%s", t->c_cflag & PARENB? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_PARODD:
		fprintf(stdout, "%s%s", t->c_cflag & PARODD? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_HUPCL:
		fprintf(stdout, "%s%s", t->c_cflag & HUPCL? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_CLOCAL:
		fprintf(stdout, "%s%s", t->c_cflag & CLOCAL? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_ISIG:
		fprintf(stdout, "%s%s", t->c_lflag & ISIG? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_ICANON:
		fprintf(stdout, "%s%s", t->c_lflag & ICANON? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_ECHO:
		fprintf(stdout, "%s%s", t->c_lflag & ECHO? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_ECHOE:
		fprintf(stdout, "%s%s", t->c_lflag & ECHOE? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_ECHONL:
		fprintf(stdout, "%s%s", t->c_lflag & ECHONL? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_NOFLSH:
		fprintf(stdout, "%s%s", t->c_lflag & NOFLSH? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_IEXTEN:
		fprintf(stdout, "%s%s", t->c_lflag & IEXTEN? "+" : "-", yk_dict_id2name(yk_term_attr_names, flag));
		break;
	case YK_VEOF:
		fprintf(stdout, "eof=%#x", t->c_cc[VEOF]);
		break;
	case YK_VEOL:
		fprintf(stdout, "eol=%#x", t->c_cc[VEOL]);
		break;
	case YK_VERASE:
		fprintf(stdout, "erase=%#x", t->c_cc[VERASE]);
		break;
	case YK_VINTR:
		fprintf(stdout, "intr=%#x", t->c_cc[VINTR]);
		break;
	case YK_VQUIT:
		fprintf(stdout, "quit=%#x", t->c_cc[VQUIT]);
		break;
	case YK_VKILL:
		fprintf(stdout, "kill=%#x", t->c_cc[VKILL]);
		break;
	case YK_VSTART:
		fprintf(stdout, "start=%#x", t->c_cc[VSTART]);
		break;
	case YK_VSTOP:
		fprintf(stdout, "stop=%#x", t->c_cc[VSTOP]);
		break;
	case YK_VTIME:
		fprintf(stdout, "time=%#x", t->c_cc[VTIME]);
		break;
	case YK_VMIN:
		fprintf(stdout, "min=%#x", t->c_cc[VMIN]);
		break;
	}
}

void yk_term_attr_dump(int fd) {
	struct termios t;
	memset((void*)&t, 0, sizeof(t));
	if(tcgetattr(fd, &t)) {
		yk_fprintf(stderr, "error: yk_term_attr_dump: tcgetattr: %m\n");
		return;
	}
	fprintf(stdout, "ispeed: %s    ospeed:%s\n", yk_dict_id2name(yk_term_speed_names, cfgetispeed(&t)), 
		yk_dict_id2name(yk_term_speed_names, cfgetospeed(&t)));
	_yk_term_attr_print(&t, YK_IGNBRK);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_BRKINT);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_IGNPAR);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_PARMRK);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_INPCK);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_ISTRIP);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_IGNCR);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_ICRNL);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_INLCR);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_IUCLC);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_IXON);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_IXOFF);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_IXANY);
	fprintf(stdout," \n");
	_yk_term_attr_print(&t, YK_OPOST);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_OLCUC);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_OCRNL);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_ONLCR);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_ONOCR);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_ONLRET);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_OFILL);
	fprintf(stdout," \n");
	_yk_term_attr_print(&t, YK_CSIZE);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_CREAD);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_CSTOPB);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_PARENB);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_PARODD);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_HUPCL);
	fprintf(stdout," \n");
	_yk_term_attr_print(&t, YK_ISIG);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_ICANON);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_ECHO);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_ECHOE);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_ECHONL);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_NOFLSH);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_IEXTEN);
	fprintf(stdout," \n");
	_yk_term_attr_print(&t, YK_VEOF);
	fprintf(stdout,"  ");
	_yk_term_attr_print(&t, YK_VEOL);
	fprintf(stdout," ");
	_yk_term_attr_print(&t, YK_VERASE);
	fprintf(stdout,"  ");
	_yk_term_attr_print(&t, YK_VINTR);
	fprintf(stdout,"  ");
	_yk_term_attr_print(&t, YK_VKILL);
	fprintf(stdout,"  ");
	_yk_term_attr_print(&t, YK_VQUIT);
	fprintf(stdout,"  ");
	_yk_term_attr_print(&t, YK_VSTART);
	fprintf(stdout,"  ");
	_yk_term_attr_print(&t, YK_VSTOP);
	fprintf(stdout,"  ");
	_yk_term_attr_print(&t, YK_VMIN);
	fprintf(stdout,"  ");
	_yk_term_attr_print(&t, YK_VTIME);
	fprintf(stdout,"\n");
}
