#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <db.h>

typedef struct student_t {
	int  id;
	char name[20];
}STUDENT;

static int access_flags = 0;
static char *dbfile = NULL;
static char *dbname = NULL;
static char *progname = "access";
static char *access_method = "BTREE";

void usage(int exit_code) {
	fprintf(stdout, "usage: %s [-ch] <dbfile> [dbname]\n", progname);
	fprintf(stdout, "   -c     create database <dbname>\n");
	fprintf(stdout, "   -t     specify the access mothod of the database\n");
	fprintf(stdout, "   -h     dispaly this help message\n");
	exit(exit_code);
}

struct acc_mtd_t {
	char *name;
	int  id;
};

struct acc_mtd_t acc_mtds[] = {
	{"BTREE", DB_BTREE},
	{"btree", DB_BTREE},
	{"HASH", DB_HASH},
	{"hash", DB_HASH},
	{"QUEUE", DB_QUEUE},
	{"queue", DB_QUEUE},
	{"RECNO", DB_RECNO},
	{"recno", DB_RECNO},
	{NULL, 0}
};

int acc_mtd_name2id(char *name) {
	struct acc_mtd_t *p = acc_mtds;
	for(; p->name; p++) {
		if(strcmp(name, p->name) == 0)
			return p->id;
	}
	return 0;
}

void init(int argc, char *argv[]) {
	int opt;
	while((opt=getopt(argc, argv, "cht:")) != -1) {
		switch(opt) {
		case 'c':
			access_flags |= DB_CREATE|DB_EXCL;
			break;
		case 'h':
			usage(0);
		case 't':
			access_method = optarg;
			break;
		default:
			usage(1);
		}
	}
	if(argv[optind] == NULL)
		usage(1);
	dbfile = argv[optind];
	if(argv[optind+1])
		dbname = argv[optind+1];
}

int access_show_rec(DB *dbp, db_recno_t n) {
	DBC *cur;
	DBT k,v;
	char buf[1024];
	int ret = 1;
	if(ret = dbp->cursor(dbp, NULL, &cur, 0)) {
		dbp->err(dbp, ret, "access_show_rec: dbp->cursor");
		goto err0;
	}
	memset((void*)&k, 0, sizeof(DBT));
	memset((void*)&v, 0, sizeof(DBT));
	k.data  = &n;
	k.size  = sizeof(n);
	if(ret = cur->get(cur, &k, &v, DB_SET_RECNO)) {
		if(ret != DB_NOTFOUND)
			dbp->err(dbp, ret, "access_show_rec: cur->get");
		goto err1;
	}
	printf("%.*s : %.*s\n", k.size, (char*)k.data, v.size, (char*)v.data);
	ret = 0;
err1:
	cur->close(cur);
err0:
	return ret;
}

int access_dump_recs(DB *dbp) {
	DBC *cur;
	DBT k, v;
	int ret = 1, count = 1;
	if(ret = dbp->cursor(dbp, NULL, &cur, 0)) {
		dbp->err(dbp, ret, "access_dump_recs: dbp->cursor");
		goto err0;
	}
	memset((void *)&k, 0, sizeof(DBT));
	memset((void *)&v, 0, sizeof(DBT));
	while((ret = cur->get(cur, &k, &v, DB_NEXT)) == 0) {
		fprintf(stdout, "%03d    %.*s : %.*s\n", count++, k.size, (char*)k.data, v.size, (char*)v.data);
	}
	if(ret != DB_NOTFOUND)
		dbp->err(dbp, ret, "access_dump_recs: cur->get");
	ret = 0;
err0:
	return ret;
}

int access_input_recs(DB *dbp) {
	char buf[1024], revbuf[1024];
	int ret = 0;
	int len = 0;
	char *t, *p;
	DBT k, v;
	while(1) {
		fprintf(stdout, "input> ");
		fflush(stdout);
		memset(buf, 0, sizeof(buf));
		memset(revbuf, 0, sizeof(revbuf));
		if(fgets(buf, sizeof(buf), stdin) == NULL)
			break;
		len = strlen(buf);
		if(len > 0 && buf[len-1] == '\n') {
			buf[len-1] = '\0';
			len--;
		}
		if(len == 0)
			continue;
		if(strcasecmp(buf, "exit") == 0 ||
		   strcasecmp(buf, "quit") == 0 ||
		   strcasecmp(buf, "save") == 0)
			break;
		for(t = revbuf, p = buf + len; p >= buf;)
			*t++ = *--p;
		*t = '\0';	
		memset((void*)&k, 0, sizeof(DBT));
		memset((void*)&v, 0, sizeof(DBT));
		k.data = buf;
		v.data = revbuf;
		k.size = v.size = len;
		if(ret = dbp->put(dbp, NULL, &k, &v, DB_NOOVERWRITE))
			dbp->err(dbp, ret, "access_input_recs: dbp->put");
		
	}
	return ret;
}


int db_access(void) {
	DB *dbp;
	int ret = 1;
	if(ret = db_create(&dbp, NULL, 0)) {
		fprintf(stderr, "error: db_create: %s\n", db_strerror(ret));
		goto err0;
	}
	
	dbp->set_errpfx(dbp, "error");
	dbp->set_errfile(dbp, stderr);
	
	if(ret = dbp->set_flags(dbp, DB_RECNUM)) {
		dbp->err(dbp, ret, "dbp->set_flag[DB_RECNUM]");
		goto err1;
	}
	
	if(ret = dbp->open(dbp, NULL, dbfile, dbname, 
				acc_mtd_name2id(access_method), access_flags, 0)) {
		dbp->err(dbp, ret, "dbp->open[%s%s%s]", dbfile, dbname?"/":"", dbname?dbname:"");
		goto err1;
	}
	DBT k, v;
	char buf[1024];
	char revbuf[1024];
	char *t, *p;
	int len;
	while(1) {
		
		fprintf(stdout, "access> ");
		fflush(stdout);
		memset(buf, 0, sizeof(buf));
		if(fgets(buf, sizeof(buf), stdin) == NULL)
			break;
		len = strlen(buf);
		if(len >= 1 && buf[len-1] == '\n') {
			buf[len-1] = '\0';
			len--;
		}

		if(strcasecmp(buf, "exit") == 0 || strcasecmp(buf, "quit") == 0)
			break;
		if(len == 0)
			continue;
		if(strcasecmp(buf, "input") == 0) {
			access_input_recs(dbp);
		}
		else if(strcasecmp(buf, "list") == 0) {
			access_dump_recs(dbp);
		}
		else if(strcasecmp(buf, "del") == 0) {
			fprintf(stderr, "not supported\n");
		}
		else if(strcasecmp(buf, "remove") == 0) {
			remove(dbfile);
			goto success;
		}
		else if(strcasecmp(buf, "stat") == 0) {
			if(ret = dbp->stat_print(dbp, 0)) {
				dbp->err(dbp, ret, "dbp->stat_print");
			}
		}
		else if((ret = atoi(buf)) > 0) {
			access_show_rec(dbp, ret);
		}
		else if(strcasecmp(buf, "help") == 0) {
			fprintf(stdout, "the following command are supported:\n");
			fprintf(stdout, "  input       switch into input mode, to input data int db\n");
			fprintf(stdout, "  remove      remove the db and exit\n");
			fprintf(stdout, "  list        dump the db\n");
			fprintf(stdout, "  stat        display statistic informations of the db\n");
			fprintf(stdout, "  help        display this help message\n");
			fprintf(stdout, "  exit/quit   exit this application\n");
		}
	}
success:
	ret = 0;
err1:
	if(dbp)
		dbp->close(dbp, 0);
err0:
	return ret;
}

int main(int argc, char *argv[]) {
	
	init(argc, argv);

	return db_access();
}

