#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include "cws.h"


//////////////////////////////////////////////
//
//        CLIENT
//
//////////////////////////////////////////////

int websocket_client_close(websocket_client_t *client)
{
    if (client->ssl)
        client->secure.free(client);

    return (close(client->fd));
}

void set_client_callbacks(websocket_client_t *client)
{
    client->ssl = 0x0;
    client->get_error = &get_error;
    client->connect = &websocket_client_connect;
    client->handshake.generate_handshake_key = &generate_handshake_key;
    client->handshake.send_handshake = &send_handshake_request;
    client->secure.init = &initialize_ssl;
    client->secure.free = &free_ssl_context;
    client->close = &websocket_client_close;
}

uint8_t websocket_connect(websocket_client_t *client)
{
    struct sockaddr_in socks = {0};

    socks.sin_addr.s_addr = inet_addr(client->url.addr);
    socks.sin_port = htons((uint16_t)atoi(client->url.port));
    socks.sin_family = AF_INET;

    if ((client->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        return (1);
    
    if (connect(client->fd, (struct sockaddr *)&socks, sizeof(socks)) < 0) {
        close(client->fd);
        client->err_code = 0x3F5;
        return (1);
    }

    return (0);
}

uint8_t websocket_client_connect(websocket_client_t *client)
{
    if (websocket_connect(client))
        return (1);
    
    // Initialize SSL Context
    if (client->ssl) {
        if (client->secure.init(client)) {
            close(client->fd);
            client->err_code = 0x3F6;
            return (1);
        }
    }

    client->read = &websocket_client_read;
    client->write = &websocket_client_write;

    return (0);
}