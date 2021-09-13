#include "cws.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    websocket_client_t client;
    websocket_frame_t frame;
    unsigned char response[0x1000];

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
    
    client.read(&client, response, sizeof(response));

    printf("%s\n", response);

    memset(response, 0, sizeof(response));

    char buf[] = "hello world";

    websocket_create_text_frame(&frame, buf, strlen(buf), WS_CLIENT_MODE);

    websocket_format_frame(&frame, response, sizeof(response));

    for (uint16_t i = 0; i < strlen(response); i++)
        printf("%x ", response[i]);

    client.write(&client, response, strlen(response));

    sleep(1);

    client.close(&client);

    return (0);

    err:
        fprintf(stderr, "[-] Error : %s\n", client.get_error(client.err_code));
        return (1);
}