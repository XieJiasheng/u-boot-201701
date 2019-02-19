#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BL_SIZE (16*1024)
#define BL_HEADER_INFO "    "
#define BL_HEADER_SIZE 16


int main(int argc, char const *argv[])
{
	FILE *fp = NULL;
	int file_len = 0;
	unsigned char *buff = NULL, data = 0;
	int buf_len = 0;
	unsigned int checksum = 0, count;
	int i = 0;
	unsigned int nbytes = 0;

	if (3 != argc)
	{
		printf("argc parameter number is %d, not 3\n", argc);
		printf("%s <source file> <destination>\n", argv[0]);
		return -1;
	}

	buff = (char*)malloc(BL_SIZE);
	if (NULL == buff)
	{
		printf("malloc error\n");
		return -1;
	}

	memset(buff, 0, BL_SIZE);

	fp = fopen(argv[1], "rb");
	if (NULL == fp)
	{
		printf("open source file error\n");
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	count = (file_len < (BL_SIZE - BL_HEADER_SIZE))?file_len:(BL_SIZE - BL_HEADER_SIZE);
	memcpy(buff, BL_HEADER_INFO, BL_HEADER_SIZE);
	nbytes = fread(buff+ BL_HEADER_SIZE, 1, count, fp);
	if (count != nbytes)
	{
		printf("fread %s failed\n", argv[1]);
		free(buff);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	fp = NULL;


	// calculate checksum
	for (i = 0; i < count; ++i)
	{
		data = *(volatile unsigned char*)(buff + BL_HEADER_SIZE + i);
		checksum += data;
	}

	*(volatile unsigned int *) buff = BL_SIZE;
	*(volatile unsigned int *)(buff + 8) = checksum;

	fp = fopen(argv[2], "wb");
	if (NULL == fp)
	{
		printf("open aim file error\n");
		return -1;
	}

	nbytes = fwrite(buff, 1, BL_SIZE, fp);
	if (BL_SIZE != nbytes)
	{
		printf("write aim file failed\n");
		free(buff);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	free(buff);

	return 0;
}