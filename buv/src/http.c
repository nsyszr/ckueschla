#include <errno.h>
#include <fcntl.h>
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "http.h"

struct http_method_text
{
    enum http_method method;
    char *method_text;
};

static struct http_method_text method_text[] = {
    {HTTP_METHOD_GET, "GET"},
    {HTTP_METHOD_POST, "POST"},
    {HTTP_METHOD_PUT, "PUT"},
    {HTTP_METHOD_DELETE, "DELETE"},
    {HTTP_METHOD_CONNECT, "CONNECT"},
    {HTTP_METHOD_OPTIONS, "OPTIONS"},
    {HTTP_METHOD_TRACE, "TRACE"},
    {HTTP_METHOD_PATCH, "PATCH"}};

const char *http_method_text(enum http_method method)
{
    for (int i = 0; i < sizeof(method_text) / sizeof(http_method_text); i++)
    {
        if (method == method_text[i].method)
            return method_text[i].method_text;
    }

    return NULL;
}

struct http_status_text
{
    enum http_status status;
    char *status_text;
};

static struct http_status_text status_text[] = {
    {HTTP_STATUS_CONTINUE, "Continue"},
    {HTTP_STATUS_SWITCHING_PROTOCOLS, "Switching Protocols"},
    {HTTP_STATUS_PROCESSING, "Processing"},

    {HTTP_STATUS_OK, "OK"},
    {HTTP_STATUS_CREATED, "Created"},
    {HTTP_STATUS_ACCEPTED, "Accepted"},
    {HTTP_STATUS_NON_AUTHORITATIVE_INFO, "Non-Authoritative Information"},
    {HTTP_STATUS_NO_CONTENT, "No Content"},
    {HTTP_STATUS_RESET_CONTENT, "Reset Content"},
    {HTTP_STATUS_PARTIAL_CONTENT, "Partial Content"},
    {HTTP_STATUS_MULTI_STATUS, "Multi-Status"},
    {HTTP_STATUS_ALREADY_REPORTED, "Already Reported"},
    {HTTP_STATUS_IM_USED, "IM Used"},

    {HTTP_STATUS_MULTIPLE_CHOICES, "Multiple Choices"},
    {HTTP_STATUS_MOVED_PERMANENTLY, "Moved Permanently"},
    {HTTP_STATUS_FOUND, "Found"},
    {HTTP_STATUS_SEE_OTHER, "See Other"},
    {HTTP_STATUS_NOT_MODIFIED, "Not Modified"},
    {HTTP_STATUS_USE_PROXY, "Use Proxy"},
    {HTTP_STATUS_TEMPORARY_REDIRECT, "Temporary Redirect"},
    {HTTP_STATUS_PERMANENT_REDIRECT, "Permanent Redirect"},

    {HTTP_STATUS_BAD_REQUEST, "Bad Request"},
    {HTTP_STATUS_UNAUTHORIZED, "Unauthorized"},
    {HTTP_STATUS_PAYMENT_REQUIRED, "Payment Required"},
    {HTTP_STATUS_FORBIDDEN, "Forbidden"},
    {HTTP_STATUS_NOT_FOUND, "Not Found"},
    {HTTP_STATUS_METHOD_NOT_ALLOWED, "Method Not Allowed"},
    {HTTP_STATUS_NOT_ACCEPTABLE, "Not Acceptable"},
    {HTTP_STATUS_PROXY_AUTH_REQUIRED, "Proxy Authentication Required"},
    {HTTP_STATUS_REQUEST_TIMEOUT, "Request Timeout"},
    {HTTP_STATUS_CONFLICT, "Conflict"},
    {HTTP_STATUS_GONE, "Gone"},
    {HTTP_STATUS_LENGTH_REQUIRED, "Length Required"},
    {HTTP_STATUS_PRECONDITION_FAILED, "Precondition Failed"},
    {HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE, "Request Entity Too Large"},
    {HTTP_STATUS_REQUEST_URI_TOO_LONG, "Request URI Too Long"},
    {HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"},
    {HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE, "Requested Range Not Satisfiable"},
    {HTTP_STATUS_EXPECTATION_FAILED, "Expectation Failed"},
    {HTTP_STATUS_TEAPOT, "I'm a teapot"},
    {HTTP_STATUS_MISDIRECTED_REQUEST, "Misdirected Request"},
    {HTTP_STATUS_UNPROCESSABLE_ENTITY, "Unprocessable Entity"},
    {HTTP_STATUS_LOCKED, "Locked"},
    {HTTP_STATUS_FAILED_DEPENDENCY, "Failed Dependency"},
    {HTTP_STATUS_UPGRADE_REQUIRED, "Upgrade Required"},
    {HTTP_STATUS_PRECONDITION_REQUIRED, "Precondition Required"},
    {HTTP_STATUS_TOO_MANY_REQUESTS, "Too Many Requests"},
    {HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE, "Request Header Fields Too Large"},
    {HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS, "Unavailable For Legal Reasons"},

    {HTTP_STATUS_INTERNAL_SERVER_ERROR, "Internal Server Error"},
    {HTTP_STATUS_NOT_IMPLEMENTED, "Not Implemented"},
    {HTTP_STATUS_BAD_GATEWAY, "Bad Gateway"},
    {HTTP_STATUS_SERVICE_UNAVAILABLE, "Service Unavailable"},
    {HTTP_STATUS_GATEWAY_TIMEOUT, "Gateway Timeout"},
    {HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported"},
    {HTTP_STATUS_VARIANT_ALSO_NEGOTIATES, "Variant Also Negotiates"},
    {HTTP_STATUS_INSUFFICIENT_STORAGE, "Insufficient Storage"},
    {HTTP_STATUS_LOOP_DETECTED, "Loop Detected"},
    {HTTP_STATUS_NOT_EXTENDED, "Not Extended"},
    {HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED, "Network Authentication Required"}};

const char *http_status_text(enum http_status status)
{
    for (int i = 0; i < sizeof(status_text) / sizeof(http_status_text); i++)
    {
        if (status == status_text[i].status)
            return status_text[i].status_text;
    }

    return NULL;
}

struct http_response_writer
{
    char *buf;
    size_t buf_size;
    enum http_status status;
};

int http_response_writer_write(http_response_writer_t *w, const char *buf,
                               size_t *buf_size)
{
    http_response_writer_write_header(w, HTTP_STATUS_OK);

    return (0);
}

int http_response_writer_write_header(http_response_writer_t *w,
                                      enum http_status status)
{
    w->status = status;

    return (0);
}

struct http_serve_mux
{
};

int http_serve_mux_handle_func(const char *pattern, http_handle_func_t handler)
{
    return (0);
}

struct http_server
{
};

int http_server_listen_and_serve(http_server_t *server, http_serve_mux_t *mux)
{
    return (0);
}