#include "pa_alloc.h"


struct run * freepages;


// astart must be less then astop
bool zero_range(uint64_t * astart, uint64_t * astop){

    if(astart >= astop) return false;

    for(uint64_t * ptr = astart; ptr < astop; ++ptr){
        *ptr = 0;
    }

    return true;
}