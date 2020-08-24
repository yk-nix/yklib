obj-m := namelist.o
#test-objs := hello.o helloworld.o

KDIR  := /lib/modules/3.10.0-862.el7.x86_64/build/

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm *.symvers *.o *mod.c *.ko *.order 
