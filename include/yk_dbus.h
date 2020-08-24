#include <dbus/dbus.h>

#define YK_DBUS_MESSAGE_SIGNAL		1
#define YK_DBUS_MESSAGE_METHODCALL	2

typedef struct _DBusMessageArgument {
	int	ma_type;
	union {
		char	*String;
		int      Integer;
	} ma_data;
#define ma_str		ma_data.String
#define ma_int		ma_data.Integer
} DBusMessageArgument;

typedef struct DBusMessageAddress {
	DBusBusType BusType;
	char	*Name;
	char	*Path;
	char	*Iface;
	char	*Obj;
} DBusMessageAddress;

