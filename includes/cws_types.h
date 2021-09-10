#pragma once
#include <stdint.h>
#include <mbedtls/ssl.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>


#define WS_RFC_STD_VERSION 13

//////////////////////////////////////////////
//
//        TYPEDEF
//
//////////////////////////////////////////////
typedef struct websocket_client_t websocket_client_t;
typedef struct websocket_server_t websocket_server_t;
typedef struct websocket_frame_t websocket_frame_t;
typedef struct websocket_url_t websocket_url_t;
typedef struct websocket_options_t websocket_options_t;
typedef struct websocket_buffer_t websocket_buffer_t;
typedef struct websocket_error_t websocket_error_t;
typedef struct websocket_handshake_t websocket_handshake_t;
typedef struct websocket_ssl_t websocket_ssl_t;

struct websocket_error_t
{
    char *message;
    uint16_t err_code;
};

struct websocket_buffer_t
{
    char buf[0x400];
    int pos;
};

struct websocket_ssl_t
{
    mbedtls_ssl_context ssl_ctx;
    mbedtls_ssl_config ssl_config;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    uint8_t (*init)(websocket_client_t *);
    void (*free)(websocket_client_t *);
};

struct websocket_options_t
{
    unsigned char key[0x100];
    char *additional_hdr;
};

struct websocket_url_t
{
    char *protocol;
    char *addr;
    char *port;
    char *path;
    char *query;
};

struct websocket_handshake_t
{
    websocket_buffer_t buffer;
    websocket_options_t options;
    uint8_t (*generate_handshake_key)(websocket_handshake_t *);
    int (*send_handshake)(websocket_client_t *);
};

struct websocket_client_t
{
    websocket_buffer_t buffer;
    websocket_url_t url;
    websocket_handshake_t handshake;
    websocket_ssl_t secure;
    char * (*get_error)(uint16_t);
    uint8_t (*connect)(websocket_client_t *);
    int (*read)(websocket_client_t *, char *, size_t);
    int (*write)(websocket_client_t *, char *, size_t);
    int fd;
    uint16_t err_code;
    uint8_t ssl;
};

struct websocket_server_t
{

};

struct websocket_frame_t
{
    
};