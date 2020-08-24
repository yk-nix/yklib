
#include <linux/types.h>
#include <linux/init.h>
#include <linux/module.h>


void say_hello(char *name){
	if(name == NULL) return;
	printk(KERN_INFO "hello, %s!\n", name);
}

void say_bye(char *name){
	if(name == NULL) return;
	printk(KERN_INFO "bye, %s!\n", name);
}
