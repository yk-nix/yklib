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
	DB_LOCK   lock;
	DBT obj;
	u_int32_t stu_id = 10;
	obj.data = &stu_id;
	obj.size = sizeof(u_int32_t);
	u_int32_t id;
	if(ret = dbenv->lock_id(dbenv, &id)) {
		dbenv->err(dbenv, ret, "get lock id error");
		goto err1;
	}	
	fprintf(stdout, "lock-id: %d\n", id);
	if(ret = dbenv->lock_detect(dbenv, 0, DB_LOCK_DEFAULT, NULL)) {
		dbenv->err(dbenv, ret, "lock_detect");
		goto err2;		
	}
	ret = 0;
err2:
	dbenv->lock_id_free(dbenv, id);
err1:
	if(dbenv)
		dbenv->close(dbenv, 0);
err0:
	return ret;
}

