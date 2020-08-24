#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _YK_MSG_DEBUG
#include <errno.h>
#define yk_printf        printf
#else   /* _YK_MSG_DEBUG */ 
#define yk_printf(x,...)    
#endif  /* _YK_MSG_DEBUG */

struct msgbuf {
	long mtype;
	char mtext[1];
};

int yk_msgget(char *file, char projno, int flags) {
	key_t key = ftok(file, projno);
	if(key == -1) {
		yk_printf("yk_msgget: create key = %#x\n", key);
		return -1;
	}
	return msgget(key, flags);
}

int yk_msgsnd(int msgid, long mtype, const char *msg, size_t size, int flags) {
	int len = size + sizeof(long) + 1;
	char *cache = (char *)malloc(len);
	if(cache == NULL) {
		yk_printf("yk_msgsnd: malloc error: %s\n", strerror(errno));
		return -1;
	}
	memset(cache, 0, len);
	struct msgbuf *p = (struct msgbuf *)cache;
	p->mtype = mtype;
	memcpy(p->mtext, msg, size);
	int ret = msgsnd(msgid, cache, size, flags);
	free(cache);
	if(ret < 0) {
		yk_printf("yk_msgsnd: msgsnd error: %s\n", strerror(errno));
	}
	return ret;
}

int yk_msgrcv(int msgid, long mtype, char *cache, size_t size, int flags) {
	return msgrcv(msgid, cache, size, mtype, flags);
}


static void _yk_msg_print_stat(int msgid, struct msqid_ds *s) {
	printf("key : %#x  msgid:%d\n"
	       "size: %d   mode: %o\n"
	       "cuid: %d   cgid: %d\n"
	       "uid : %d    gid: %d\n"
	       "qnum: %d   cbytes: %d\n"
	       "lspid : %d   lrpid: %d\n"
	       "atime: %ld\n"
	       "dtime: %ld\n"
	       "ctime: %ld\n", 
		s->msg_perm.__key, msgid,
		s->msg_qbytes,  s->msg_perm.mode,
		s->msg_perm.cuid, s->msg_perm.cgid,
		s->msg_perm.uid, s->msg_perm.gid,
		s->msg_qnum, s->__msg_cbytes,
		s->msg_lspid, s->msg_lrpid,
		s->msg_stime,
		s->msg_rtime,
		s->msg_ctime);
	
}
void yk_msg_print_stat(int msgid) {
	struct msqid_ds s;
	memset((void *)&s, 0, sizeof(s));
	if(msgctl(msgid, IPC_STAT, &s)) {
		yk_printf("yk_msg_print_stat: msgctl[IPC_STAT] error: %s\n", strerror(errno));
		return;
	}
	_yk_msg_print_stat(msgid, &s);
}

int yk_msg_set_mode(int msgid, int mode) {
	struct msqid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(msgctl(msgid, IPC_STAT, &s)) {
		yk_printf("yk_msg_set_mode: msgctl[IPC_STAT] error: %s\n", strerror(errno));
		return -1;
	}
	s.msg_perm.mode = mode;
	if(msgctl(msgid, IPC_SET, &s)) {
		yk_printf("yk_msg_set_mode: msgctl[IPC_SET] error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
int yk_msg_get_mode(int msgid) {
	struct msqid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(msgctl(msgid, IPC_STAT, &s)) {
		yk_printf("yk_msg_get_mode: msgctl[IPC_STAT] error: %s\n", strerror(errno));
		return 0;
	}
	return s.msg_perm.mode;
}

int yk_msg_set_uid(int msgid, int uid) {
	struct msqid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(msgctl(msgid, IPC_STAT, &s)) {
		yk_printf("yk_msg_set_uid: msgctl[IPC_STAT] error: %s\n", strerror(errno));
		return -1;
	}
	s.msg_perm.uid = uid;
	if(msgctl(msgid, IPC_SET, &s)) {
		yk_printf("yk_msg_set_uid: msgctl[IPC_SET] error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
int yk_msg_get_uid(int msgid) {
	struct msqid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(msgctl(msgid, IPC_STAT, &s)) {
		yk_printf("yk_msg_get_uid: msgctl[IPC_STAT] error: %s\n", strerror(errno));
		return 0;
	}
	return s.msg_perm.uid;
}

int yk_msg_set_gid(int msgid, int gid) {
	struct msqid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(msgctl(msgid, IPC_STAT, &s)) {
		yk_printf("yk_msg_set_gid: msgctl[IPC_STAT] error: %s\n", strerror(errno));
		return -1;
	}
	s.msg_perm.gid = gid;
	if(msgctl(msgid, IPC_SET, &s)) {
		yk_printf("yk_msg_set_gid: msgctl[IPC_SET] error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
int yk_msg_get_gid(int msgid) {
	struct msqid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(msgctl(msgid, IPC_STAT, &s)) {
		yk_printf("yk_msg_get_uid: msgctl[IPC_STAT] error: %s\n", strerror(errno));
		return 0;
	}
	return s.msg_perm.gid;
}

int yk_msg_remove(int msgid) {
	return msgctl(msgid, IPC_RMID, NULL);
}

#ifdef _YK_MSG_TEST
#include <getopt.h>
#include <stdlib.h>
#define CMD_SET_MODE         1
#define CMD_SET_GID          2
#define CMD_SET_UID          3
#define CMD_PRINT            4
#define CMD_CREATE           5
#define CMD_REMOVE           6
#define CMD_READ             7
#define CMD_WRITE            8
#define CMD_ERR              -1
#define PROJ_NO             'A'
static char *progname = "msg";
static char *_mode = NULL;
static char *_uid  = NULL;
static char *_gid  = NULL;
static char *_size = NULL;
static char *_keyfile = NULL;
static char *_datafile = NULL;
static int  _msgid = 0;
static long _msgtype = 1;
static int _iskey = 0;
static int _key = 0;
static void usage(int ecode) {
	fprintf(stdout, "usage: %s [-m mode] [-u uid] [-g gid] [-n size] [-kdph] [-w [file]] keyfile|key\n"
			"   -m mode            set the mode of the message queue\n"
			"   -u uid             set the owner uid of the message queue\n"
			"   -g gid             set the owner gid of the message queue\n"
			"   -n size            set the size of the message queue to be 'size'\n"
			"   -d                 delete the message queue\n"
			"   -r                 read the data in the message queue and print to stdout\n"
			"   -w [file]          write the data reading from file into the message queue,\n"
			"                      read data from stdin if no file specified.\n"
			"   -h                 display this help message'\n"
			"   -p                 print the infomations of the shared memory\n", progname);
	exit(ecode);
}
static int msg_set_mode(int msgid, char *arg) {
	char *err = NULL;
	int mode = strtol(arg, &err, 0);
	if(mode == 0 && err == arg) {
		fprintf(stderr, "bad mode value\n");
		return -1;
	}
	return yk_msg_set_mode(msgid, mode);
}
static int msg_set_uid(int msgid, char *arg) {
	char *err = NULL;
	int uid = strtol(arg, &err, 0);
	if(uid == 0 && err == arg) {
		fprintf(stderr, "bad uid value\n");
		return -1;
	}
	return yk_msg_set_uid(msgid, uid);
}
static int msg_set_gid(int msgid, char *arg) {
	char *err = NULL;
	int gid = strtol(arg, &err, 0);
	if(gid == 0 && err == arg) {
		fprintf(stderr, "bad gid value\n");
		return -1;
	}
	return yk_msg_set_gid(msgid, gid);
}
static void msg_print_stat(int msgid) {
	yk_msg_print_stat(msgid);
}
static int msg_remove(int msgid) {
	return yk_msg_remove(msgid);
}
static int msg_create(char *keyfile) {
	char *err = NULL;
	int msgid = yk_msgget(keyfile, PROJ_NO, IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
	if(msgid == -1) {
		fprintf(stderr, "msgget error: %s\n", strerror(errno));
		return -1;
	}
	yk_msg_print_stat(msgid);
	return 0;
}

static int msg_read(int msgid, long mtype) {
	int bytes = 0, count = 0;;
	char cache[1024];
	memset(cache, 0, sizeof(cache));
	struct msgbuf *p;
	while((bytes = yk_msgrcv(msgid, mtype, cache, sizeof(cache), 0)) > 0) {
		p = (struct msgbuf *)cache;
		fprintf(stdout, "%.*s", bytes, p->mtext);
		fflush(stdout);
		memset(cache, 0, sizeof(cache));
		count += bytes;
	}
	return count;
}
static int msg_write(int msgid, char *file, long mtype) {
	int ret = 0, bytes = 0;
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	if(file == NULL) {
		fscanf(stdin, "%s", buf);
		bytes = strlen(buf);
		return yk_msgsnd(msgid, mtype, buf, bytes, 0);
	}
}
static int init(int argc, char *argv[]) {
	int cmd = CMD_PRINT;
	char opt;
	while((opt = getopt(argc, argv, "m:u:g:ndpkhrw::")) != -1) {
		switch(opt) {
		case 'm':
			cmd = CMD_SET_MODE;
			_mode = optarg;
			break;
		case 'u':
			cmd = CMD_SET_UID;
			_uid = optarg;
			break;
		case 'g':
			cmd = CMD_SET_GID;
			_gid = optarg;
			break;
		case 'p':
			cmd = CMD_PRINT;
			break;
		case 'n':
			cmd = CMD_CREATE;
			_size = optarg;
			break;
		case 'd':
			cmd = CMD_REMOVE;
			break;
		case 'k':
			_iskey = 1;
			break;
		case 'r':
			cmd = CMD_READ;
			break;
		case 'w':
			cmd = CMD_WRITE;
			_datafile = optarg;
			break;
		case 'h':
			usage(0);
		default: 
			usage(1);
		}
	}
	if(argv[optind] == NULL)
		usage(1);
	if(_iskey) {
		char *err;
		_key = strtol(argv[optind], &err, 0);
		if(_key == 0 && err == argv[optind])
			usage(1);
	}
	else
		_keyfile = argv[optind];
	return cmd;
}

int main(int argc, char *argv[]) {
	int cmd = init(argc, argv);
	if(cmd == CMD_CREATE) {
		if(_iskey) {
			return msgget(_key, IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
		}
		else
			return msg_create(_keyfile);	
	}
	if(_iskey) 
		_msgid = shmget(_key, 0);
	else
		_msgid = yk_msgget(_keyfile, PROJ_NO, 0);
	if(_msgid == -1) {
		fprintf(stderr, "msgget error: %s\n", strerror(errno));
		return 1;
	}
	switch(cmd) {
	case CMD_PRINT:
		msg_print_stat(_msgid);
		return 0;
	case CMD_SET_MODE:
		return msg_set_mode(_msgid, _mode);
	case CMD_SET_UID:
		return msg_set_uid(_msgid, _uid);
	case CMD_SET_GID:
		return msg_set_gid(_msgid, _gid);
	case CMD_REMOVE:
		return msg_remove(_msgid);
	case CMD_READ:
		return msg_read(_msgid, _msgtype);
	case CMD_WRITE:
		return msg_write(_msgid, _datafile, _msgtype);
	defaut:
		return 1;
	}
	return 0;
}
#endif /* _YK_MSG_TEST */
