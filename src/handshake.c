#include <mbedtls/base64.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/mbedtls_config.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "cws.h"


//////////////////////////////////////////////
//
//        HANDSHAKE
//
//////////////////////////////////////////////

uint8_t urandom(unsigned char *output, size_t output_len)
{
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (unsigned char *)"urandom", 0x7);

    mbedtls_ctr_drbg_random(&ctr_drbg, output, output_len);

    mbedtls_entropy_free(&entropy);

    mbedtls_ctr_drbg_free(&ctr_drbg);

    return (0);
}

uint8_t generate_handshake_key(websocket_handshake_t *handshake)
{
    unsigned char buf[0x10] = {0};
    size_t olen = 0;

    if (urandom(buf, sizeof(buf)))
        return (1);
    
    for (uint16_t i = 0; i < 0x10; i++)
        buf[i] &= 0x7f;
    
    mbedtls_base64_encode(handshake->options.key, 0x100, &olen, buf, 0x10);

    return (0);
}

uint8_t make_handshake_request(websocket_client_t *client)
{
    char address[0x100], path[0x100];

    if (client->url.port)
        goto create_buffer;

    if (IS_WS_PROTOCOL(client->url.protocol))
        client->url.port = "80";
    else if (IS_WSS_PROTOCOL(client->url.protocol))
        client->url.port = "443";
    else
        return (1);
    
    create_buffer:

        if (inet_pton(AF_INET, client->url.addr, address))
            snprintf(address, 0x100, "%s:%s", client->url.addr, client->url.port);
        else
            strncpy(address, client->url.addr, 0x100);
        
        if (client->url.path)
            snprintf(path, 0x100, "/%s", client->url.path);
        else
            strncpy(path, "/", sizeof(path));

        snprintf(
            client->handshake.buffer.buf, 
            0x3FF,
        "GET %s HTTP/1.1\r\nHost: %s\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: %s\r\nSec-WebSocket-Protocol: chat, superchat\r\nSec-WebSocket-Version: 13\r\n%s",
        path, 
        address, 
        client->handshake.options.key, 
        (client->handshake.options.additional_hdr == NULL) ? "\r\n" : client->handshake.options.additional_hdr);

    return (0);
}

int send_handshake_request(websocket_client_t *client)
{
    return (client->write(client, client->handshake.buffer.buf, strlen(client->handshake.buffer.buf)));
}