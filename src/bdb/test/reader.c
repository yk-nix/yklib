#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <db.h>

typedef struct student_t {
	int  id;
	char name[20];
}STUDENT;

void lock_test(DB_ENV *dbenv) {
	
}

int main(int argc, char *argv[]) {
	int ret = 1;
	DB_ENV *dbenv;
	DB     *dbp;
	DBT    k, v;
	DBC    *cur;
	u_int32_t flags;
	if(ret = db_env_create(&dbenv, 0)) {
		fprintf(stderr, "error creating dbenv handle\n");
		goto err0;
	}

	if(argc == 2) {
		if(ret = dbenv->set_encrypt(dbenv, argv[1], DB_ENCRYPT_AES)) {
			dbenv->err(dbenv, ret, "dbenv->set_encrypt error");
			goto err1;
		}
	}
	flags = DB_INIT_LOCK|DB_CREATE|DB_INIT_TXN|DB_INIT_MPOOL|DB_THREAD|DB_ENCRYPT;
	if(ret = dbenv->open(dbenv, "env", flags, 0)) {
		dbenv->err(dbenv, ret, "dbenv->open error");
		goto err1;
	}
	if(ret = db_create(&dbp, dbenv, 0)) {
		dbenv->err(dbenv, ret, "error creating db handle");
		goto err1;
	}
	//flags = DB_CREATE|DB_THREAD|DB_ENCRYPT;
	flags = DB_CREATE|DB_ENCRYPT;
	if(ret = dbp->open(dbp, NULL, "test.dat", "teachers", DB_BTREE, flags, 0)) {
		dbenv->err(dbenv, ret, "error create/open database[students]");
		goto err2;
	} 
	if(ret = dbp->cursor(dbp, NULL, &cur, 0)) {
		dbenv->err(dbenv, ret, "db cursor handler error");
		goto err2;
	}
	while(1) {
		memset((void*)&k, 0, sizeof(DBT));
		memset((void*)&v, 0, sizeof(DBT));
		ret = cur->get(cur, &k, &v, DB_NEXT);
		if(ret == DB_NOTFOUND)
			break;
		else if(ret) {
			dbenv->err(dbenv, ret, "cur->get error");
			break;
		}
		STUDENT *stu = (STUDENT *)v.data;
		fprintf(stdout, "%02d  %s\n", stu->id, stu->name);
	}
	ret = 0;
err2:
	if(dbp)
		dbp->close(dbp, 0);
err1:
	if(dbenv)
		dbenv->close(dbenv, 0);
err0:
	return ret;
}

