
TARGET	:= $(basename $(TARGET))_sched.o
OBJS	:= sched.o

export TARGET

$(TARGET):
	@$(foreach obj, $(OBJS), \
		$(CC) -I$(INC_DIR) -fPIC -c $(basename $(obj)).c; \
	 )
	@$(foreach obj, $(OBJS), \
		$(MV) $(obj) $(TMP_DIR)$(basename $(TARGET))_$(obj); \
	)


