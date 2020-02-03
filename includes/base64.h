#ifndef BASE64_H
#define BASE64_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include <stdint.h>
#include <assert.h>


int base64_encode(const char* buffer, size_t length, char** b64text);
int base64_decode(char* b64message, char** buffer, int* length);
int calc_base64_length(const char* b64input);

#endif // BASE64_H
