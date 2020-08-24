#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <db.h>
#include <time.h>

void lock_test(DB_ENV *dbenv) {
	
}

void lock_dump(DB_LOCK *lock) {
	printf("off: %d\n", lock->off);
	printf("ndx: %d\n", lock->ndx);
	printf("gen: %d\n", lock->gen);
	printf("mod: %d\n", lock->mode);
}

void lock_stat_dump(DB_LOCK_STAT *stat) {
	//printf("st_lastid: %d\n", stat->st_lastid);
	printf("st_nmodes: %d\n", stat->st_nmodes);
	printf("st_maxlocks: %d\n", stat->st_maxlocks);
	printf("st_nlockers: %d\n", stat->st_nlockers);
	printf("st_nrequests: %d\n", stat->st_nrequests);
	printf("st_nreleases: %d\n", stat->st_nreleases);
	printf("st_ndeadlocks: %d\n", stat->st_ndeadlocks);
	printf("st_regsize: %d\n", stat->st_regsize);
	printf("st_region_wait: %d\n", stat->st_region_wait);
	printf("st_region_nowait: %d\n", stat->st_region_nowait);
}

int main(int argc, char *argv[]) {
	int ret;
	DB_ENV *dbenv;
	u_int32_t flags;
	if(ret = db_env_create(&dbenv, 0)) {
		fprintf(stderr, "error creating dbenv handle\n");
		goto err0;
	}
	flags = DB_INIT_LOCK;
	if(ret = dbenv->open(dbenv, "env", flags, 0)) {
		dbenv->err(dbenv, ret, "dbenv->open error");
		goto err1;
	}
	DB_LOCK_STAT *stat;
	if(ret = dbenv->lock_stat(dbenv, &stat, 0)) {
		dbenv->err(dbenv, ret, "dbenv->lock_stat");
		goto err1;
	}
	//lock_stat_dump(stat);
	dbenv->lock_stat_print(dbenv, 0);
	free(stat);
	ret = 0;
err1:
	if(dbenv)
		dbenv->close(dbenv, 0);
err0:
	return ret;
}

