#pragma once
#include <stdlib.h>

/*

Allocator to not have to gain time and not use malloc/free each time

*/

void A_Init(const size_t size);


void * M_Allocate(const size_t size);

