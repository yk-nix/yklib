#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <db.h>

typedef struct yk_db_ctx_t {
	DB_ENV	*dbenv;
	DB	*dbp;
}YK_DB_CTX;

typedef struct student_t {
	u_int32_t      id;
	u_int8_t       age;
	u_int8_t       height;
	unsigned char  name[];
} STUDENT;

static void stu_dump(void *data) {
	STUDENT *stu = (STUDENT *)data;
	fprintf(stdout, "id: %d   name: %s  age: %d  height: %dcm\n", 
		stu->id, stu->name, stu->age, stu->height);
}
static int stu_create(STUDENT **stu, u_int32_t id, u_int8_t age, 
		      u_int8_t height, const unsigned char *name) {
	int len  = sizeof(STUDENT) + strlen(name) + 1;
	*stu = calloc(1, len);
	if(stu == NULL)
		return -1;
	(*stu)->id      = id;
	(*stu)->age     = age;
	(*stu)->height  = height;
	sprintf((*stu)->name, "%s", name);
	return len;
}
static int stu_free(STUDENT *stu) {
	if(stu)
		free(stu);
}
static int stu_insert(YK_DB_CTX *ctx, int argc, void *argv[]) {
	int ret = 1;
	u_int32_t id     = *((u_int32_t*)argv[0]);
	u_int8_t  age    = *((u_int8_t *)argv[1]);
	u_int8_t  height = *((u_int8_t *)argv[2]);
	char     *name   = *((char**)argv[3]);
	STUDENT  *stu    = NULL;
	DB       *dbp    = ctx->dbp;
	int size = stu_create(&stu, id, age, height, name);
	if(size <= 0) 
		return -1;
	DBT k, v;
	memset((void *)&k, 0, sizeof(DBT));
	memset((void *)&v, 0, sizeof(DBT));
	k.data = &stu->id;
	k.size = sizeof(stu->id);
	v.data = (void *)stu;
	v.size = size;
	ret = dbp->put(dbp, NULL, &k, &v, DB_NOOVERWRITE);
	stu_free(stu);
	return ret;
}

static int stu_delete(YK_DB_CTX *ctx, void *key) {
	
	u_int32_t id = *((u_int32_t *)key);
	DB *dbp = ctx->dbp;
	DBT k;
	memset((void *)&k, 0, sizeof(DBT));
	k.data = &id;
	k.size = sizeof(id);
	return dbp->del(dbp, NULL, &k, 0);
}









int yk_db_dump(YK_DB_CTX *ctx, void (*dump_func)(void *data)) {
	int ret = 1;
	DBC *cur;
	DBT k, v;
	u_int32_t id = 0;
	DB  *dbp = ctx->dbp;
	if(ret = dbp->cursor(dbp, NULL, &cur, 0)) {
		dbp->err(dbp, ret, "error get db cursor");
		return ret;
	}
	memset((void*)&k, 0, sizeof(DBT));
	memset((void*)&v, 0, sizeof(DBT));
	//v.flags = DB_DBT_MALLOC;
	int count = 0;
	while((ret = cur->get(cur, &k, &v, DB_NEXT)) == 0) {
		dump_func(v.data);
		//free(v.data);
	}
	cur->close(cur);
	ret = 0;
	return ret;
}


YK_DB_CTX *yk_db_open(char *dir, char *filename) {
	int ret;
	u_int32_t flags;
	YK_DB_CTX *ctx = calloc(1, sizeof(YK_DB_CTX));
	if(ctx == NULL)
		goto err0;
	
	DB_ENV *dbenv;
	if(ret = db_env_create(&dbenv, 0)) {
		fprintf(stderr, "error creating db environment handle: %s\n", db_strerror(ret));
		goto err1;
	}
	flags = DB_CREATE|DB_THREAD|DB_INIT_MPOOL|DB_INIT_LOCK|DB_INIT_LOG;
	if(ret = dbenv->open(dbenv, dir, flags, 0)) {
		dbenv->err(dbenv, ret, "error opening the db environment %S", dir);
		goto err1;
	}
	
	DB *dbp;
	if(ret = db_create(&dbp, dbenv, 0)) {
		dbenv->err(dbenv, ret, "error creating db handle");
		goto err1;
	}
	flags = DB_CREATE|DB_THREAD;
	if(dbp->open(dbp, NULL, filename, NULL, DB_BTREE, flags, 0)) {
		dbenv->err(dbenv, ret, "error open database %s", filename);
		goto err3;
	}
	ctx->dbenv = dbenv;
	ctx->dbp = dbp;
	return ctx;
err3:
	if(dbp)
		dbp->close(dbp, 0);
err2:
	if(dbenv)
		dbenv->close(dbenv, 0);
err1:
	if(ctx)
		free(ctx);
err0:
	return NULL;
}

void yk_db_close(YK_DB_CTX *ctx) {
	DB_ENV	*dbenv;
	DB	*dbp;
	if(ctx) {
		dbenv = ctx->dbenv;
		dbp   = ctx->dbp;
		if(dbp)
			dbp->close(dbp, 0);
		if(dbenv)
			dbenv->close(dbenv, 0);
		free(ctx);
	}
}

int yk_db_insert(YK_DB_CTX *ctx,
		 int argc,
		 void **argv,
		 int (*insert_func)(YK_DB_CTX *ctx, int argc, void *argv[])) {
	DB_ENV *dbenv = ctx->dbenv;
	int ret = insert_func(ctx, argc, argv);
	if(ret) {
		dbenv->err(dbenv, ret, "error inserting data into database");
	}
	return ret;
}

int yk_db_delete(YK_DB_CTX *ctx, void *k, 
                 int (*delete_func)(YK_DB_CTX *ctx, void *key)) {
	DB_ENV *dbenv = ctx->dbenv;
	int ret = delete_func(ctx, k);
	if(ret) {
		dbenv->err(dbenv, ret, "error deleting data into database");
	}
	return ret;
}

/*---------------------   test ------------------*/
static char *progname = NULL;
static void usage(int exit_code) {
	fprintf(stdout, "usage: %s <cmd> [cmd-arguments-or-options]\n", progname);
	fprintf(stdout, "The following cmd are supported:\n");
	fprintf(stdout, "  insert       try to insert a record into the database\n");
	fprintf(stdout, "  del          try to delete a record from the database\n");
	fprintf(stdout, "  list         dump all the records from the database\n");
	exit(exit_code);
}

static void usage_insert(int exit_code) {
	fprintf(stdout, "usage:  %s insert [-h] [-N name] [-A age] [-H height] student_id\n", progname);
	fprintf(stdout, "Insert a student infomation into the database. Make suer he student-id\n");
	fprintf(stdout, "is unique, or the insert oepration would fail to be carried out.\n");
	fprintf(stdout, "   -N name        specify the name of student to be inserted into\n"); 
	fprintf(stdout, "   -H height      specify the height of student to be inserted into\n"); 
	fprintf(stdout, "   -A age         specify the age of student to be inserted into\n"); 
	exit(exit_code);
}

static int insert(int argc, char *argv[]) {
	int ret = 1;
	int opt;
	struct option opts[] = {
		{"age",     required_argument, NULL, 'A'},
		{"name",    required_argument, NULL, 'N'},
		{"height",  required_argument, NULL, 'H'},
		{"help",    no_argument,       NULL, 'h'}
	};
	int idx = 0;
	u_int32_t id     = 0;
	u_int8_t  age    = 0;
	u_int8_t  height = 0;
	char     *name   = NULL;
	while((opt = getopt_long(argc, argv, "hN:A:H:", opts, &idx)) != -1) {
		switch(opt) {
		case 'N':
			name= optarg;
			break;
		case 'A':
			age = (u_int8_t)(atoi(optarg));
			break;
		case 'H':
			height = (u_int8_t)(atoi(optarg));
			break;
		case 'h':
			usage_insert(0);
		default:
			usage_insert(1);
		}
	}
	if(argv[optind] == NULL)
		usage_insert(1);
	id = (u_int32_t)atoi(argv[optind]);
	void *fields[5] = { &id, &age, &height, &name, NULL};
	
	YK_DB_CTX *ctx = yk_db_open("data", "students");
	if(ctx) {
		ret = yk_db_insert(ctx, 5, fields, stu_insert);
		yk_db_close(ctx);
	}
	return ret;
 }

static int list(int arg, char *argv[]) {
	int ret = 1;
	YK_DB_CTX *ctx = yk_db_open("data", "students");
	if(ctx) {
		ret = yk_db_dump(ctx, stu_dump);
		yk_db_close(ctx);
	}
	return ret;
}

static int delete(int argc, char *argv[]) {
	int ret = 1;
	if(argc < 2)
		return 1;
	u_int32_t id = (u_int32_t)(atoi(argv[1]));
	YK_DB_CTX *ctx = yk_db_open("data", "students");
	if(ctx) {
		ret = yk_db_delete(ctx, &id, stu_delete);
		yk_db_close(ctx);
	}
	return ret;
}

static int main_routine(int argc, char *argv[]) {
	if(argc < 2)
		usage(1);
	char *cmd = argv[1];
	if(strcasecmp(cmd, "insert") == 0)
		return insert(argc-1, &argv[1]);
	else if(strcasecmp(cmd, "list") == 0)
		return list(argc-1, &argv[1]);
	else if(strcasecmp(cmd, "del") == 0)
		return delete(argc-1, &argv[1]);
	else {
		int opt;
		struct option opts[] = {
			{ "help", no_argument, NULL, 'h'}
		};
		int idx = 0;
		while((opt = getopt_long(argc, argv, "h", opts, &idx)) != -1) {
			switch(opt) {
			case 'h':
				usage(0);
				break;
			default:
				break;
			}
		}
	}
}
static void init(int argc, char *argv[]) {
	progname = strrchr(argv[0], '/');
	if(progname)
		progname++;
	else
		progname = argv[0];
}

int main(int argc, char *argv[]) {
	init(argc, argv);
	return main_routine(argc, argv);

}
