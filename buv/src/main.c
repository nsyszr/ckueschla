#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <microhttpd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <zlib.h>

#include "basic_map.h"

#define PORT 8888

struct route
{
    char *rt_url;
    MHD_AccessHandlerCallback rt_handler;
};

static int
index_handler(void *cls, struct MHD_Connection *connection, const char *url,
              const char *method, const char *version, const char *upload_data,
              size_t *upload_data_size, void **ptr)
{
    int ret = MHD_NO;
    struct MHD_Response *response = NULL;
    int fd = 0;
    struct stat fd_stat = {};

    if (strcmp(method, MHD_HTTP_METHOD_GET) != 0)
    {
        return MHD_NO; /* Unexpected METHOD */
    }

    /* load index file */
    /* TODO: add error handling */
    fd = open("web/buv/index.html", O_RDONLY | O_NONBLOCK);

    /* get size of index file */
    if (fstat(fd, &fd_stat) == -1)
    {
        fprintf(stderr, "fstat(%d) returned errno=%d.\n", fd, errno);
        close(fd);
        return MHD_NO;
    }

    response = MHD_create_response_from_fd(fd_stat.st_size, fd);
    if (response == NULL)
    {
        return MHD_NO;
    }

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

static int
static_files_handler(void *cls, struct MHD_Connection *connection,
                     const char *url, const char *method, const char *version,
                     const char *upload_data, size_t *upload_data_size,
                     void **ptr)
{
    int err = MHD_NO;
    struct MHD_Response *response = NULL;
    char filename[2048] = {};
    int fd = 0;
    struct stat fd_stat = {};

    if (strcmp(method, MHD_HTTP_METHOD_GET) != 0)
    {
        return (MHD_NO); /* unexpected method */
    }

    if (sprintf(filename, "web/buv/%s", &url[1]) < 0)
    {
        fprintf(stderr, "failed to compose file name\n");
        return (MHD_NO);
    }

    /* load index file */
    if ((fd = open(filename, O_RDONLY | O_NONBLOCK)) == -1)
    {
        /* file not found */
        response = MHD_create_response_from_buffer(0, NULL,
                                                   MHD_RESPMEM_PERSISTENT);
        err = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    }
    else
    {
        /* get size of index file */
        if (fstat(fd, &fd_stat) == -1)
        {
            fprintf(stderr, "fstat(%d) returned errno=%d.\n", fd, errno);
            goto out_free_buffer;
        }

        response = MHD_create_response_from_fd(fd_stat.st_size, fd);
        if (response == NULL)
        {
            fprintf(stderr, "failed to create response object\n");
            goto out_free_buffer;
        }

        err = MHD_queue_response(connection, MHD_HTTP_OK, response);
    }

    MHD_destroy_response(response);
    return (err);

out_free_buffer:
    /* on error close the file descriptor */
    close(fd);
    return (MHD_NO);
}

static int
api_handler(void *cls, struct MHD_Connection *connection, const char *url,
            const char *method, const char *version, const char *upload_data,
            size_t *upload_data_size, void **con_cls)
{
    const char *page = "<html><body>API</body></html>";
    struct MHD_Response *response;
    int ret;

    printf("URL: %s\n", &url[1]);

    response = MHD_create_response_from_buffer(strlen(page),
                                               (void *)page, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

static int
routes_handler(void *cls, struct MHD_Connection *connection, const char *url,
               const char *method, const char *version, const char *upload_data,
               size_t *upload_data_size, void **ptr)
{
    static int aptr;
    int ret;
    MHD_AccessHandlerCallback handler = NULL;
    struct MHD_Response *response;

    const struct route routes[] = {
        {.rt_url = "/", .rt_handler = index_handler},
        {.rt_url = "/api", .rt_handler = api_handler}};

    if (&aptr != *ptr)
    {
        /* do never respond on first call */
        *ptr = &aptr;
        return MHD_YES;
    }
    *ptr = NULL; /* reset when done */

    for (int i = 0; i < sizeof(routes) / sizeof(struct route); i++)
    {
        if (strcmp(routes[i].rt_url, url) == 0)
        {
            handler = routes[i].rt_handler;
            break;
        }
    }

    /* run handler if exists */
    if (handler != NULL)
    {
        return handler(cls, connection, url, method, version, upload_data,
                       upload_data_size, ptr);
    }

    /* if no handler exists then static file handling */
    printf("URL: %s\n", &url[1]);

    return static_files_handler(cls, connection, url, method, version,
                                upload_data, upload_data_size, ptr);
}

int main()
{
#ifdef DEBUG
    printf("Debug mode enabled\n");
#endif /* DEBUG */

    /*struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &routes_handler, NULL, MHD_OPTION_END);
    if (NULL == daemon)
        return 1;
    getchar();

    MHD_stop_daemon(daemon);
    return 0;*/

    char val[11] = {0};
    char *rval = NULL;
    int vali = 0;
    int rvali = 0;
    size_t rval_size = 0;
    int key = 0;

    /*char val1[] = "Djordje";
    char val2[] = "Daniela";
    char val3[] = "Lina";

    long key1 = 100;
    long key2 = 200;
    long key3 = 300;*/

    /*size_t array_size = 256;

    uint32_t crc1 = {0};
    size_t key1_size = sizeof(key1);
    crc1 = crc32(&key1, key1_size);
    printf("CRC1=%u, INDEX=%lu KEY_SIZE=%lu\n", crc1, crc1 % array_size, key1_size);

    uint32_t crc2 = {0};
    size_t key2_size = sizeof(key2);
    crc2 = crc32(&key2, key2_size);
    printf("CRC2=%u, INDEX=%lu KEY_SIZE=%lu\n", crc2, crc2 % array_size, key2_size);

    uint32_t crc3 = {0};
    size_t key3_size = sizeof(key3);
    crc3 = crc32(&key3, key3_size);
    printf("CRC3=%u, INDEX=%lu KEY_SIZE=%lu\n", crc3, crc3 % array_size, key3_size);*/

    /*basic_map_t *mymap = NULL;
    basic_map_alloc(&mymap);
    basic_map_insert(mymap, &key1, sizeof(key1), val1, sizeof(val1));
    basic_map_insert(mymap, &key2, sizeof(key2), val2, sizeof(val2));
    basic_map_insert(mymap, &key3, sizeof(key3), val3, sizeof(val3));

    char *val1r, *val2r, *val3r;
    size_t val1sz, val2sz, val3sz;
    val1r = val2r = val3r = NULL;
    val1sz = val2sz = val3sz = 0;

    basic_map_at(mymap, &key1, sizeof(key1), (void **)&val1r, &val1sz);
    basic_map_at(mymap, &key2, sizeof(key2), (void **)&val2r, &val2sz);
    basic_map_at(mymap, &key3, sizeof(key3), (void **)&val3r, &val3sz);
    printf("val1=%s, val2=%s, val3=%s\n", val1r, val2r, val3r);

    val1r = "xxxxxxxxxxxxxxxxx456dftgddfg";
    basic_map_at(mymap, &key1, sizeof(key1), (void **)&val1r, &val1sz);
    printf("val1=%s, val2=%s, val3=%s\n", val1r, val2r, val3r);

    char *trueval1r = NULL;
    basic_map_at(mymap, &key1, sizeof(key1), (void **)&trueval1r, &val1sz);
    printf("val1=%s, val2=%s, val3=%s\n", trueval1r, val2r, val3r);

    basic_map_free(mymap);*/

    basic_map_t *mymap = NULL;
    basic_map_alloc(&mymap);

    for (key = 0; key < 500; key++)
    {
        sprintf(val, "%010d", key);
        basic_map_insert(mymap, &key, sizeof(key), val, sizeof(val));
    }

    for (key = 0; key < 500; key++)
    {
        basic_map_at(mymap, &key, sizeof(key), (void **)&rval, &rval_size);
        // printf("key=%d, val=%s\n", key, rval);
    }

    /*int lookup = 100;
    basic_map_contains(mymap, &lookup, sizeof(lookup));*/
    printf("Map size is %lu\n", basic_map_size(mymap));

    basic_map_free(mymap);

#ifdef DEBUG
    bmap_debug_print_memory_leaks();
#endif /* DEBUG */

    return (0);
}