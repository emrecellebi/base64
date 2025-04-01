#include <stdio.h>
#include <string.h>

#include "base64.h"

int main(int argc, char** argv)
{
	if(argc <= 1)
	{
		printf("b64 : No command specified. Use 'b64  -h or --help' for a detailed command list");
		return 1;
	}
	
	if(argc == 3 && (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--encode") == 0))
	{
		const char* str = argv[2];
		size_t inlen = strlen(str);
		
		char* encode;
		size_t enc_sz = base64_encode_alloc(str, inlen, &encode);
		printf("%s", encode);
		
		return 1;
	}
	
	if(argc == 3 && (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--decode") == 0))
	{
		const char* str = argv[2];
		size_t inlen = strlen(str);
		
		char* decode;
		struct base64_decode_context ctx;
		base64_decode_alloc_ctx(&ctx, str, inlen, &decode, &inlen);
		decode[inlen] = '\0';
		printf("%s", decode);
		
		return 1;
	}
	
	if(argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
	{
		printf("\nCopyright (c) 2012-2025 Emre Celebi\n\n");
		printf("Usage: b64 [-options] <parameters>\n");
		printf("  -d, --decode <str>\n");
		printf("  -e, --encode <str>\n");
		printf("  -v, --version\n");
		printf("  -h, --help\n");
		return 1;
	}
	
	if(argc == 2 && (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version")))
	{
		printf("\nb64 version: 0.0.1 Pre-Alpha\n");
		printf("Last revised: 01th Apr 2025\n");
		printf("The last version is always avaible\n");
		printf("Created By Emre Celebi\n");
		return 1;
	}
	
	return 0;
}