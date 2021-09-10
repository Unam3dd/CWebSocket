#pragma once
#include <stddef.h>
#include <stdint.h>
#include "cws_types.h"

//////////////////////////////////////////////
//
//              MACRO
//
//////////////////////////////////////////////
#define IS_WS_PROTOCOL(str) (str[0] == 'w' && str[1] == 's')
#define IS_WSS_PROTOCOL(str) (str[0] == 'w' && str[1] == 's' && str[2] == 's')


//////////////////////////////////////////////
//
//        URL PARSER
//
//////////////////////////////////////////////

// parse.c
uint16_t parse_url(websocket_client_t *client, const char *url);
char *parse_protocol(websocket_buffer_t *buffer);
char *parse_host(websocket_buffer_t *buffer);
char *parse_port(websocket_buffer_t *buffer);
char *parse_path(websocket_buffer_t *buffer);
char *parse_query(websocket_buffer_t *buffer);


//////////////////////////////////////////////
//
//        ERROR
//
//////////////////////////////////////////////

// error.c
extern const websocket_error_t errors[];
char *get_error(uint16_t err);


//////////////////////////////////////////////
//
//        CLIENT
//
//////////////////////////////////////////////

// client.c
void set_client_callbacks(websocket_client_t *client);
uint8_t websocket_connect(websocket_client_t *client);
uint8_t websocket_client_connect(websocket_client_t *client);

//////////////////////////////////////////////
//
//              SSL
//
//////////////////////////////////////////////

// ssl.c
uint8_t initialize_ssl(websocket_client_t *client);
uint8_t initialize_ssl_context(websocket_client_t *client);
void initialize_ssl_config(websocket_client_t *client);
void free_ssl_context(websocket_client_t *client);


//////////////////////////////////////////////
//
//        HANDSHAKE
//
//////////////////////////////////////////////

// handshake.c
uint8_t urandom(unsigned char *output, size_t output_len);
uint8_t generate_handshake_key(websocket_handshake_t *handshake);
uint8_t make_handshake_request(websocket_client_t *client);
int send_handshake_request(websocket_client_t *client);


//////////////////////////////////////////////
//
//        R/W Functions
//
//////////////////////////////////////////////

// rw.c
int websocket_client_read(websocket_client_t *client, char *buf, size_t len);
int websocket_client_write(websocket_client_t *client, char *buf, size_t len);