#include <stddef.h>
#include <string.h>
#include "cws.h"

//////////////////////////////////////////////
//
//        URL PARSER
//
//////////////////////////////////////////////

uint16_t parse_url(websocket_client_t *client, const char *url)
{
    if (!url)
        return (1);
    
    strncpy(client->buffer.buf, url, 0x3FF);

    client->buffer.pos = 0;

    client->url.protocol = parse_protocol(&client->buffer);

    if (!IS_WS_PROTOCOL(client->url.protocol) && !IS_WSS_PROTOCOL(client->url.protocol)) {
        client->err_code = 0x3EA;
        return (0x3EA);
    }

    if (IS_WSS_PROTOCOL(client->url.protocol))
        client->ssl = 0x1;

    client->url.addr = parse_host(&client->buffer);
    client->url.port = parse_port(&client->buffer);
    client->url.path = parse_path(&client->buffer);
    client->url.query = parse_query(&client->buffer);

    return (0);
}

char *parse_protocol(websocket_buffer_t *buffer)
{
    char *tmp = (char *)(buffer->buf + buffer->pos);
    char *search = strchr(tmp, ':');

    if (!search)
        return (NULL);
    
    *search++ = 0;
    buffer->pos += (search - tmp);
    
    return (tmp);
}

char *parse_host(websocket_buffer_t *buffer)
{
    char *tmp = (char *)(buffer->buf + buffer->pos);

    if (*tmp == '/' && *(tmp + 1) == '/')
        tmp+= 2;
    
    char *search = strchr(tmp, ':');

    if (!search) {
        search = tmp;

        while (*search && *search != '/')
            search++;   
    }
    
    *search++ = 0;

    buffer->pos += (search - (tmp - 2));
    
    return (tmp);
}

char *parse_port(websocket_buffer_t *buffer)
{
    char *buf = (char *)(buffer->buf + buffer->pos);
    char *tmp = buf;

    while (*tmp && *tmp != '/')
        tmp++;
    
    *tmp++ = 0;
    buffer->pos += (tmp - buf);

    return (buf);
}

char *parse_path(websocket_buffer_t *buffer)
{
    char *buf = (char *)(buffer->buf + buffer->pos);
    char *tmp = buf;

    while (*tmp && *tmp != '?')
        tmp++;
    
    *tmp++ = 0;
    buffer->pos += (tmp - buf);

    return (buf);
}

char *parse_query(websocket_buffer_t *buffer)
{
    char *buf = (char *)(buffer->buf + buffer->pos);
    char *tmp = buf;

    while (*tmp)
        tmp++;
    
    if (*tmp == 0)
        return (NULL);
    
    *tmp++ = 0;
    buffer->pos += (tmp - buf);

    return (buf);
}