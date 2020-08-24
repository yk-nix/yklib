
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <dbus/dbus.h>

#include "yk_dbus.h"

/*
 * Broadcast message to all applications connected to the same DBus.
 */
int yk_dbus_send_signal(DBusMessageAddress *src, DBusMessageAddress *dst, DBusMessage *msg)
{
	int 			ret = -1;
	DBusConnection		*conn;
	dbus_uint32_t		serial;
	
	if(src == NULL || dst == NULL || msg == NULL)
		goto err0;
	
	if(src->BusType != dst->BusType)
		goto err0;
	
	conn = dbus_bus_get(src->BusType, NULL);
	if(conn == NULL) {
		goto err0;
	}
	
	if(src->Name) {
		ret = dbus_bus_request_name(conn, src->Name, DBUS_NAME_FLAG_REPLACE_EXISTING, NULL);
		if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
			ret = -1;
			goto err0;
		}
	}
	
	ret = -1;
	if(dbus_connection_send(conn, msg, &serial)) {
		ret = 0;
		dbus_connection_flush(conn);
	}
err0:
	return ret;
}

DBusMessage *yk_dbus_method_call(DBusMessageAddress *src, DBusMessageAddress *dst, DBusMessage *msg)
{
	DBusMessage		*reply = NULL;
	DBusConnection		*conn;
	DBusPendingCall		*pending;
	int			ret;
	
	if(src == NULL || dst == NULL || msg == NULL)
		goto err0;
	if(src->BusType != dst->BusType)
		goto err0;
	conn = dbus_bus_get(src->BusType, NULL);
	if(conn == NULL)
		goto err0;
	if(src->Name) {
		ret = dbus_bus_request_name(conn, src->Name, DBUS_NAME_FLAG_REPLACE_EXISTING, NULL);
		if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
			goto err0;
	}
	if(!dbus_connection_send_with_reply(conn, msg, &pending, -1))
		goto err0;
	if(pending == NULL)
		goto err0;
	dbus_connection_flush(conn);
	dbus_pending_call_block(pending);
	reply = dbus_pending_call_steal_reply(pending);
	dbus_pending_call_unref(pending);
err0:
	return reply;
}

DBusMessage *yk_dbus_create_message(int type, DBusMessageAddress *dst, DBusMessageArgument **args, int num)
{
	DBusMessage	*msg = NULL;
	DBusMessageIter  it;
	int              i;
	if(args == NULL || num <= 0)
		goto err0;
	switch(type) {
	case YK_DBUS_MESSAGE_SIGNAL:
		msg = dbus_message_new_signal(dst->Path, dst->Iface, dst->Obj);
		break;
	case YK_DBUS_MESSAGE_METHODCALL:
		msg = dbus_message_new_method_call(dst->Name, dst->Path, dst->Iface, dst->Obj);
		break;
	default:
		break;
	}
	if(msg == NULL)
		goto err0;
	dbus_message_iter_init_append(msg, &it);
	for(i = 0; i < num; i++) {
		if(!dbus_message_iter_append_basic(&it, args[i]->ma_type, &args[i]->ma_data))
			goto err1;
	}
	return msg;
err1:
	dbus_message_unref(msg);
	msg = NULL;
err0:
	return msg;
}

DBusMessage *yk_dbus_recv(DBusMessageAddress *local, char *filter, DBusConnection **conn_ret)
{
	DBusMessage	*msg = NULL;
	DBusConnection	*conn;
	const char	*path;
	
	if(local == NULL)
		goto err0;
	
	conn = dbus_bus_get(local->BusType, NULL);
	if(conn == NULL)
		goto err0;
	if(conn_ret)
		*conn_ret = conn;

	if(local->Name) {
		dbus_bus_request_name(conn, local->Name, DBUS_NAME_FLAG_REPLACE_EXISTING, NULL);
	}
	
	if(filter) {
		dbus_bus_add_match(conn, filter, NULL);
		dbus_connection_flush(conn);
	}
	
	while(1) {	
		dbus_connection_read_write(conn, 0);
		msg  = dbus_connection_pop_message(conn);
		if(msg == NULL) {
			usleep(100000);
			continue;
		}
		path = dbus_message_get_path(msg);
		if(path && local->Path && !strcasecmp(local->Path, path))
			break;
		dbus_message_unref(msg);
	}
err0:
	return msg;
}

void _yk_print_message_argument(DBusMessageArgument *arg)
{
	switch(arg->ma_type) {
	case DBUS_TYPE_STRING:
		fprintf(stdout, "%s\n", arg->ma_str);
		break;
	case DBUS_TYPE_INT32:
		fprintf(stdout, "%d\n", arg->ma_int);
		break;
	default:
		break;
	}
}

void yk_dbus_dump_message(DBusMessage *msg)
{
	DBusMessageArgument  arg;
	DBusMessageIter	     it;
	
	if(msg == NULL)
		return;
	
	if(!dbus_message_iter_init(msg, &it))
		return;
	
	do {
		arg.ma_type = dbus_message_iter_get_arg_type(&it);
		dbus_message_iter_get_basic(&it, &arg.ma_data);
		_yk_print_message_argument(&arg);
	} while(dbus_message_iter_next(&it));
}



#ifdef DBUS_TEST
DBusMessageAddress sender = {
	.BusType  = DBUS_BUS_SESSION,
	.Name  = "com.sender_app",
	.Path  = "/com/sender/object"
};

DBusMessageAddress recver = {
	.BusType  = DBUS_BUS_SESSION,
	.Name  = "com.recver_app",
	.Path  = "/com/recver/object",
	.Iface = "com.recver.interface",
	.Obj   = "method"
};
void reply_method_call(DBusMessage *msg, DBusConnection *conn)
{
	DBusMessageArgument	arg;
	DBusMessageIter		it, it_resp;
	DBusMessage		*resp;
	char                    buf[1024];
	dbus_uint32_t           serial;
	void                    *value = buf;;
	
	resp = dbus_message_new_method_return(msg);
	if(resp == NULL)
		return;
	dbus_message_iter_init_append(resp, &it_resp);

	if(!dbus_message_iter_init(msg, &it))
		goto send_resp;
	do {
		memset(buf, 0, sizeof(buf));
		arg.ma_type = dbus_message_iter_get_arg_type(&it);
		dbus_message_iter_get_basic(&it, &arg.ma_data);
		switch(arg.ma_type) {
		case DBUS_TYPE_STRING:
			snprintf(buf, sizeof(buf), "resp:%s", arg.ma_str);
			dbus_message_iter_append_basic(&it_resp, DBUS_TYPE_STRING, &value);
			break;
		case DBUS_TYPE_INT32:
			*((int*)value) = arg.ma_int * 2;
			dbus_message_iter_append_basic(&it_resp, DBUS_TYPE_INT32, value);
			break;
		default:
			break;						
		}
	} while(dbus_message_iter_next(&it));
	
send_resp:
	dbus_connection_send(conn, resp, &serial);
	dbus_message_unref(resp);
}

yk_dbus_send_signal_test(int argc, char *argv[])
{
	DBusMessageArgument arguments[] = {
	  { DBUS_TYPE_STRING, "hello,world!" },
	  { DBUS_TYPE_INT32,  1013 },
	  { DBUS_TYPE_STRING, "cool" }
	};
	int i = 0;
	DBusMessageArgument *args[3];
	for(i = 0; i < 3; i++) {
		args[i] = &arguments[i];
	}
	DBusMessage *msg = yk_dbus_create_message(YK_DBUS_MESSAGE_SIGNAL, &recver, args, 3);
	yk_dbus_send_signal(&sender, &recver, msg);
}

void analyze(DBusMessageAddress *local, DBusMessage *msg, DBusConnection *conn)
{
	if(local == NULL || msg == NULL)
		return;
	
	if(dbus_message_is_signal(msg, local->Iface, local->Obj)) {
		printf("singal message recved: ");
		yk_dbus_dump_message(msg);
	}
	else if(dbus_message_is_method_call(msg, local->Iface, local->Obj)) {
		printf("method call request recved: ");
		yk_dbus_dump_message(msg);
		reply_method_call(msg, conn);
	}
	else
		fprintf(stdout, "unkown message type\n");
}

void yk_dbus_recv_test(int argc, char *argv[])
{
	DBusMessage *msg;
printf("BusType: %d\n", recver.BusType);
printf("Name: %s\n", recver.Name);
printf("Path: %s\n", recver.Path);
printf("Iface: %s\n", recver.Iface);
printf("Obj: %s\n", recver.Obj);
	DBusConnection *conn;
	while(1) {
		msg = yk_dbus_recv(&recver, "interface='com.recver.interface'", &conn);
		analyze(&recver, msg, conn);
		dbus_message_unref(msg);
	}
}

void main(int argc, char *argv[])
{
	//yk_dbus_send_signal_test(argc, argv);
	yk_dbus_recv_test(argc, argv);
}
#endif
