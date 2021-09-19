#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "cws.h"

//////////////////////////////////////////////
//
//        Websocket Frames
//
//////////////////////////////////////////////


//////////////////////////////////////////////////////////
//
//                     RFC
//
////////////////////////////////////////////////////////

// Fragmentation : https://datatracker.ietf.org/doc/html/rfc6455#section-5.4
// Control Frames : https://datatracker.ietf.org/doc/html/rfc6455#section-5.5
// Close Frames : https://datatracker.ietf.org/doc/html/rfc6455#section-5.5.1
// Ping Frames : https://datatracker.ietf.org/doc/html/rfc6455#section-5.5.2
// Pong Frames : https://datatracker.ietf.org/doc/html/rfc6455#section-5.5.3
// Data Frames : https://datatracker.ietf.org/doc/html/rfc6455#section-5.6
// Examples : https://datatracker.ietf.org/doc/html/rfc6455#section-5.7


/*
 0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+

*/

// Mask data
char *mask_data(char *buf, uint8_t *key)
{
    char *tmp = buf;

    for (uint8_t i = 0; *tmp; i++)
        *tmp++ ^= key[i % 4];

    return (buf);
}

void generate_mask_key(websocket_frame_t *frame)
{
    uint8_t *tmp = frame->mask_key;
    uint8_t byte = 0, i = 0;

    srandom(time(NULL));

    while (i < 4) {
        byte = (rand() % 0x7F);

        if (!byte)
            continue;
        
        tmp[i++] = byte;
    }
}

void websocket_create_text_frame(websocket_frame_t *frame, char *data, size_t len, uint8_t mode)
{
    frame->fin = 0x1;
    frame->rsv = 0x0;
    frame->opcode = WS_OPCODE_TEXT_FRAME;
    frame->mask = 0;
    frame->payload = data;
    frame->ext.ext_len = 0x0;
    frame->ext.ext__len = 0x0;
    frame->len = len;
    frame->frame_len = 2;

    if (len >= 0x7E && len <= 0xFFFF) {
        frame->len = 0x7E;
        frame->ext.ext_len = len;
        frame->frame_len += 2;
    } 
    
    if (len > 0xFFFF) {
        frame->len = 0x7F;
        frame->ext.ext__len = len;
        frame->frame_len += 8;
    }

    if (mode) {
        frame->mask = 1;
        generate_mask_key(frame);
        frame->payload = mask_data(data, frame->mask_key);
        frame->frame_len += 4;
    }

    frame->frame_len += len;
}

char *websocket_create_buffer(websocket_frame_t *frame, websocket_buffer_t *buf)
{
    char *p_ptr = buf->ptr;

    buf->pos = 0;

    if (frame->frame_len > 0x1000) {
        buf->ptr = (char *)malloc(sizeof(char) * get_next_memory(frame->frame_len));
        p_ptr = buf->ptr;
        buf->alloc = 0x1;
    }

    return (p_ptr);
}

websocket_buffer_t *websocket_format_frame(websocket_frame_t *frame, websocket_buffer_t *buf)
{
    unsigned char *tmp = (unsigned char *)((buf->alloc) ? (buf->ptr) : (buf->buf));

    *tmp++ = (frame->fin << 0x7 | frame->rsv << 0x6 | frame->opcode);
    
    *tmp++ = (frame->mask << 0x7 | frame->len);

    if (frame->ext.ext_len) {
        *tmp++ = (frame->ext.ext_len >> 8);
        *tmp++ = (frame->ext.ext_len & 0xFF);
    }

    if (frame->ext.ext__len) {
        for (uint8_t i = 1; i < 9; i++)
            *tmp++ = (frame->ext.ext__len >> (0x40 - (0x8 * i)) & 0xFF);
    }
    
    if (frame->mask)
        strncat((char *) tmp, (char *)frame->mask_key, 0x4);
    
    strncat((char *) tmp, frame->payload, strlen(frame->payload));

    return (buf);
}

void websocket_delete_buffer(websocket_buffer_t *buf)
{
    if (buf->alloc) {
        free(buf->ptr);
        buf->alloc = 0;
    }
}


websocket_frame_t *websocket_parse_frame(websocket_frame_t *frame, websocket_buffer_t *buf)
{
    char *tmp = buf->alloc ? buf->ptr : buf->buf;

    frame->fin = (*tmp >> 0x7);
    frame->opcode = (*tmp++ & 0xF);
    
    frame->mask = (*tmp >> 0x7);
    
    if (frame->mask)
        return (NULL);
    
    frame->len = (*tmp++ & 0x7F);

    if (!frame->len)
        return (NULL);

    if (frame->len == 0x7E) {
        frame->ext.ext_len = *tmp++;
        frame->ext.ext_len <<= 8;
        frame->ext.ext_len |= *tmp++ & 0xFF;
    }
    
    if (frame->len == 0x7F) {
        for (uint8_t i = 7; i >= 0; i--)
            frame->ext.ext__len |= ((*tmp++ << (8 * i)) & 0xFF);
    }

    frame->payload = tmp;

    return (frame);
}