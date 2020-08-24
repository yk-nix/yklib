
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/types.h>

#include <linux/fs.h>
#include <asm/uaccess.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "hello.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YOKA");
MODULE_DESCRIPTION("a helloworld module for test");
MODULE_VERSION("v1.0");

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 	0
#define DEVICE_NAME	"chardev"
#define BUF_LEN		80

static int major;
static int isopen = 0;
static char msg[BUF_LEN];
static char *msg_ptr;

static struct file_operations fops = {
	.read    = device_read,
	.write   = device_write,
	.open    = device_open,
	.release = device_release
};

static char *name = NULL;
module_param(name, charp, S_IRUGO);

#define procfs_name "helloworld"
struct proc_dir_entry *our_proc_file;

static int hello_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "Hello, World!\n");
	return SUCCESS;
}

static int hello_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, hello_proc_show, NULL);
}

static struct file_operations hello_proc_fops = {
	.owner = THIS_MODULE,
	.open  = hello_proc_open,
	.read  = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int device_open(struct inode *inode, struct file *file)
{
	static int counter = 0;
	if(isopen)
		return -EBUSY;
	isopen++;
	sprintf(msg, "I already told you %d times Hello World!\n", counter++);
	msg_ptr = msg;
	try_module_get(THIS_MODULE);
	
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
	isopen--;
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	int bytes_read = 0;
	if(*msg_ptr == 0)
		return 0;
	while(length && *msg_ptr){
		put_user(*(msg_ptr++), buffer++);
		length--;	
		bytes_read++;
	}
	return bytes_read;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t *offset)
{
	printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
	return -EINVAL;
}


int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, 
		  int *eof, void *data)
{
	int ret;
	printk(KERN_INFO "profile_read (/proc/%s) called \n", procfs_name);
	if(offset > 0){
		ret = 0;
	}
	else{
		ret = sprintf(buffer, "Hello, World!\n");
	}
	return ret;
}

static int hello_init(void)
{
	printk(KERN_INFO "helloworld-module loaded.\n");
	say_hello(name);

	major = register_chrdev(0, DEVICE_NAME, &fops);
	if(major < 0){
		printk(KERN_ALERT "register char-device failed with %d\n", major);
		return major;
	}
	printk(KERN_INFO "I was assigned major number %d. To talk to\n", major);
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, major);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");
	
	proc_create(procfs_name, 0, NULL, &hello_proc_fops);
	
	return SUCCESS;
}


static void hello_exit(void)
{
	say_bye(name);
	unregister_chrdev(major, DEVICE_NAME);
	remove_proc_entry(procfs_name, NULL);
	printk(KERN_INFO "helloworld-module unloaded.\n");
}

module_init(hello_init);
module_exit(hello_exit);
