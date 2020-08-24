#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <dbus.h>


void main(int argc, char *argv[]){
        int ret;
        DBusError err;
        DBusConnection *conn;

        dbus_error_init(&err);

        conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
        if(dbus_error_is_set(&err)){
                printf("Error: dbus_bus_get: %s\n", err.message);
                dbus_err_free(&err);
        }
        if(conn == NULL) return;
	
        ret = dbus_bus_request_name(conn, "yui.dbus", DBUS_NAME_FALG_REPLACE_EXISTING, &err);
        if(dbus_error_is_set(&err)){
                printf("Error: dbus_bus_request_name: %s\n", err.message);
                dbus_error_free(&err);
        }
        if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) return;

        dbus_connection_close(conn);
}
