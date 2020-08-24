#ifndef _YK_SCHED_H_
#define _YK_SCHED_H_
/*
 *
 */
extern int yk_sched_policy_name2id(const char *name);
extern const char *yk_sched_policy_id2name(int id);
extern void yk_sched_policy_dump();
#endif /* _YK_SCHED_H_ */
