#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dbus/dbus.h>

#define DBUS_SENDER_NAME           "com.sender_app"
#define DBUS_RECVER_NAME           "com.recver_app"
#define DBUS_RECVER_PATH           "/com/recver/object"
#define DBUS_RECVER_INTERFACE      "com.recver.interface"
#define DBUS_RECVER_SIGNAL         "signal"
#define DBUS_RECVER_METHOD         "method"
#define DBUS_RECVER_RULE           "type='signal',interface='com.recver.interface'"
#define DBUS_RECVER_REPLY          "I am %d, get a message"
#define DBUS_CLIENT_PID_FILE       "/tmp/dbus-client.pid"

#define MODE_SIGNAL                1
#define MODE_METHOD                2

#define error(prefix, e) \
do { \
	if(dbus_error_is_set(&e)) \
		printf(prefix ":%s\n", e.message); \
} while(0)


union DBusMessageValue {
	char	*string;
	int	integer;
};

void print_message(DBusMessage *msg) {
	int                    ret;
	DBusMessageIter        it;
	union DBusMessageValue value;
	if(dbus_message_iter_init(msg, &it)) {
		printf("message without argment received.\n");
		return;
	}
	ret = dbus_message_iter_get_arg_type(&it);
	switch(ret) {
	case DBUS_TYPE_STRING:
		dbus_message_iter_get_basic(&it, &value);
		printf("message[%s] received.\n", value.string);
		break;
	case DBUS_TYPE_INT32:
		dbus_message_iter_get_basic(&it, &value);
		printf("message[%d] received.\n", value.integer);
		break;
	default:
		printf("message with argument type error received.\n");
	}
	
}

void reply_method_call(DBusMessage *msg, DBusConnection *conn)
{
	DBusMessage     *reply;
	DBusMessageIter it, it2;
	int             val_int = getpid();
	char           *val_str = "hello,coool!";
	dbus_uint32_t    serial;
	
	reply = dbus_message_new_method_return(msg);
	if(reply == NULL) {
		printf("dbus_message_new_method_retur: nout of memory!\n");
		return;
	}
	
	dbus_message_iter_init_append(reply, &it);
	if(!dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, val_str)) {
		printf("dbus_emssage_iter_append_basic: out of memory!\n");
		goto out;
	}
	if(!dbus_message_iter_init(msg, &it2)) {
		printf("message has no arugment\n");
		goto out;
	}
	do {
		int type = dbus_message_iter_get_arg_type(&it2);
		union DBusMessageValue value;
		switch(type) {
		case DBUS_TYPE_STRING:
			dbus_message_iter_get_basic(&it2, &value);
			printf("message[%s] received\n", value.string);
			if(!dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, "reply")) {
				printf("dbus_message_iter_append_basic: out of memory!\n");
				goto out;
			}
			break;
		case DBUS_TYPE_INT32:
			dbus_message_iter_get_basic(&it2, &value);
			printf("message[%d] received\n", value.integer);
			value.integer = getpid();
			if(!dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, &value)) {
				printf("dbus_message_iter_append_basic: out of memory!\n");
				goto out;
			}
			break;
		default:
			printf("message with unkown type recevied.\n");
			break;
		}
		
	} while(dbus_message_iter_next(&it2));
	
	if(!dbus_connection_send(conn, reply, &serial)) {
		printf("dbus_connection_send: out of memory!");
		goto out;
	}
out:
	dbus_message_unref(reply);
}

void analyze(DBusMessage *msg, DBusConnection *conn)
{
	if(dbus_message_is_signal(msg, DBUS_RECVER_INTERFACE, DBUS_RECVER_SIGNAL)) {
		print_message(msg);
	}
	else if(dbus_message_is_method_call(msg, DBUS_RECVER_INTERFACE, DBUS_RECVER_METHOD)) {
		reply_method_call(msg, conn);
	}
	else {
		printf("unkown message type\n");
	}
}

void dbus_recver(void)
{
	int             ret;
	DBusConnection  *conn;
	DBusMessage     *msg;
	DBusError       err;
	
	dbus_error_init(&err);
	
	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if(conn == NULL) {
		error("connection err", err);
		goto err0;
	}
	
	ret = dbus_bus_request_name(conn, DBUS_RECVER_NAME, 
				DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
	if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		error("name error", err);
		goto err0;
	}
	
	dbus_bus_add_match(conn, DBUS_RECVER_RULE, &err);
	dbus_connection_flush(conn);
	if(dbus_error_is_set(&err)) {
		printf("match error: %s\n", err.message);
		goto err0;
	}

	while(1) {
		dbus_connection_read_write(conn, 0);
		
		msg = dbus_connection_pop_message(conn);
		if(msg == NULL) {
			usleep(500000);
			continue;
		}
		
		const char *path = dbus_message_get_path(msg);
		if(strcasecmp(DBUS_RECVER_PATH, path)) {
			printf("wrong path: %s\n", path);
			goto next;
		}
	
		analyze(msg, conn);
next:		
		dbus_message_unref(msg);
	}
err1:
	
err0:
	dbus_error_free(&err);
	return;
}


void dbus_send(int mode, char *type, void *value)
{
	DBusMessage     *msg;
	DBusMessageIter  it;
	DBusError        err;
	DBusConnection  *conn;
	DBusPendingCall *pending;
	dbus_uint32_t    serial;
	int              ret;
	
	dbus_error_init(&err);
	
	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if(conn == NULL) {
		error("connection error", err);
		goto err0;
	}
	
	ret = dbus_bus_request_name(conn, DBUS_SENDER_NAME, 
				DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
	if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		error("name error", err);
	}
	
	int   _type;
	int   val_int;
	char *val_str;
	void  *val;
	if(!strcasecmp(type, "STRING")) {
		_type = DBUS_TYPE_STRING;
		val_str = value;
		val = &val_str;
	}
	else if(!strcasecmp(type, "INT32")) {
		_type = DBUS_TYPE_INT32;
		val_int = atoi(value);
		val = &val_int;
	}
	else {
		printf("wrong argument type.\n");
		goto err0;
	}
	if(mode == MODE_METHOD) {
		printf("call app[bus_name]=%s, object[path]=%s, interface=%s, method=%s\n", 
			DBUS_RECVER_NAME, DBUS_RECVER_PATH, DBUS_RECVER_INTERFACE, DBUS_RECVER_METHOD);
		msg = dbus_message_new_method_call(DBUS_RECVER_NAME, DBUS_RECVER_PATH,
				DBUS_RECVER_INTERFACE, DBUS_RECVER_METHOD);
		if(msg == NULL) {
			printf("error dbus_message_new_method_call\n");
			goto err0;
		}
		dbus_message_iter_init_append(msg, &it);
		if(!dbus_message_iter_append_basic(&it, _type, val)) {
			printf("dbus_message_iter_append_basic: out of memory!\n");
			goto err1;
		}
		if(!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
			printf("out of memory\n");
			goto err1;
		}
		if(pending == NULL) {
			printf("pending call nulll\n");
			goto err1;
		}
		dbus_connection_flush(conn);
		dbus_message_unref(msg);
		dbus_pending_call_block(pending);
		msg = dbus_pending_call_steal_reply(pending);
		if(msg == NULL) {
			printf("replay null\n");
			goto err0;
		}
		dbus_pending_call_unref(pending);
		if(!dbus_message_iter_init(msg, &it)) {
			goto err1;
		}
		do {
			int __type = dbus_message_iter_get_arg_type(&it);
			union DBusMessageValue _val;
			switch(__type) {
			case DBUS_TYPE_STRING:
				dbus_message_iter_get_basic(&it, &_val);
				printf("resp: %s\n", _val.string);
				break;
			case DBUS_TYPE_INT32:
				dbus_message_iter_get_basic(&it, &_val);
				printf("resp: %d\n", _val.integer);
				break;
			default:
				break;
			}
		}while(dbus_message_iter_next(&it));
	}
	else if(mode = MODE_SIGNAL) {
		printf("signal to object[path]=%s, interface=%s, signal=%s\n",
			DBUS_RECVER_PATH, DBUS_RECVER_INTERFACE, DBUS_RECVER_SIGNAL);
		msg = dbus_message_new_signal(DBUS_RECVER_PATH, DBUS_RECVER_INTERFACE, DBUS_RECVER_SIGNAL);
		if(msg == NULL) {
			goto err0;
		}
		dbus_message_iter_init_append(msg, &it);
		if(!dbus_message_iter_append_basic(&it, _type, val)) {
			goto err1;
		}
		if(!dbus_connection_send(conn, msg, &serial)) {
			goto err1;
		}
		dbus_connection_flush(conn);
	}
err1:
	dbus_message_unref(msg);

err0:
	dbus_error_free(&err);
}


void main(int argc, char *argv[]) {
	if(argc < 2)
		return;
	if(!strcasecmp(argv[1], "receive"))
		dbus_recver();
	else if(!strcasecmp(argv[1], "send")) {
		if(argc < 5)
			return;
		if(!strcasecmp(argv[2], "SIGNAL"))
			dbus_send(MODE_SIGNAL, argv[3], argv[4]);
		else if(!strcasecmp(argv[2], "METHOD"))
			dbus_send(MODE_METHOD, argv[3], argv[4]);
	}
}
