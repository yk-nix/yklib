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
	int ret;
	DB_ENV *dbenv;
	DB     *dbp;
	DB_TXN *txn;
	DBT    k, v;
	u_int32_t flags;
	if(ret = db_env_create(&dbenv, 0)) {
		fprintf(stderr, "error creating dbenv handle\n");
		goto err0;
	}
	if(ret = dbenv->set_encrypt(dbenv, "yoka", DB_ENCRYPT_AES)) {
		dbenv->err(dbenv, ret, "dbenv->set_encrypt error");
		goto err1;
	}
	
	flags = DB_INIT_LOCK|DB_CREATE|DB_INIT_TXN|DB_INIT_MPOOL|DB_THREAD|DB_ENCRYPT;
	if(ret = dbenv->open(dbenv, "env", flags, 0)) {
		dbenv->err(dbenv, ret, "dbenv->open error");
		goto err1;
	}
	if(ret = dbenv->txn_begin(dbenv, NULL, &txn, 0)) {
		dbenv->err(dbenv, ret, "error create txn handle");
		goto err1;
	}
	if(ret = db_create(&dbp, dbenv, 0)) {
		dbenv->err(dbenv, ret, "error creating db handle");
		goto err1;
	}
	flags = DB_CREATE|DB_THREAD|DB_ENCRYPT;
	if(ret = dbp->open(dbp, txn, "test.dat", "teachers", DB_BTREE, flags, 0)) {
		dbenv->err(dbenv, ret, "error create/open database[students]");
		goto err2;
	} 
	int i = 0;
	STUDENT stu;
	for(; i < 20; i++) {
		memset((void *)&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "name-%02d", i);
		memset((void*)&k, 0, sizeof(DBT));
		memset((void*)&v, 0, sizeof(DBT));
		k.data = &i;
		k.size = sizeof(int);
		v.data = &stu;
		v.size = sizeof(STUDENT);
		if(ret = dbp->put(dbp, txn, &k, &v, 0)) {
			dbenv->err(dbenv, ret, "error puting data[%d, %s] into database", stu.id, stu.name);
		}
	}
	if(ret = txn->commit(txn, 0)) {
		dbenv->err(dbenv, ret, "error doing commit");
		goto err2;
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

