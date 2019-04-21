#ifndef MAP_H_
#define MAP_H_

#include <sys/types.h>

typedef struct basic_map basic_map_t;

#ifdef DEBUG
void bmap_debug_print_memory_leaks();
#endif

int basic_map_alloc(basic_map_t **);
void basic_map_free(basic_map_t *);
int basic_map_insert(basic_map_t *, void *, size_t, void *, size_t);
int basic_map_at(basic_map_t *, void *, size_t, void **, size_t *);
size_t basic_map_size(basic_map_t *);
bool basic_map_empty(basic_map_t *);
bool basic_map_contains(basic_map_t *, void *, size_t);
int basic_map_emplace(basic_map_t *, void *, size_t, void *, size_t);

#endif /* MAP_H_ */
