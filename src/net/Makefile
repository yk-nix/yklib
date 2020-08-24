
TARGET	:= $(basename $(TARGET))_yk_net.o
SUB_DIR	:= inet route link

export TARGET

$(TARGET):
	@$(foreach dir, $(SUB_DIR), \
		make -C $(dir); \
	 )

