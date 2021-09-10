#include "cws.h"
#include <unistd.h>

//////////////////////////////////////////////
//
//        R/W Functions
//
//////////////////////////////////////////////

int websocket_client_read(websocket_client_t *client, char *buf, size_t len)
{
    return (client->ssl 
    ? mbedtls_ssl_read(&client->secure.ssl_ctx, (unsigned char *)buf, len) 
    : read(client->fd, buf, len) 
    );
}

int websocket_client_write(websocket_client_t *client, char *buf, size_t len)
{
    return (client->ssl 
    ? mbedtls_ssl_write(&client->secure.ssl_ctx, (unsigned char *)buf, len) 
    : write(client->fd, buf, len) 
    );
}