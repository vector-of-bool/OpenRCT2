#ifndef _UTIL_PATH_H_
#define _UTIL_PATH_H_

#include <openrct2/common.h>

#ifdef __cplusplus
extern "C" {
#endif

struct orct2_path_t;
typedef struct orct2_path_t * orct2_path;

extern orct2_path path_create();
extern orct2_path path_from_string(const char * path);
extern void         path_free(orct2_path);
extern const char * path_c_str(orct2_path path);
extern char * path_strdup(orct2_path path);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _UTIL_PATH_H_