#include "randombytes.h"

static int fd = -1;

/**
  * \fn void random_bytes(uint8_t* bytes_array, uint32_t nb_bytes)
  * \brief This function generates random bytes

  * \param[out] bytes_array bytes array representing the generated value
  * \param[in] nb_bytes 32 bytes int representing the size in bytes of the generated value
*/
void random_bytes(uint8_t* bytes_array, uint32_t nb_bytes) {

  int i;

  if (fd == -1) {
    for (;;) {
      fd = open("/dev/urandom",O_RDONLY);
      if (fd != -1) break;
      sleep(1);
    }
  }

  while (nb_bytes > 0) {
    if (nb_bytes < 1048576) i = nb_bytes; else i = 1048576;

    i = read(fd, bytes_array, i);
    if (i < 1) {
      sleep(1);
      continue;
    }

    bytes_array += i;
    nb_bytes -= i;
  }
}
