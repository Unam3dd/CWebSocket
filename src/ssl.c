#include "cws.h"
#include <mbedtls/net_sockets.h>

//////////////////////////////////////////////
//
//              SSL
//
//////////////////////////////////////////////

uint8_t initialize_ssl(websocket_client_t *client)
{
    initialize_ssl_config(client);

    if (initialize_ssl_context(client))
        return (1);
    
    return (0);
}

uint8_t initialize_ssl_context(websocket_client_t *client)
{
    mbedtls_ssl_init(&client->secure.ssl_ctx);

    mbedtls_ssl_setup(&client->secure.ssl_ctx, &client->secure.ssl_config);

    mbedtls_ssl_set_bio(&client->secure.ssl_ctx, &client->fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    if (mbedtls_ssl_handshake(&client->secure.ssl_ctx))
        return (1);
    
    return (0);
}

void initialize_ssl_config(websocket_client_t *client)
{
    mbedtls_entropy_init(&client->secure.entropy);
    
    mbedtls_ctr_drbg_init(&client->secure.ctr_drbg);

    mbedtls_ctr_drbg_seed(&client->secure.ctr_drbg, mbedtls_entropy_func, &client->secure.entropy, (unsigned char *)"ssl_client", 10);

    mbedtls_ssl_config_init(&client->secure.ssl_config);
    
    mbedtls_ssl_config_defaults(&client->secure.ssl_config, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);

    mbedtls_ssl_conf_authmode(&client->secure.ssl_config, MBEDTLS_SSL_VERIFY_OPTIONAL);

    mbedtls_ssl_conf_rng(&client->secure.ssl_config, mbedtls_ctr_drbg_random, &client->secure.ctr_drbg);
}

void free_ssl_context(websocket_client_t *client)
{
    mbedtls_ssl_free(&client->secure.ssl_ctx);
    
    mbedtls_ssl_config_free(&client->secure.ssl_config);
    
    mbedtls_entropy_free(&client->secure.entropy);
    
    mbedtls_ctr_drbg_free(&client->secure.ctr_drbg);
}