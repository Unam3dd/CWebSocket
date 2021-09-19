#include "cws.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    uint32_t i = 0;
    websocket_client_t client;
    websocket_frame_t frame;
    websocket_buffer_t ws_buffer, recv_buffer;
    
    unsigned char *buf = (char *)malloc(sizeof(char) * 0xFFFFF);
    unsigned char data[0x100] = {0};

    memset(buf, 0, sizeof(buf));

    for (i = 0; i < (0xFFFFF - 1); i++)
        buf[i] = 'a';
    
    buf[i] = 0;

    set_client_callbacks(&client);

    client.handshake.options.additional_hdr = NULL;

    if (parse_url(&client, argv[1]))
        goto err;
    
    client.handshake.generate_handshake_key(&client.handshake);
    
    make_handshake_request(&client);

    if (client.connect(&client))
        goto err;
    
    printf("%s\n", client.handshake.buffer.buf);

    client.handshake.send_handshake(&client);

    client.read(&client, data, sizeof(data));

    printf("%s\n", data);

    client.read(&client, recv_buffer.buf, sizeof(recv_buffer.buf));

    websocket_parse_frame(&frame, &recv_buffer);

    printf("%d\n", frame.len);


    websocket_create_text_frame(&frame, buf, strlen(buf), WS_CLIENT_MODE);

    char *ptr_buf = websocket_create_buffer(&frame, &ws_buffer);

    websocket_format_frame(&frame, &ws_buffer);

    client.write(&client, ptr_buf, frame.frame_len);

    websocket_delete_buffer(&ws_buffer);

    free(buf);

    sleep(1);

    client.close(&client);

    return (0);

    err:
        fprintf(stderr, "[-] Error : %s\n", client.get_error(client.err_code));
        return (1);
}