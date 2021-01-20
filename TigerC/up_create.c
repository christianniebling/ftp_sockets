#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define fileNum 100


int main()
{
	int fp, i, j;
	int cols, rows;
	char sample[100] = "";
	char str[] = "meow ";
	for(j = 1; j <= fileNum; j++)
	{
		cols = (5 + (rand() % 500));
		rows = (1000 + (rand() % 1000));
		fprintf(stdout, "rows: %d, cols: %d\n", rows, cols);
		sprintf(sample, "upload%d.txt", j);
		fp = open(sample, O_WRONLY | O_APPEND | O_CREAT | O_EXCL, 0644);
		if(fp == -1)
			fprintf(stdout, "oops\n");
		
		for(i = 0; i < (rows * cols); i++)
		{
			if(i % cols == 0 && i != 0)
				write(fp, "\n", 1);
			write(fp, str, strlen(str));
		}

		close(fp);
	}
	return 0;
}
