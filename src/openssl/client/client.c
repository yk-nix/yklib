#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>



static char	*dst_entry    = "192.168.2.241:http";
static char	*dst_host_ip  = "192.168.2.241";
static short	dst_host_port = 1314;
static char	*server_name  = "192.168.2.241";
static char	*service_name = "http";
static char	*http_version = "HTTP/1.1";

enum HTTP_REQUEST_TYPE {
	HTTP_REQTYPE_GET,
	HTTP_REQTYPE_POST,
	HTTP_REQTYPE_OTHER,
	HTTP_REQTYPE_MAX,
};
struct http_req_type {
	enum HTTP_REQUEST_TYPE	id;
	char			*name;
};

static struct http_req_type http_req_types[HTTP_REQTYPE_MAX] = {
	{ HTTP_REQTYPE_GET,	"GET" },
	{ HTTP_REQTYPE_POST,	"POST" },
};

inline char *http_req_type_to_name(enum HTTP_REQUEST_TYPE type) {
	int i = 0;
	for(; i < HTTP_REQTYPE_MAX; i++)
		if(http_req_types[i].id == type)
			return http_req_types[i].name;
	return NULL;
}
inline enum HTTP_REQUEST_TYPE http_req_name_to_type(char *name) {
	int i = 0;
	for(; i < HTTP_REQTYPE_MAX; i++)
		if(strcasecmp(http_req_types[i].name, name) == 0)
			return http_req_types[i].id;
	return HTTP_REQTYPE_OTHER;

}

inline char *make_hostname(char *server, char *service, char *hostname) {
	sprintf(hostname, "%s:%s", server, service);
	return hostname;
}

static void conn_dump_info(BIO *conn) {
	if(conn == NULL)
		return;
	fprintf(stdout, "hostname: %s\n", BIO_get_conn_hostname(conn));
	fprintf(stdout, "    port: %s\n", BIO_get_conn_port(conn));
}

static void cache_dump_info(BIO *cache) {
	int bytes;
	int buf[1024];
	BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);
	memset(buf, 0, sizeof(buf));
	while((bytes = BIO_read(cache, buf, sizeof(buf))) > 0) {
		BIO_write(out, buf, bytes);
		memset(buf, 0, sizeof(buf));
	}
	BIO_flush(out);
	BIO_free(out);
}

size_t http_get_page(enum HTTP_REQUEST_TYPE req_type, char *path, BIO *conn, BIO *cache) {
	int bytes = 0;
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	bytes = sprintf(buf, "%s %s %s\n\n", http_req_type_to_name(req_type),
			path, http_version);
	BIO_write(conn, buf, bytes);
	BIO_flush(conn);
	memset(buf, 0, sizeof(buf));
	while((bytes = BIO_read(conn, buf, sizeof(buf))) > 0) {
		BIO_write(cache, buf, bytes);
		memset(buf, 0, sizeof(buf));
	}
	return BIO_ctrl_pending(cache);
}

int init(int argc, char *argv[]) {
	if(argc == 2)
		server_name = argv[1];
}

int main(int argc, char *argv[]) {
	ERR_load_crypto_strings();
	char hostname[128];
	init(argc, argv);
	memset(hostname, 0, sizeof(hostname));
	BIO *conn = BIO_new_connect(make_hostname(server_name, service_name, hostname));
	if(BIO_do_connect(conn) <= 0) {
		fprintf(stderr, "Error: BIO_new_connect: failed to connect to server.\n");
		ERR_print_errors_fp(stderr);
	}
	conn_dump_info(conn);
	BIO *cache = BIO_new(BIO_s_mem());
	printf(" page size: %d bytes\n", 
		http_get_page(HTTP_REQTYPE_GET, "/", conn, cache));
	cache_dump_info(cache);
	BIO_free(conn);
	BIO_free(cache);
}
