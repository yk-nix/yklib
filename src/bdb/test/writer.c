#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <db.h>

typedef struct student_t {
	int  id;
	char name[20];
}STUDENT;

static char *password = "yoka";
static int id = 0;
static char *name = "-";
void init(int argc, char* argv[]) {
	int opt;
	while((opt = getopt(argc, argv, "P:")) != -1) {
		switch(opt) {
		case 'P':
			password = optarg;
			break;
		default:
			break;
		}
	}

	if(argv[optind])
		id = atoi(argv[optind]);
	if(argv[optind+1])
		name = argv[optind+1];
}
void lock_test(DB_ENV *dbenv) {
	
}

int main(int argc, char *argv[]) {
	int ret = 1;
	DB_ENV *dbenv;
	DB     *dbp;
	DBT    k, v;
	DBC    *cur;
//	DB_TXN *txn;
	u_int32_t flags;
	
	init(argc, argv);
	
	if(ret = db_env_create(&dbenv, 0)) {
		fprintf(stderr, "error creating dbenv handle\n");
		goto err0;
	}
	if(ret = dbenv->set_encrypt(dbenv, password, DB_ENCRYPT_AES)) {
		dbenv->err(dbenv, ret, "dbenv->set_encrypt error");
		goto err1;
	}
	flags = DB_INIT_LOCK|DB_CREATE|DB_INIT_TXN|DB_INIT_MPOOL|DB_THREAD|DB_ENCRYPT;
	if(ret = dbenv->open(dbenv, "env", flags, 0)) {
		dbenv->err(dbenv, ret, "dbenv->open error");
		goto err1;
	}
//	if(ret = dbenv->txn_begin(dbenv, NULL, &txn, 0)) {
//		dbenv->err(dbenv, ret, "txn_begin error");
//		goto err1;
//	}
	if(ret = db_create(&dbp, dbenv, 0)) {
		dbenv->err(dbenv, ret, "error creating db handle");
		goto err1;
	}
	//flags = DB_CREATE|DB_THREAD|DB_ENCRYPT;
	flags = DB_CREATE|DB_ENCRYPT;
	//if(ret = dbp->open(dbp, txn, "test.dat", "teachers", DB_BTREE, flags, 0)) {
	if(ret = dbp->open(dbp, NULL, "test.dat", "teachers", DB_BTREE, flags, 0)) {
		dbenv->err(dbenv, ret, "error create/open database[students]");
		goto err2;
	} 
	//if(ret = dbp->cursor(dbp, txn, &cur, 0)) {
	if(ret = dbp->cursor(dbp, NULL, &cur, 0)) {
		dbenv->err(dbenv, ret, "db cursor handler error");
		goto err2;
	}

	STUDENT stu;
	stu.id = id;
	snprintf(stu.name, sizeof(stu.name), "%s", name);
	memset((void*)&k, 0, sizeof(DBT));
	memset((void*)&v, 0, sizeof(DBT));
	k.data = &stu.id;
	k.size = sizeof(stu.id);
	v.data = &stu;
	v.size = sizeof(STUDENT);
printf("try to put data[%d, %s] into db.\n", stu.id, stu.name);
	if(ret = cur->put(cur, &k, &v, DB_KEYLAST)) {
		dbenv->err(dbenv, ret, "cur->put error");
		goto err3;
	}
printf("put fin.\n");
	char cmd[128];
	while(1) {
		memset(cmd, 0, sizeof(cmd));
		printf("please input cmd:");
		scanf("%s", cmd);
		if(strcasecmp(cmd, "exit") == 0) {
			break;
		}
		else if (strcasecmp(cmd, "commit") == 0) {
			cur->close(cur);
//			if(ret = txn->commit(txn, 0)) {
//				dbenv->err(dbenv, ret, "tnx-commit error");
//			}
		}
	}
	ret = 0;
err3:
	if(cur)
		cur->close(cur);
err2:
	if(dbp)
		dbp->close(dbp, 0);
err1:
	if(dbenv)
		dbenv->close(dbenv, 0);
err0:
	return ret;
}

