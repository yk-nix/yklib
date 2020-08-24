#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#ifdef _YK_SHM_DEBUG
#include <errno.h>
#define yk_printf        printf
#else 
#define yk_printf(x,...)    
#endif 


int yk_shmget(char *file, char projno, size_t size, int flags) {
	key_t key = ftok(file, projno);
	if(key == -1) {
		yk_printf("yk_shm_get: create key = %#x\n", key);
		return -1;
	}
	return shmget(key, size, flags);
}

char *yk_shmat(int shmid, int flags) {
	return (char*)shmat(shmid, NULL, flags);
}

int yk_shmdt(const char *addr) {
	return shmdt(addr);
}


static void _yk_shm_print_stat(int shmid, struct shmid_ds *s) {
	printf("key : %#x  shmid:%d\n"
	       "size: %d   mode: %o\n"
	       "cuid: %d   cgid: %d\n"
	       "uid : %d    gid: %d\n"
	       "nattch: %d\n"
	       "cpid : %d   lpid: %d\n"
	       "atime: %ld\n"
	       "dtime: %ld\n"
	       "ctime: %ld\n", 
		s->shm_perm.__key, shmid,
		s->shm_segsz,  s->shm_perm.mode,
		s->shm_perm.cuid, s->shm_perm.cgid,
		s->shm_perm.uid, s->shm_perm.gid,
		s->shm_nattch, 
		s->shm_cpid, s->shm_lpid,
		s->shm_atime,
		s->shm_dtime,
		s->shm_ctime);
	
}
void yk_shm_print_stat(int shmid) {
	struct shmid_ds s;
	memset((void *)&s, 0, sizeof(s));
	if(shmctl(shmid, IPC_STAT, &s)) {
		yk_printf("yk_shm_print_stat: shmctl[IPC_STAT] error: %s\n", strerror(errno));
		return;
	}
	_yk_shm_print_stat(shmid, &s);
}

int yk_shm_set_mode(int shmid, int mode) {
	struct shmid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(shmctl(shmid, IPC_STAT, &s)) {
		yk_printf("yk_shm_set_mode: shmctl[IPC_STAT] error: %s\n", strerror(errno));
		return -1;
	}
	s.shm_perm.mode = mode;
	if(shmctl(shmid, IPC_SET, &s)) {
		yk_printf("yk_shm_set_mode: shmctl[IPC_SET] error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
int yk_shm_get_mode(int shmid) {
	struct shmid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(shmctl(shmid, IPC_STAT, &s)) {
		yk_printf("yk_shm_get_mode: shmctl[IPC_STAT] error: %s\n", strerror(errno));
		return 0;
	}
	return s.shm_perm.mode;
}

int yk_shm_set_uid(int shmid, int uid) {
	struct shmid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(shmctl(shmid, IPC_STAT, &s)) {
		yk_printf("yk_shm_set_uid: shmctl[IPC_STAT] error: %s\n", strerror(errno));
		return -1;
	}
	s.shm_perm.uid = uid;
	if(shmctl(shmid, IPC_SET, &s)) {
		yk_printf("yk_shm_set_uid: shmctl[IPC_SET] error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
int yk_shm_get_uid(int shmid) {
	struct shmid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(shmctl(shmid, IPC_STAT, &s)) {
		yk_printf("yk_shm_get_uid: shmctl[IPC_STAT] error: %s\n", strerror(errno));
		return 0;
	}
	return s.shm_perm.uid;
}

int yk_shm_set_gid(int shmid, int gid) {
	struct shmid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(shmctl(shmid, IPC_STAT, &s)) {
		yk_printf("yk_shm_set_gid: shmctl[IPC_STAT] error: %s\n", strerror(errno));
		return -1;
	}
	s.shm_perm.gid = gid;
	if(shmctl(shmid, IPC_SET, &s)) {
		yk_printf("yk_shm_set_gid: shmctl[IPC_SET] error: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
int yk_shm_get_gid(int shmid) {
	struct shmid_ds s;
	memset((void*)&s, 0, sizeof(s));
	if(shmctl(shmid, IPC_STAT, &s)) {
		yk_printf("yk_shm_get_uid: shmctl[IPC_STAT] error: %s\n", strerror(errno));
		return 0;
	}
	return s.shm_perm.gid;
}

int yk_shm_remove(int shmid) {
	return shmctl(shmid, IPC_RMID, NULL);
}

#ifdef _YK_SHM_TEST
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
static char *progname = "shm";
static char *_mode = NULL;
static char *_uid  = NULL;
static char *_gid  = NULL;
static char *_size = NULL;
static char *_keyfile = NULL;
static char *_datafile = NULL;
static int  _shmid = 0;
static int _iskey = 0;
static int _key = 0;
static void usage(int ecode) {
	fprintf(stdout, "usage: %s [-m mode] [-u uid] [-g gid] [-n size] [-kdph] [-w [file]] keyfile|key\n"
			"   -m mode            set the mode of the shared memeory\n"
			"   -u uid             set the owner uid of the shared memory\n"
			"   -g gid             set the owner gid of the shared memory\n"
			"   -n size            create a new shared memory of length of 'size'\n"
			"   -d                 delete the shared memory\n"
			"   -r                 read the data in the shared memory and print to stdout\n"
			"   -w [file]          write the data reading from file into the shared memory,\n"
			"                      read data from stdin if no file specified.\n"
			"   -h                 display this help message'\n"
			"   -p                 print the infomations of the shared memory\n", progname);
	exit(ecode);
}
static int shm_set_mode(int shmid, char *arg) {
	char *err = NULL;
	int mode = strtol(arg, &err, 0);
	if(mode == 0 && err == arg) {
		fprintf(stderr, "bad mode value\n");
		return -1;
	}
	return yk_shm_set_mode(shmid, mode);
}
static int shm_set_uid(int shmid, char *arg) {
	char *err = NULL;
	int uid = strtol(arg, &err, 0);
	if(uid == 0 && err == arg) {
		fprintf(stderr, "bad uid value\n");
		return -1;
	}
	return yk_shm_set_uid(shmid, uid);
}
static int shm_set_gid(int shmid, char *arg) {
	char *err = NULL;
	int gid = strtol(arg, &err, 0);
	if(gid == 0 && err == arg) {
		fprintf(stderr, "bad gid value\n");
		return -1;
	}
	return yk_shm_set_gid(shmid, gid);
}
static void shm_print_stat(int shmid) {
	yk_shm_print_stat(shmid);
}
static int shm_remove(int shmid) {
	return yk_shm_remove(shmid);
}
static int shm_create(char *keyfile, char *arg) {
	char *err = NULL;
	int size = strtol(arg, &err, 0);
	if(size == 0 && err == arg) {
		fprintf(stderr, "bad size of the shared memory\n");
		return -1;
	}
	int shmid = yk_shmget(keyfile, PROJ_NO, size, IPC_CREAT|IPC_EXCL|SHM_R|SHM_W);
	if(shmid == -1) {
		fprintf(stderr, "shmget error: %s\n", strerror(errno));
		return -1;
	}
	yk_shm_print_stat(shmid);
	return 0;
}

static int shm_read(int shmid) {
	char *cache = yk_shmat(shmid, SHM_RDONLY);
	if(cache == (void *)-1) {
		fprintf(stderr, "shmat error: %s\n", strerror(errno));
		return -1;
	}

	int ret = fprintf(stdout, "%s\n", cache);
	if(yk_shmdt(cache)) {
		fprintf(stderr, "shmdt error: %s\n", strerror(errno));
		return -1;
	}
	return ret;
}
static int shm_write(int shmid, char *file) {
	int ret = 0;
	char *cache = yk_shmat(shmid, 0);
	if(cache == (void *)-1) {
		fprintf(stderr, "shmat error: %s\n", strerror(errno));
		return 1;
	}
	if(file == NULL) {
		ret = fscanf(stdin, "%s", cache);
		goto ex;
	}
	FILE *f = fopen(file, "r");
	char buf[1024]; 
	memset(buf, 0, sizeof(buf));
	while(fgets(buf, sizeof(buf), f)) {
		ret += sscanf(buf, "%s", cache + ret);	
	}
ex:
	if(yk_shmdt(cache)) {
		fprintf(stderr, "shmdt error: %s\n", strerror(errno));
		return -1;
	}
	return ret;
}
static int init(int argc, char *argv[]) {
	int cmd = CMD_PRINT;
	char opt;
	while((opt = getopt(argc, argv, "m:u:g:n:dpkhrw::")) != -1) {
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
			char *err = NULL;
			int size = strtol(_size, &err, 0);
			if(size == 0 && err == _size) {
				fprintf(stderr, "bad size value of the shared memory.\n");
				return 1;
			}
			return shmget(_key, size, IPC_CREAT|IPC_EXCL|SHM_R|SHM_W);
		}
		else
			return shm_create(_keyfile, _size);	
	}
	if(_iskey) 
		_shmid = shmget(_key, 0, 0);
	else
		_shmid = yk_shmget(_keyfile, PROJ_NO, 0, 0);
	if(_shmid == -1) {
		fprintf(stderr, "shmget error: %s\n", strerror(errno));
		return 1;
	}
	switch(cmd) {
	case CMD_PRINT:
		shm_print_stat(_shmid);
		return 0;
	case CMD_SET_MODE:
		return shm_set_mode(_shmid, _mode);
	case CMD_SET_UID:
		return shm_set_uid(_shmid, _uid);
	case CMD_SET_GID:
		return shm_set_gid(_shmid, _gid);
	case CMD_REMOVE:
		return shm_remove(_shmid);
	case CMD_READ:
		return shm_read(_shmid);
	case CMD_WRITE:
		return shm_write(_shmid, _datafile);
	defaut:
		return 1;
	}
	return 0;
}
#endif /* _YK_SHM_TEST */
