#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <linux/slab.h>

#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YOKA");
MODULE_DESCRIPTION("dump list_head data-structure");
MODULE_VERSION("v1.0.0");

#define NAME_SIZE	30

static struct list_head namelist;
static struct mutex lock;

struct name{
	struct list_head list;
	char   name[NAME_SIZE];
	long    age;
};

static int add_one(char *name, int name_len, char *age, int age_len)
{
	struct name *entry;
	int err = -ENOMEM;
	mutex_lock(&lock);
	entry = kmalloc(sizeof(*entry), GFP_KERNEL);
	if(entry != NULL){
		if(name_len > 0)
			memcpy(entry->name, name, name_len > NAME_SIZE ? NAME_SIZE : name_len);
		if(age_len > 0)
			if(kstrtol(age, 0, &entry->age)) entry->age = 999;
		list_add(&entry->list, &namelist);
		err = 0;
	}
	mutex_unlock(&lock);
	return err;
}

static ssize_t _seq_write(struct file *filp, const char  __user *buf, size_t len, loff_t *off)
{
	int err = -1;
	char *name = NULL, *age = NULL;
	int i = 0, loop = 0, name_len = 0, age_len = 0;
	char *data;
	if(len <= 0) return 0;
	data = kmalloc(len, GFP_KERNEL);
	if(data == NULL) return err;
	err = copy_from_user(data, buf, len);
	if(err) return err;
	err = -1;
parse_loop:
	while(data[i] == ' ' || data[i] == '\t' || data[i] == '\n'){
		i++;
		if(i >= len) goto parse_end;
	}
	if(loop == 0) name = data + i;
	if(loop == 1) age = data + i;
	while(data[i] != ' ' && data[i] != '\t' && data[i] != '\n'){
		if(loop == 0)
			name_len++;
		if(loop == 0)
			age_len++;
		i++;
		if(i >= len) goto parse_end;
	}
	loop++;
	if(loop <= 2) goto parse_loop;
parse_end:
	err = add_one(name, name_len, age, age_len);
	if(data) kfree(data);
	return err;
}

static int _seq_show(struct seq_file *m, void *entry)
{
	struct name *name = list_entry(entry, struct name, list);
	seq_printf(m, "name: %s        age:%ld\n", name->name, name->age);
	return 0;
}

static void *_seq_start(struct seq_file *m, loff_t *pos)
{
	mutex_lock(&lock);
	return seq_list_start(&namelist, *pos);
}

static void _seq_stop(struct seq_file *m, void *p)
{
	mutex_unlock(&lock);
}

static void *_seq_next(struct seq_file *m, void *p, loff_t *pos)
{
	return seq_list_next(p, &namelist, pos);
}

static struct seq_operations seq_ops = {
	.start = _seq_start,
	.next  = _seq_next,
	.stop  = _seq_stop,
	.show  = _seq_show
};

static int _seq_open(struct inode *indoe, struct file *file)
{
	return seq_open(file, &seq_ops);
}

static struct file_operations _seq_fops = {
	.open = _seq_open,
	.read = seq_read,
	.write = _seq_write,
	.llseek = seq_lseek,
	.release = seq_release
};

static void clean_all(struct list_head *head)
{
	struct name *entry;
	while(!list_empty(head)){
		entry = list_entry(head->next, struct name, list);
		list_del(&entry->list);
		kfree(entry);
	}
}

static int __init init(void)
{
	//struct proc_dir_entry *entry;

	mutex_init(&lock);
	INIT_LIST_HEAD(&namelist);
	
	add_one("yoka", 4, "32", 2);
	
	proc_create("namelist", 0, NULL, &_seq_fops);
	
	printk(KERN_INFO "/proc/namelist created\n");
	
	return 0;
}

static void __exit fini(void)
{
	remove_proc_entry("namelist", NULL);
	clean_all(&namelist);
	printk(KERN_INFO "/proc/namelist deleted\n");
}

module_init(init);
module_exit(fini);
