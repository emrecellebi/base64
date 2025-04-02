#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"

#define ENC_BLOCKSIZE (1024*3*10)
#define DEC_BLOCKSIZE (1024*3)

static void do_encode(FILE* in, FILE* out);
static void do_decode(FILE* in, FILE* out);

int main(int argc, char** argv)
{
	if(argc <= 1)
	{
		printf("b64 : No command specified. Use 'b64 -h or --help' for a detailed command list");
		return 1;
	}
	
	if(argc == 3 && (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--encode") == 0))
	{
		FILE* in;
		const char* data = argv[2];
		if((in = fopen(data, "rb")) != NULL)
		{
			do_encode(in, stdout);
			fclose(in);
			return 1;
		}
		
		char* encode;
		size_t inlen = strlen(data);
		size_t enc_sz = base64_encode_alloc(data, inlen, &encode);
		printf("%s", encode);
		
		return 1;
	}
	
	if(argc == 3 && (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--decode") == 0))
	{
		FILE* in;
		const char* data = argv[2];
		if((in = fopen(data, "rb")) != NULL)
		{
			do_decode(in, stdout);
			fclose(in);
			return 1;
		}
		
		char* decode;
		size_t inlen = strlen(data);
		struct base64_decode_context ctx;
		base64_decode_alloc_ctx(&ctx, data, inlen, &decode, &inlen);
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
		printf("\nb64 version: 0.0.2 Pre-Alpha\n");
		printf("Last revised: 02th Apr 2025\n");
		printf("The last version is always avaible\n");
		printf("Created By Emre Celebi\n");
		return 1;
	}
	
	return 0;
}

static void do_encode(FILE* in, FILE* out)
{
	char inbuf[ENC_BLOCKSIZE];
	char outbuf[BASE64_LENGTH(ENC_BLOCKSIZE)];
	size_t sum;
	
	do
	{
		size_t n;
		sum = 0;
		do
		{
			n = fread(inbuf + sum, 1, ENC_BLOCKSIZE - sum, in);
			sum += n;
		}while(!feof(in) && !ferror(in) && sum < ENC_BLOCKSIZE);
		
		if(sum > 0)
		{
			base64_encode(inbuf, sum, outbuf, BASE64_LENGTH(sum));
		}
	}while(!feof(in) && !ferror(in) && sum == ENC_BLOCKSIZE);
	
	if(fwrite(outbuf, 1, BASE64_LENGTH(sum), out) < sum)
	{
		printf("write error");
		exit(EXIT_FAILURE);
	}
}

static void do_decode(FILE* in, FILE* out)
{
	char inbuf[BASE64_LENGTH(DEC_BLOCKSIZE)];
	char outbuf[DEC_BLOCKSIZE];
	size_t sum;
	struct base64_decode_context ctx;
	
	base64_decode_ctx_init(&ctx);
	
	do
	{
		bool ok;
		size_t n;
		unsigned int k;
		
		sum = 0;
		do
		{
			n = fread(inbuf + sum, 1, BASE64_LENGTH(DEC_BLOCKSIZE) - sum, in);
			sum += n;
			
			if(ferror(in))
			{
				printf("read error");
				exit(EXIT_FAILURE);
			}
		}while(sum < BASE64_LENGTH(DEC_BLOCKSIZE) && !feof(in));
		
		for(k = 0; k < 1 + !!feof(in); k++)
		{
			if(k == 1 && ctx.i == 0)
				break;
			
			n = DEC_BLOCKSIZE;
			ok = base64_decode_ctx(&ctx, inbuf, (k == 0 ? sum : 0), outbuf, &n);
			
			if(fwrite(outbuf, 1, n, out) < n)
			{
				printf("write error");
				exit(EXIT_FAILURE);
			}
			
			if(!ok)
			{
				printf("invalid input");
				exit(EXIT_FAILURE);
			}
		}
	}while(!feof(in));
}