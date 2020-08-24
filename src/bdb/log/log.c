#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <db.h>

void * log_dump(DB_ENV *dbenv) {
	int ret;
	DB_LSN       lsn;
	DB_LOGC      *cur;
	DBT          log;
	if(ret = dbenv->log_cursor(dbenv, &cur, 0)) {
		dbenv->err(dbenv, ret, "dbenv->log_cursor");
		return;
	}
	memset((void *)&lsn, 0, sizeof(lsn));
	memset((void *)&log, 0, sizeof(log));
	while((ret = cur->get(cur, &lsn, &log, DB_NEXT)) == 0) {
		printf("%.*s\n", log.size, log.data);
	}
	cur->close(cur, 0);
}

int main(int argc, char *argv[]) {
	DB_ENV       *dbenv;
	int          ret;
	u_int32_t    flags;

	if(ret = db_env_create(&dbenv, 0)) {
		fprintf(stderr, "error: db_env_create: %s\n", db_strerror(ret));
		goto err0;
	}
	dbenv->set_errfile(dbenv, stderr);
	dbenv->set_errpfx(dbenv, "error");
	flags = DB_CREATE|DB_INIT_LOG;
	if(ret = dbenv->open(dbenv, NULL, flags, 0)) {
		dbenv->err(dbenv, ret, "dbenv->open");
		goto err1;
	}
	if(argc < 2) 
		log_dump(dbenv);
	else {
		DB_LSN lsn;
		memset((void *)&lsn, 0, sizeof(lsn));
		DBT log;
		log.data = argv[1];
		log.size = strlen(argv[1]);
		if(ret = dbenv->log_put(dbenv, &lsn, &log, DB_FLUSH)) {
			dbenv->err(dbenv, ret, "dbenv->log_put");
			goto err1;
		}
		if(ret = dbenv->log_printf(dbenv, NULL, "extra info: %s", argv[1])) {
			dbenv->err(dbenv, ret, "dbenv->log_printf");
		}
	}
	ret = 0;
err1:
	if(dbenv)
		dbenv->close(dbenv, 0);
err0:
	return ret;
}

