#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
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

    srandom(time(NULL));

    for (uint8_t i = 0; i < 4; i++)
        tmp[i] = rand() % 0x7F;
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

    if (len >= 0x7E && len <= 0xFFFF) {
        frame->len = 0x7E;
        frame->ext.ext_len = len & 0xFFFF;
    } 
    
    if (len > 0xFFFF) {
        frame->len = 0x7E;
        frame->ext.ext__len = len;
    }

    if (mode) {
        frame->mask = 1;
        generate_mask_key(frame);
        frame->payload = mask_data(data, frame->mask_key);
    }
}

unsigned char *websocket_format_frame(websocket_frame_t *frame, unsigned char *buf, size_t buf_size)
{
    size_t size = (frame->len + frame->ext.ext_len + frame->ext.ext__len);

    if (buf_size <= size)
        return (NULL);

    buf[0] = (frame->fin << 0x7 | frame->rsv << 0x6 | frame->opcode);
    
    buf[1] = (frame->mask << 0x7 | frame->len);

    if (frame->ext.ext_len)
        buf[2] = (frame->ext.ext_len);

    if (frame->ext.ext__len)
        buf[2] = (frame->ext.ext__len);
    
    if (frame->mask)
        strncat((char *)buf, (char *)frame->mask_key, 0x4);
    
    strncat((char *)buf, frame->payload, size);

    return (buf);
}