#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "base64.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

#define ENC_BLOCKSIZE (1024*3*10)
#define DEC_BLOCKSIZE (1024*3)

static void do_encode(FILE* in, FILE* out, uintmax_t wrap_column);
static void do_decode(FILE* in, FILE* out, bool ignore_garbage);
static void wrap_write(const char* buffer, size_t len, uintmax_t wrap_column, size_t* current_column, FILE* out);

int main(int argc, char** argv)
{
	uintmax_t wrap_column = 76;
	bool ignore_garbage = true; // false;
	
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
			do_encode(in, stdout, wrap_column);
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
			do_decode(in, stdout, ignore_garbage);
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
		printf("\nb64 version: 0.0.3 Pre-Alpha\n");
		printf("Last revised: 03th Apr 2025\n");
		printf("The last version is always avaible\n");
		printf("Created By Emre Celebi\n");
		return 1;
	}
	
	return 0;
}

static void wrap_write(const char* buffer, size_t len, uintmax_t wrap_column, size_t* current_column, FILE* out)
{
	size_t written = 0;
	
	if(wrap_column == 0)
	{
		if (fwrite (buffer, 1, len, stdout) < len)
		{
			printf("write error");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		for(written = 0; written < len;)
		{
			uintmax_t cols_remaining = wrap_column - *current_column;
			size_t to_write = MIN(cols_remaining, SIZE_MAX);
			to_write = MIN(to_write, len - written);
			
			if(to_write == 0)
			{
				if(fputc ('\n', out) == EOF)
				{
					printf("write error");
					exit(EXIT_FAILURE);
				}
				*current_column = 0;
			}
			else
			{
				if(fwrite(buffer + written, 1, to_write, stdout) < to_write)
				{
					printf("write error");
					exit(EXIT_FAILURE);
				}
				*current_column += to_write;
				written += to_write;
			}
		}
	}
}

static void do_encode(FILE* in, FILE* out, uintmax_t wrap_column)
{
	size_t current_column = 0;
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
			wrap_write(outbuf, BASE64_LENGTH(sum), wrap_column, &current_column, out);
		}
	}while(!feof(in) && !ferror(in) && sum == ENC_BLOCKSIZE);
}

static void do_decode(FILE* in, FILE* out, bool ignore_garbage)
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
			if(ignore_garbage)
			{
				for(size_t i = 0; n > 0 && i < n;)
				{
					if(isbase64(inbuf[sum + i]) || inbuf[sum + i] == '=')
						i++;
					else
						memmove(inbuf + sum + i, inbuf + sum + i + 1, --n - i);
				}
			}
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