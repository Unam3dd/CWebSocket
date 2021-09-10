#include "cws.h"
#include <stdio.h>
#include <stddef.h>

int main(int argc, char **argv)
{
    websocket_client_t client;
    char response[0x1000];

    set_client_callbacks(&client);

    client.handshake.options.additional_hdr = NULL;

    if (parse_url(&client, argv[1]))
        goto err;
    
    client.handshake.generate_handshake_key(&client.handshake);
    
    make_handshake_request(&client);

    if (client.connect(&client))
        goto err;
    
    printf("%s\n", client.handshake.buffer.buf);

    send_handshake_request(&client);

    client.read(&client, response, sizeof(response));

    printf("%s\n", response);

    return (0);

    err:
        fprintf(stderr, "[-] Error : %s\n", client.get_error(client.err_code));
        return (1);
}