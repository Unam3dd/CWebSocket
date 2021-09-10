#include <stddef.h>
#include "cws.h"

//////////////////////////////////////////////
//
//        ERROR
//
//////////////////////////////////////////////

const websocket_error_t errors[]  = {
    { "Normal Closure", 0x3E8},
    { "Going Away",     0x3E9},
    { "Protocol Error", 0x3EA},
    { "Unsupported Data", 0x3EB},
    { "Reserved",        0x3EC},
    { "No Status Rcvd",  0x3ED},
    { "Abnormal Closure", 0x3EE},
    { "Invalid frame payload data", 0x3EF},
    { "Policy Violation", 0x3F0},
    { "Message Too Big", 0x3F1},
    { "Mandatory Ext", 0x3F2},
    { "Internal Server Error", 0x3F3},
    { "TLS Handshake", 0x3F4},
    { "Error connect to remote host", 0x3F5},
    { "Error init secure connection", 0x3F6},
    { 0 },
};

char *get_error(uint16_t err)
{
    for (uint16_t i = 0; errors[i].err_code; i++)
        if (errors[i].err_code == err)
            return (errors[i].message);
    
    return (NULL);
}