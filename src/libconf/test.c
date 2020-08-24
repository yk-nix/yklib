#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <libconfig.h>

void create(int argc, char *argv[]) {
	config_t conf;
	config_setting_t *root, *elm;
	config_init(&conf);
	root = config_root_setting(&conf);
	if(!root)
		fprintf(stderr, "config_root_setting: %s\n", strerror(errno));
	//printf("root:%s\n", config_setting_name(root));
	elm = config_setting_add(root, "name", CONFIG_TYPE_STRING);
	if(elm == NULL) {
		fprintf(stderr, "config_setting_add: %s\n", strerror(errno));
		goto exit;
	}
	config_setting_set_string(elm, "yui");
	
	elm = config_setting_add(root, "name", CONFIG_TYPE_STRING);
	if(elm == NULL) {
		fprintf(stderr, "erro: config_setting_add: %s\n", strerror(errno));
		goto exit;
	}
	config_setting_set_string(elm, "lisa");
	config_write_file(&conf, "test.conf");
exit:
	config_destroy(&conf);
	return;
}

void parse_test(char *filename) {
	config_t *c = (config_t *)malloc(sizeof(config_t));
	if(!c)
		return;
	if(config_read_file(c, filename) == CONFIG_FALSE) {
		fprintf(stderr, "parse config error at file:%s  line: %d  reason:%s\n",
				config_error_file(c), config_error_line(c), config_error_text(c));
	}
	printf("parse %s succeeded.\n", filename);
	const char *name;
	int age;
	config_setting_t *root = config_root_setting(c);
	config_setting_t *s = config_lookup(c, ".name");
	if(s) {
		printf("name = %s\n", config_setting_get_string(s));
	}

	if(config_setting_lookup_string(root, "name", &name) == CONFIG_TRUE) 
		printf("name: %s\n", name);
	if(config_setting_lookup_int(root, "age", &age) == CONFIG_TRUE)
		printf("age: %d\n", age); 
	config_destroy(c);
	free(c);
}

void main(int argc, char *argv[]) {
	if(argc > 1)
		parse_test(argv[1]);

}
