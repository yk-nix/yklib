#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <db.h>

static char *tunable_home = "data";
static char *tunable_datafile = "mp.dat";
static int  tunable_pagesize = 512;

extern void yk_hexdump(char *, int);

int main(int argc, char *argv[]) {
	DB_ENV       *dbenv;
	DB_MPOOLFILE *mpool;
	int          ret;
	u_int32_t    flags;
	int          pagesize = 512;

	if(ret = db_env_create(&dbenv, 0)) {
		fprintf(stderr, "error: db_env_create: %s\n", db_strerror(ret));
		goto err0;
	}
	if(ret = dbenv->set_mp_pagesize(dbenv, pagesize)) {
		dbenv->err(dbenv, ret, "dbenv->set_mp_pagesize");
		goto err1;
	}
	dbenv->set_errfile(dbenv, stderr);
	dbenv->set_errpfx(dbenv, "error");
	
	flags = DB_INIT_MPOOL;
	if(ret = dbenv->open(dbenv, NULL, flags, 0)) {
		dbenv->err(dbenv, ret, "dbenv->open");
		goto err1;
	}
	if(ret = dbenv->memp_fcreate(dbenv, &mpool, 0)) {
		dbenv->err(dbenv, ret, "dbenv->memp_fcreate");
		goto err1;
	}
	if(ret = mpool->open(mpool, "mp.dat", DB_DIRECT, 0, pagesize)) {
		dbenv->err(dbenv, ret, "mpool->open");
		goto err2;
	}
	if(ret = dbenv->memp_sync(dbenv, NULL)) {
		dbenv->err(dbenv, ret, "dbenv->mem_sync");
		goto err2;
	}
err2:
	if(mpool)
		mpool->close(mpool, 0);
err1:
	if(dbenv)
		dbenv->close(dbenv, 0);
err0:
	return ret;
}

