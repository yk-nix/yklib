#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sched.h>

typedef struct _sched_policy_entry {
	int          id;
	char        *name;
} sched_policy_entry_t;

static sched_policy_entry_t sched_policy_entries[] = {
	{ SCHED_FIFO, "SCHED_FIFO" },
	{ SCHED_RR,   "SCHED_RR" },
	{ SCHED_OTHER,"SCHED_OTHER" },
#if defined (_GNU_SOURCE)
	{ SCHED_BATCH,"SCHED_BATCH" },
	{ SCHED_IDLE, "SCHED_IDLE" },
#endif
	{ -1,  NULL }
};

const char *yk_sched_policy_id2name(int id) {
	sched_policy_entry_t *p = &sched_policy_entries[0];
	for(; p->id != -1 && p->name; p++) {
		if(p->id == id)
			return p->name;
	}
	return NULL;
}

int yk_sched_policy_name2id(const char *name) {
	sched_policy_entry_t *p = &sched_policy_entries[0];
	for(; p->id != -1 && p->name; p++) {
		if(!strcasecmp(name, p->name))
			return p->id;
	}
	return -1;
}

void yk_sched_policy_dump() {
	int i = 0;
	sched_policy_entry_t *p;
	for(p = &sched_policy_entries[i]; p->id != -1 && p->name;) {
		printf("%s\n", p->name);
		i++;
		p = &sched_policy_entries[i];
	}
}
