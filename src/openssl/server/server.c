#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <syslog.h>

extern yk_hexdump(char *, int);

static char *http_port_name = "*:1314";
static char conn_info_buf[128];
static char *progname = "openssl-server";

/* log levels:
 * 	LOG_EMERG, LOG_ALERT, LOG_ERR, LOG_WARNING
 * 	LOG_DEBUG, LOG_INFO, LOG_CRIT, LOG_NOTICE
 */
static void log_printf(int level, char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	vsyslog(LOG_USER|level, fmt, va);
	va_end(va);
}

static int init(int argc, char *argv[]) {
	/* make unified log format */
	openlog(progname, LOG_PID|LOG_CONS, LOG_USER);
}

static char *get_conn_info(BIO *conn) {
	int sock = BIO_get_fd(conn, NULL);
	struct sockaddr_in addr;
	int len = sizeof(addr);
	if(sock < 0)
		return;
	memset((void *)&addr, 0, len);
	memset(conn_info_buf, 0, sizeof(conn_info_buf));
	if(getpeername(sock, (struct sockaddr *)&addr, &len))
		 log_printf(LOG_ERR, "failed to peer host info: getpeername: %m\n");
	snprintf(conn_info_buf, sizeof(conn_info_buf),
		"%s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	return conn_info_buf;
}
static void conn_dump_info(BIO *conn) {
	sprintf("connection: %s\n", get_conn_info(conn));
}

static void *conn_handler(void *arg) {
	BIO *conn = (BIO *)arg;
	BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);
	char buf[1024];
	int bytes;
	log_printf(LOG_INFO, "connected: %s\n", get_conn_info(conn));
	memset(buf, 0, sizeof(buf));
	while((bytes = BIO_read(conn, buf, sizeof(buf))) > 0) {
		BIO_write(out, buf, bytes);
		BIO_flush(out);
		memset(buf, 0, sizeof(buf));
	}
	log_printf(LOG_INFO, "disconnected: %s\n", get_conn_info(conn));
	BIO_free(conn);
}

void main(int argc, char *argv[]) {
	init(argc, argv);

	BIO *abio, *conn;
	ERR_load_crypto_strings();
	abio = BIO_new_accept(http_port_name);
	if(BIO_do_accept(abio) <= 0) {
		fprintf(stderr, "Error setup accept\n");
		ERR_print_errors_fp(stderr);
		goto exit;
	}
	while(1) {
		log_printf(LOG_INFO, "server started, waiting from connection requests....");
		if(BIO_do_accept(abio) <= 0) {
			fprintf(stderr, "Error accept connection\n");
			ERR_print_errors_fp(stderr);
			goto exit;
		}
		conn = BIO_pop(abio);
		pthread_t thd;
		if(pthread_create(&thd, NULL, conn_handler, (void*)conn)) {
			log_printf(LOG_ERR, "failed to create thread: pthread_create:%m\n");
			log_printf(LOG_INFO, "failed to create connection handler, close the connection\n");
			BIO_free(conn);
		}
	}
exit:
	if(abio)
		BIO_free(abio);
}
