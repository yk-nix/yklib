#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/route.h>

typedef struct rtentry_field {
        char   *field_name;
        int     id;
} rtentry_field_t;

#define CENTOS  1
#define FEDORA  2

static rtentry_field_t centos_rt[] = {
	{"Iface", 0},
	{"Destination", 1},
	{"Gateway", 2},
	{"Flags", 3},
	{"RefCnt", 4},
	{"Use", 5},
	{"Metric", 6},
	{"Mask", 7},
	{"MTU", 8},
	{"Window", 9},
	{"IRTT", 10},
	{ NULL, -1},
};


int rtentry_field_name2id(rtentry_field_t rt[], const char *name) {
	int i = 0;
	for(; rt[i].field_name != NULL; i++) {	
		if(strcasecmp(name, rt[i].field_name) == 0)
			return rt[i].id;
	} 
	return -1;
}
const char *rtentry_field_id2name(rtentry_field_t rt[], int idx) {
	int i = 0;
	for(; rt[i].id != -1; i++) {
		if(idx == rt[i].id)
			return rt[i].field_name;
	}
	return NULL;
}

static int rtentry_sort_cmp(const void *_e1, const void *_e2) {
	struct rtentry *e1 = (struct rtentry *)_e1;
	struct rtentry *e2 = (struct rtentry *)_e2;
	return *((u_int32_t *)e2->rt_genmask.sa_data) - *((u_int32_t *)e1->rt_genmask.sa_data);
}

int centos_rt_analzye(struct rtentry **rt) {
	FILE *fp = fopen("/proc/net/route", "r");
	char buf[1024];
	int  fields[24];
	char *title, *token;
	int idx;
	char *str = buf;
	int count = 0;
	int i, id;
	struct rtentry *e;
	char * cache;
	if(fp == NULL)
		goto err0;
	memset(buf, 0, sizeof(buf));
	if(fgets(buf, sizeof(buf), fp) == NULL)
		goto err1;
	idx = 0;
	while(token = strsep(&str, " \t\r\n")) {
		if(token && token[0] != '\0') {
			id = rtentry_field_name2id(centos_rt, token);
			if(id < 0)
				goto err1;
			fields[idx++] = id;
		}
	}

	cache = malloc(sizeof(struct rtentry));
	while(fgets(buf, sizeof(buf), fp)) {
		idx = 0;
		str = buf;
		cache = realloc(cache, sizeof(struct rtentry) *(count+1));
		if(cache == NULL)
			goto err2;
		e = (struct rtentry *)(cache + sizeof(struct rtentry) * count);
		memset((void *)e, 0, sizeof(struct rtentry));
		while(token = strsep(&str, " \t\r\n")) {
			if(token && token[0] != '\0') {
				switch(fields[idx++]) {
				case 0:
					e->rt_dev = strdup(token);			
					break;
				case 1:
					sscanf(token, "%08X", (u_int32_t *)e->rt_dst.sa_data);
					break;
				case 2:
					sscanf(token, "%08X", (u_int32_t *)e->rt_gateway.sa_data);
					break;
				case 3:
					sscanf(token, "%04X", &e->rt_flags);
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					e->rt_metric = atoi(token);
					break;
				case 7:
					sscanf(token, "%08x",(u_int32_t *)e->rt_genmask.sa_data);
					break;
				case 8:
					e->rt_mtu = atoi(token);
					break;
				case 9: 
					e->rt_window = atoi(token);
					break;
				case 10: 
					e->rt_irtt = atoi(token);
					break;
				default:
					goto err1;
				}
			}
		}
		count++;
	}
	*rt = (struct rtentry *)cache;
	qsort(cache, (size_t)count, sizeof(struct rtentry), rtentry_sort_cmp);
	return count;
err2:
	e = (struct rtentry *)cache;
	for(i = 0; i < count; i++) {
		if(e->rt_dev)
			free(e->rt_dev);
	}
	free(cache);
err1:
	fclose(fp);
err0:
	return -1;
}

int yk_route_get(int ostype, struct rtentry **rt) {
	switch(ostype) {
	case CENTOS:
		return centos_rt_analzye(rt);
		break;
	case FEDORA:
	default:
		return 0;
	}
}

void yk_route_free(struct rtentry **rt, int count) {
	int i = 0;
	struct rtentry *p = *rt;
	for(; i < count; i++) {
		if(p->rt_dev)
			free(p->rt_dev);
		p++;
	}
	free(*rt);
	*rt = NULL;
}

