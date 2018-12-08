
#ifndef RANDOMBYTES_H
#define RANDOMBYTES_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

void random_bytes(uint8_t* bytes_array, uint32_t nb_bytes);

#endif