
TARGET	:= $(basename $(TARGET))_utils.o
OBJS	:= utils.o checksum.o

export TARGET

$(TARGET):
	@$(foreach obj, $(OBJS), \
		$(CC) -I$(INC_DIR) -fPIC -c $(basename $(obj)).c; \
	 )
	@$(foreach obj, $(OBJS), \
		$(MV) $(obj) $(TMP_DIR)$(basename $(TARGET))_$(obj); \
	)


