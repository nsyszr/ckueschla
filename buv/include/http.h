#ifndef BUV_HTTP_
#define BUV_HTTP_

#ifdef __cplusplus
extern "C"
{
#endif
    /* HTTP Methods */
    enum http_method
    {
        HTTP_METHOD_GET,
        HTTP_METHOD_HEAD,
        HTTP_METHOD_POST,
        HTTP_METHOD_PUT,
        HTTP_METHOD_DELETE,
        HTTP_METHOD_CONNECT,
        HTTP_METHOD_OPTIONS,
        HTTP_METHOD_TRACE,
        HTTP_METHOD_PATCH
    };

    const char *http_method_text(enum http_method method);

    /* HTTP Status Codes */
    enum http_status
    {
        HTTP_STATUS_CONTINUE = 100,            /* RFC 7231, 6.2.1 */
        HTTP_STATUS_SWITCHING_PROTOCOLS = 101, /* RFC 7231, 6.2.2 */
        HTTP_STATUS_PROCESSING = 102,          /* RFC 2518, 10.1 */

        HTTP_STATUS_OK = 200,                     /* RFC 7231, 6.3.1 */
        HTTP_STATUS_CREATED = 201,                /* RFC 7231, 6.3.2 */
        HTTP_STATUS_ACCEPTED = 202,               /* RFC 7231, 6.3.3 */
        HTTP_STATUS_NON_AUTHORITATIVE_INFO = 203, /* RFC 7231, 6.3.4 */
        HTTP_STATUS_NO_CONTENT = 204,             /* RFC 7231, 6.3.5 */
        HTTP_STATUS_RESET_CONTENT = 205,          /* RFC 7231, 6.3.6 */
        HTTP_STATUS_PARTIAL_CONTENT = 206,        /* RFC 7233, 4.1 */
        HTTP_STATUS_MULTI_STATUS = 207,           /* RFC 4918, 11.1 */
        HTTP_STATUS_ALREADY_REPORTED = 208,       /* RFC 5842, 7.1 */
        HTTP_STATUS_IM_USED = 226,                /* RFC 3229, 10.4.1 */

        HTTP_STATUS_MULTIPLE_CHOICES = 300,   /* RFC 7231, 6.4.1 */
        HTTP_STATUS_MOVED_PERMANENTLY = 301,  /* RFC 7231, 6.4.2 */
        HTTP_STATUS_FOUND = 302,              /* RFC 7231, 6.4.3 */
        HTTP_STATUS_SEE_OTHER = 303,          /* RFC 7231, 6.4.4 */
        HTTP_STATUS_NOT_MODIFIED = 304,       /* RFC 7232, 4.1 */
        HTTP_STATUS_USE_PROXY = 305,          /* RFC 7231, 6.4.5 */
        HTTP_STATUS_TEMPORARY_REDIRECT = 307, /* RFC 7231, 6.4.7 */
        HTTP_STATUS_PERMANENT_REDIRECT = 308, /* RFC 7538, 3 */

        HTTP_STATUS_BAD_REQUEST = 400,                     /* RFC 7231, 6.5.1 */
        HTTP_STATUS_UNAUTHORIZED = 401,                    /* RFC 7235, 3.1 */
        HTTP_STATUS_PAYMENT_REQUIRED = 402,                /* RFC 7231, 6.5.2 */
        HTTP_STATUS_FORBIDDEN = 403,                       /* RFC 7231, 6.5.3 */
        HTTP_STATUS_NOT_FOUND = 404,                       /* RFC 7231, 6.5.4 */
        HTTP_STATUS_METHOD_NOT_ALLOWED = 405,              /* RFC 7231, 6.5.5 */
        HTTP_STATUS_NOT_ACCEPTABLE = 406,                  /* RFC 7231, 6.5.6 */
        HTTP_STATUS_PROXY_AUTH_REQUIRED = 407,             /* RFC 7235, 3.2 */
        HTTP_STATUS_REQUEST_TIMEOUT = 408,                 /* RFC 7231, 6.5.7 */
        HTTP_STATUS_CONFLICT = 409,                        /* RFC 7231, 6.5.8 */
        HTTP_STATUS_GONE = 410,                            /* RFC 7231, 6.5.9 */
        HTTP_STATUS_LENGTH_REQUIRED = 411,                 /* RFC 7231, 6.5.10 */
        HTTP_STATUS_PRECONDITION_FAILED = 412,             /* RFC 7232, 4.2 */
        HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE = 413,        /* RFC 7231, 6.5.11 */
        HTTP_STATUS_REQUEST_URI_TOO_LONG = 414,            /* RFC 7231, 6.5.12 */
        HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE = 415,          /* RFC 7231, 6.5.13 */
        HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE = 416, /* RFC 7233, 4.4 */
        HTTP_STATUS_EXPECTATION_FAILED = 417,              /* RFC 7231, 6.5.14 */
        HTTP_STATUS_TEAPOT = 418,                          /* RFC 7168, 2.3.3 */
        HTTP_STATUS_MISDIRECTED_REQUEST = 421,             /* RFC 7540, 9.1.2 */
        HTTP_STATUS_UNPROCESSABLE_ENTITY = 422,            /* RFC 4918, 11.2 */
        HTTP_STATUS_LOCKED = 423,                          /* RFC 4918, 11.3 */
        HTTP_STATUS_FAILED_DEPENDENCY = 424,               /* RFC 4918, 11.4 */
        HTTP_STATUS_UPGRADE_REQUIRED = 426,                /* RFC 7231, 6.5.15 */
        HTTP_STATUS_PRECONDITION_REQUIRED = 428,           /* RFC 6585, 3 */
        HTTP_STATUS_TOO_MANY_REQUESTS = 429,               /* RFC 6585, 4 */
        HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE = 431, /* RFC 6585, 5 */
        HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS = 451,   /* RFC 7725, 3 */

        HTTP_STATUS_INTERNAL_SERVER_ERROR = 500,          /* RFC 7231, 6.6.1 */
        HTTP_STATUS_NOT_IMPLEMENTED = 501,                /* RFC 7231, 6.6.2 */
        HTTP_STATUS_BAD_GATEWAY = 502,                    /* RFC 7231, 6.6.3 */
        HTTP_STATUS_SERVICE_UNAVAILABLE = 503,            /* RFC 7231, 6.6.4 */
        HTTP_STATUS_GATEWAY_TIMEOUT = 504,                /* RFC 7231, 6.6.5 */
        HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED = 505,     /* RFC 7231, 6.6.6 */
        HTTP_STATUS_VARIANT_ALSO_NEGOTIATES = 506,        /* RFC 2295, 8.1 */
        HTTP_STATUS_INSUFFICIENT_STORAGE = 507,           /* RFC 4918, 11.5 */
        HTTP_STATUS_LOOP_DETECTED = 508,                  /* RFC 5842, 7.2 */
        HTTP_STATUS_NOT_EXTENDED = 510,                   /* RFC 2774, 7 */
        HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511 /* RFC 6585, 6 */
    };

    const char *http_status_text(enum http_status status);

    /* HTTP Request */
    struct http_request
    {
        enum http_method method;
        char *url;
        int proto_major;
        int proto_minor;
        char *remote_addr;
        char *request_uri;
    };

    /* HTTP Response Writer */
    typedef struct http_response_writer http_response_writer_t;
    int http_response_writer_write(http_response_writer_t *w, const char *buf,
                                   size_t *buf_size);
    int http_response_writer_write_header(http_response_writer_t *w,
                                          enum http_status status);

    /* HTTP ServeMux */
    typedef struct http_serve_mux http_serve_mux_t;
    typedef int (*http_handle_func_t)(http_response_writer_t *w,
                                      const struct http_request *r);

    int http_serve_mux_handle_func(const char *pattern, http_handle_func_t handler);

    /* HTTP Server */
    typedef struct http_server http_server_t;

    int http_server_listen_and_serve(http_server_t *server, http_serve_mux_t *mux);

#ifdef __cplusplus
}
#endif

#endif /* BUV_HTTP_     */