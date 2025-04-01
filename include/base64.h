#ifndef BASE64_H
#define BASE64_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BASE64_LENGTH(inlen) ((((inlen) + 2) / 3) * 4)

struct base64_decode_context
{
	unsigned int i;
	char buf[4];
};

extern void base64_encode(const char* in, size_t inlen, char* out, size_t outlen);
extern void base64_decode_ctx_init(struct base64_decode_context* ctx);
extern bool base64_decode_ctx(struct base64_decode_context* ctx, const char* in, size_t inlen, char* out, size_t* outlen);
extern bool base64_decode_alloc_ctx(struct base64_decode_context* ctx, const char* in, size_t inlen, char** out, size_t* outlen);
extern size_t base64_encode_alloc(const char* in, size_t inlen, char** out);

#ifdef __cplusplus
}
#endif

#endif /* BASE64_H */