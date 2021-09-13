#pragma once
#include <stdint.h>
#include <mbedtls/ssl.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>

//////////////////////////////////////////////
//
//        DEFINES
//
//////////////////////////////////////////////
#define WS_OPCODE_CONTINUATION_FRAME 0x0
#define WS_OPCODE_TEXT_FRAME 0x1
#define WS_OPCODE_BINARY_FRAME 0x2
#define WS_OPCODE_CONNECTION_CLOSE 0x8
#define WS_OPCODE_PING_FRAME 0x9
#define WS_OPCODE_PONG_FRAME 0xA
#define WS_CLIENT_MODE 0x1
#define WS_SERVER_MODE 0x0


//////////////////////////////////////////////
//
//        TYPEDEF
//
//////////////////////////////////////////////
typedef struct websocket_client_t websocket_client_t;
typedef struct websocket_url_t websocket_url_t;
typedef struct websocket_options_t websocket_options_t;
typedef struct websocket_buffer_t websocket_buffer_t;
typedef struct websocket_error_t websocket_error_t;
typedef struct websocket_handshake_t websocket_handshake_t;
typedef struct websocket_ssl_t websocket_ssl_t;
typedef struct websocket_frame_t websocket_frame_t;
typedef struct websocket_extented_frame_t websocket_extented_frame_t;


//////////////////////////////////////////////
//
//        STRUCT
//
//////////////////////////////////////////////

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
    int (*close)(websocket_client_t *);
    int (*read)(websocket_client_t *, char *, size_t);
    int (*write)(websocket_client_t *, char *, size_t);
    int fd;
    uint16_t err_code;
    uint8_t ssl;
};

struct websocket_extented_frame_t
{
    uint16_t ext_len;
    uint64_t ext__len;
};

struct websocket_frame_t
{
    uint8_t fin : 1;
    uint8_t rsv : 3;
    uint8_t opcode : 4;
    uint8_t mask : 1;
    uint8_t len : 7;
    websocket_extented_frame_t ext;
    uint8_t mask_key[0x4];
    char *payload;
};