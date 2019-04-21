#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <zlib.h>

#include "basic_map.h"

#ifdef DEBUG
#define bmap_calloc(I, S) bmap_debug_calloc(I, S, __FILE__, __LINE__, __FUNCTION__)
#define bmap_free(P) bmap_debug_free(P, __FILE__, __LINE__, __FUNCTION__)
struct bmap_debug_alloc
{
    void *ptr;
    const char *file;
    int line;
    const char *func;
    size_t size;
    bool free;
    struct bmap_debug_alloc *next;
};
struct bmap_debug_alloc *bmap_debug_alloc_list = NULL;

static void
bmap_debug_alloc_list_append(struct bmap_debug_alloc *dbg, struct bmap_debug_alloc *dbg_next)
{
    if (dbg->next != NULL)
    {
        bmap_debug_alloc_list_append(dbg->next, dbg_next);
    }
    else
    {
        dbg->next = dbg_next;
    }
}

/* Internal calloc to track reserved memory */
static void *
bmap_debug_calloc(size_t items, size_t size, const char *file, int line,
                  const char *func)
{
    void *ptr = calloc(items, size);

    if (ptr != NULL)
    {
        //printf("Alloc = %s, %i, %s, %p[%li]\n", file, line, func, ptr, items*size);
        struct bmap_debug_alloc *dbg = calloc(1, sizeof(struct bmap_debug_alloc));
        if (dbg == NULL)
        {
            return NULL;
        }

        dbg->ptr = ptr;
        dbg->file = file;
        dbg->line = line;
        dbg->func = func;
        dbg->size = items * size;
        dbg->free = false;

        if (bmap_debug_alloc_list != NULL)
        {
            bmap_debug_alloc_list_append(bmap_debug_alloc_list, dbg);
        }
        else
        {
            bmap_debug_alloc_list = dbg;
        }
    }

    return ptr;
}

/* Internal free to track reserved memory */
static void
bmap_debug_free(void *ptr, const char *file, int line, const char *func)
{
    free(ptr);

    /* printf("Free  = %s, %i, %s, %p\n", file, line, func, ptr); */
    if (bmap_debug_alloc_list == NULL)
    {
        printf("ALERT: Called xfree but xalloc debug list is empty!\n");
        return;
    }

    struct bmap_debug_alloc *dbg_list_item = bmap_debug_alloc_list;
    while (dbg_list_item != NULL)
    {
        if (dbg_list_item->ptr == ptr)
        {
            dbg_list_item->free = true;
        }

        dbg_list_item = dbg_list_item->next;
    }
}

void bmap_debug_print_memory_leaks()
{
    if (bmap_debug_alloc_list != NULL)
    {
        struct bmap_debug_alloc *dbg_list_item = bmap_debug_alloc_list;
        while (dbg_list_item != NULL)
        {
            if (!dbg_list_item->free)
            {
                printf("Memory leak at = %s, %i, %s, %p[%li]\n",
                       dbg_list_item->file,
                       dbg_list_item->line,
                       dbg_list_item->func,
                       dbg_list_item->ptr,
                       dbg_list_item->size);
            }

            dbg_list_item = dbg_list_item->next;
        }
    }
}
#else
#define bmap_calloc(I, S) calloc(I, S)
#define bmap_free(P) free(P)
#endif /* DEBUG */

struct basic_map_bucket
{
    void *bkt_key;
    size_t bkt_key_size;
    void *bkt_val;
    size_t bkt_val_size;
#ifdef DEBUG
    bool bkt_is_emplaced;
#endif
    struct basic_map_bucket *bkt_next;
};

struct basic_map
{
    size_t map_size;
    struct basic_map_bucket **map_buckets;
    size_t map_buckets_size;
    /* Future reserved. Instead of attaching buckets create extends. */
    size_t map_buckets_extends;
};

int basic_map_alloc(basic_map_t **mapp)
{
    if (mapp == NULL)
    {
        return EINVAL;
    }

    if ((*mapp = bmap_calloc(1, sizeof(basic_map_t))) == NULL)
    {
        return ENOMEM;
    }

    (*mapp)->map_size = 0;

    /* TODO: Get this value from a macro */
    (*mapp)->map_buckets_size = 8000;

    if (((*mapp)->map_buckets = bmap_calloc((*mapp)->map_buckets_size, sizeof(void *))) == NULL)
    {
        return ENOMEM;
    }

    (*mapp)->map_buckets_extends = 0;

    return 0;
}

static void
bmap_bucket_free(struct basic_map_bucket *bkt)
{
    if (bkt == NULL)
    {
        return;
    }

    /* If there's an attached bucket, free it first */
    if (bkt->bkt_next != NULL)
    {
        bmap_bucket_free(bkt->bkt_next);
    }

    bmap_free(bkt->bkt_val);
    bmap_free(bkt->bkt_key);
    bmap_free(bkt);
}

static int
bmap_bucket_alloc(struct basic_map_bucket **bktp, void *key,
                  size_t key_size, void *val, size_t val_size)
{
    int err = 0;

    assert(bktp != NULL);
    assert(key != NULL);
    assert(key_size != 0);
    assert(val != NULL);
    assert(val_size != 0);

    if (((*bktp) = bmap_calloc(1, sizeof(struct basic_map_bucket))) == NULL)
    {
        return ENOMEM;
    }

    /* Set everything to NULL, in case of an error we can free safely */
    (*bktp)->bkt_key = NULL;
    (*bktp)->bkt_key_size = 0;
    (*bktp)->bkt_val = NULL;
    (*bktp)->bkt_val_size = 0;
#ifdef DEBUG
    (*bktp)->bkt_is_emplaced = false;
#endif
    (*bktp)->bkt_next = NULL;

    /* Reserve memory for the key */
    if (((*bktp)->bkt_key = bmap_calloc(1, key_size)) == NULL)
    {
        err = ENOMEM;
        goto out_free_buffer;
    }

    /* Copy the key into the bucket struct */
    bcopy(key, (*bktp)->bkt_key, key_size);
    (*bktp)->bkt_key_size = key_size;

    /* Reserve memory for the value */
    if (((*bktp)->bkt_val = bmap_calloc(1, val_size)) == NULL)
    {
        err = ENOMEM;
        goto out_free_buffer;
    }

    /* Copy the value into the bucket struct */
    bcopy(val, (*bktp)->bkt_val, val_size);
    (*bktp)->bkt_val_size = val_size;

    return 0;

out_free_buffer:
    bmap_bucket_free((*bktp));

    return err;
}

void basic_map_free(basic_map_t *map)
{
    if (map == NULL)
    {
        return;
    }

    for (int i = 0; i < map->map_buckets_size; i++)
    {
        bmap_bucket_free(map->map_buckets[i]);
    }

    bmap_free(map->map_buckets);
    bmap_free(map);
}

static void
bmap_bucket_attach(struct basic_map_bucket *bkt,
                   struct basic_map_bucket *bkt_next)
{
    assert(bkt != NULL);
    assert(bkt_next != NULL);

    /* If next bucket is empty attach given bucket */
    if (bkt->bkt_next == NULL)
    {
        bkt->bkt_next = bkt_next;
        return;
    }

    bmap_bucket_attach(bkt->bkt_next, bkt_next);
}

static inline void
i_bmap_insert(basic_map_t *map, void *key, size_t key_size,
              struct basic_map_bucket *bkt)
{
    size_t pos = 0;

    /* Get the positional index of the bucket derived from the key's CRC. */
    pos = crc32(0L, key, key_size) % map->map_buckets_size;

    /* If bucket position is empty point to the new bucket */
    if (map->map_buckets[pos] == NULL)
    {
        /* Assign new bucket */
        map->map_buckets[pos] = bkt;
    }
    else
    {
        /* Attach the new bucket to an existing bucket */
        bmap_bucket_attach(map->map_buckets[pos], bkt);
    }

    /* Increase the size counter */
    map->map_size = map->map_size + 1;
}

int basic_map_insert(basic_map_t *map, void *key, size_t key_size,
                     void *val, size_t val_size)
{
    struct basic_map_bucket *bkt = NULL;
    int err = 0;

    /* Check the pointer args */
    if (map == NULL || key == NULL || val == NULL)
    {
        return EINVAL;
    }

    /* Check the size args. We don't expect args with size zero! */
    if (key_size == 0 || val_size == 0)
    {
        return EINVAL;
    }

    /* Create a new bucket with given key and value */
    if ((err = bmap_bucket_alloc(&bkt, key, key_size, val, val_size)) != 0)
    {
        return err;
    }

    i_bmap_insert(map, key, key_size, bkt);

    return 0;
}

/* bmap_bucket_has_key compares the given key with the bucket. If buckets
   are attached those keys will be compared too, if the previous comparison
   fails. 
   Return value is the matching bucket or NULL */
static struct basic_map_bucket *
bmap_bucket_has_key(struct basic_map_bucket *bkt, void *key, size_t key_size)
{
    assert(bkt != NULL);
    assert(key != NULL);
    assert(key_size != 0);

    if (bkt->bkt_key_size != key_size)
    {
        /* If there's no attached bucket, the comparison failed */
        if (bkt->bkt_next == NULL)
        {
            return NULL;
        }

        return bmap_bucket_has_key(bkt->bkt_next, key, key_size);
    }

    if (memcmp(bkt->bkt_key, key, key_size) != 0)
    {
        /* If there's no attached bucket, the comparison failed */
        if (bkt->bkt_next == NULL)
        {
            return NULL;
        }

        return bmap_bucket_has_key(bkt->bkt_next, key, key_size);
    }

    /* Key size and key content matches. This bucket has the given key. */
    return bkt;
}

int basic_map_at(basic_map_t *map, void *key, size_t key_size, void **valp,
                 size_t *val_sizep)
{
    size_t pos = 0;
    struct basic_map_bucket *val = NULL;

    /* Check the pointer args */
    if (map == NULL || key == NULL || valp == NULL || val_sizep == NULL)
    {
        return EINVAL;
    }

    /* Check the key size arg. We don't expect a key with size zero! */
    if (key_size == 0)
    {
        return EINVAL;
    }

    /* Get the positional index of the bucket derived from the key's CRC. */
    pos = crc32(0L, key, key_size) % map->map_buckets_size;

    /* Pick the bucket */
    val = map->map_buckets[pos];
    if (val == NULL)
    {
        return ENODATA;
    }

    /* Compare keys and retreive the right bucket */
    val = bmap_bucket_has_key(val, key, key_size);
    if (val == NULL)
    {
        return ENODATA;
    }

    (*valp) = val->bkt_val;
    (*val_sizep) = val->bkt_val_size;

    return 0;
}

size_t basic_map_size(basic_map_t *map)
{
    assert(map != NULL);

    return map->map_size;
}

bool basic_map_empty(basic_map_t *map)
{
    return (basic_map_size(map) == 0) ? true : false;
}

bool basic_map_contains(basic_map_t *map, void *key, size_t key_size)
{
    size_t pos = 0;
    struct basic_map_bucket *val = NULL;

    assert(map != NULL);
    assert(key != NULL);
    assert(key_size != 0);

    /* Get the positional index of the bucket derived from the key's CRC. */
    pos = crc32(0L, key, key_size) % map->map_buckets_size;

    /* Pick the bucket */
    val = map->map_buckets[pos];
    if (val == NULL)
    {
        return false;
    }

    /* Compare keys and retreive the right bucket */
    val = bmap_bucket_has_key(val, key, key_size);
    if (val == NULL)
    {
        return false;
    }

    return true;
}

/* TODO: dupliate code, see bmap_bucket_alloc! Replace with inlines? */
static int
bmap_bucket_alloc_emplace(struct basic_map_bucket **bktp, void *key,
                          size_t key_size)
{
    int err = 0;

    assert(bktp != NULL);
    assert(key != NULL);
    assert(key_size != 0);

    if (((*bktp) = bmap_calloc(1, sizeof(struct basic_map_bucket))) == NULL)
    {
        return ENOMEM;
    }

    /* Set everything to NULL, in case of an error we can free safely */
    (*bktp)->bkt_key = NULL;
    (*bktp)->bkt_key_size = 0;
    (*bktp)->bkt_val = NULL;
    (*bktp)->bkt_val_size = 0;
#ifdef DEBUG
    (*bktp)->bkt_is_emplaced = true;
#endif
    (*bktp)->bkt_next = NULL;

    /* Reserve memory for the key */
    if (((*bktp)->bkt_key = bmap_calloc(1, key_size)) == NULL)
    {
        err = ENOMEM;
        goto out_free_buffer;
    }

    /* Copy the key into the bucket struct */
    bcopy(key, (*bktp)->bkt_key, key_size);
    (*bktp)->bkt_key_size = key_size;

    /* We don't reserve memory for the value, b/c we assign the value pointer 
       to the bucket. */

    return 0;

out_free_buffer:
    bmap_bucket_free((*bktp));

    return err;
}

/* basic_map_emplace creates a new bucket without memory copying the value! The
   given value pointer should not be freed outside the map. Be careful with this
   operation. */
int basic_map_emplace(basic_map_t *map, void *key, size_t key_size,
                      void *val, size_t val_size)
{
    struct basic_map_bucket *bkt = NULL;
    int err = 0;

    /* Check the pointer args */
    if (map == NULL || key == NULL || val == NULL)
    {
        return EINVAL;
    }

    /* Check the size args. We don't expect args with size zero! */
    if (key_size == 0 || val_size == 0)
    {
        return EINVAL;
    }

    /* Create a new bucket with given key and value */
    if ((err = bmap_bucket_alloc_emplace(&bkt, key, key_size)) != 0)
    {
        return err;
    }

    /* Assign given value pointer and size to bucket */
    bkt->bkt_val = val;
    bkt->bkt_val_size = val_size;

    i_bmap_insert(map, key, key_size, bkt);

    return 0;
}
