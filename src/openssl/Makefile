
TARGET	:= $(basename $(TARGET))_yk_openssl.o
SUB_DIR	:= rand

export TARGET

$(TARGET):
	@$(foreach dir, $(SUB_DIR), \
		make -C $(dir); \
	 )

