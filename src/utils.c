#include "cws.h"


//////////////////////////////////////////////
//
//        UTILS
//
//////////////////////////////////////////////

size_t get_next_memory(size_t size)
{
    size_t i = 2;

    while (i < size)
        i <<= 1;
    
    return (i);
}