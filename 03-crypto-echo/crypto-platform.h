#ifndef CRYPTO_PLATFORM_H
#define CRYPTO_PLATFORM_H

// macos declares a different encrypt function, so rename that declaration only.
#ifdef __APPLE__
#define encrypt system_encrypt_declaration
#include <unistd.h>
#undef encrypt
#endif

#endif
