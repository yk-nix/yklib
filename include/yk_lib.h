#ifndef _YK_LIB_H_
#define _YK_LIB_H_
#include <string.h>
#include <stdlib.h>

#ifdef _DEBUG
#define debug_fprintf    fprintf
#define debug_printf     printf
#else
#define debug_fprintf(a,b,...) 
#define debug_printf(a,...)
#endif /* _DEBUG */
#define yk_printf         debug_printf
#define yk_fprintf        debug_printf


#define FLAG_ON           1
#define FLAG_OFF          0
#define EXIT_SUCCESS      0
#define EXIT_FAILED       -1
//#define EXIT_FAILURE	  EXIT_FAILED
#define YK_NAME_MAX       256
#define YK_KEY_INVALID	  -1

/* you must make sure the array is primitime data type,
 * that means you can't use it to calcuate such as struct array*/
#ifndef __cplusplus__
#define BUILD_BUG_ON_ZERO(e)  sizeof(struct {int:-!!(e);})
#define BUILD_BUG_ON_NULL(e)  ((void*)sizeof(struct {int:-!!(e);}))
#define __must_be_array(a)    BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(typeof(a),typeof(&a[0])))
#define ARRAY_SIZE(a)         (sizeof(a)/(sizeof((a)[0])) + __must_be_array(a))
#define YK_ARRAY_SIZE         ARRAY_SIZE
#endif

#define offsetof(type, member) ((size_t)&((type *)0)->member)
#define yk_offsetof            offsetof

typedef struct yk_dict {
	int	key;
	char	name[YK_NAME_MAX];
	char	*data;
	char	reserve[8];
} yk_dict_t;

/* defined int src/term/termios.c */
/* yk_dict_t sampe[] = { {xx, xxx, xxx, xxx},{YK_KEY_INVALID, "", NULL, ""}}; */
static inline int yk_dict_name2id(yk_dict_t d[], char *name) {
        int i = 0;
        for(; d[i].key != YK_KEY_INVALID; i++) {
                if(strcmp(d[i].name, name) == 0)
                        return d[i].key;
        }
        return  YK_KEY_INVALID;
}
static inline char *yk_dict_id2name(yk_dict_t d[], int id) {
        int i = 0;
        for(; d[i].key != YK_KEY_INVALID; i++) {
                if(d[i].key == id)
                        return d[i].name;
        }
        return NULL;
}

#define yk_err2(msg, e) \
do { \
	perror(msg); \
	exit(e); \
} while(0)
#define yk_err(msg)   yk_err2(msg, 1)


#define roundup_div(x, y)  (((x) + (y) - 1)/(y))
#define roundup(x, y) ((x) + (x)%(y))

#endif /* _YK_LIB_H_ */
