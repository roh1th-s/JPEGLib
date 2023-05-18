#include "Util.hpp"

#ifdef _WIN32
#include <winsock2.h> // htons
#else
#include <arpa/inet.h>
#endif

uint16_t swap_bytes(uint16_t bytes) {
    return (uint16_t) htons(bytes);
}