#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define numFiles 10


int main()
{
	int fp, i, j;
	int cols, rows;
	char sample[100] = "";
	char str[] = "x"; /* data to be put in text file */
	for(j = 1; j <= numFiles; j++)
	{
		cols = (5 + (rand() % 500)); /* num of cols between 5 - 505 */
		rows = (1000 + (rand() % 1000)); /* num of rows between 1000 - 2000 */
		fprintf(stdout, "rows: %d, cols: %d\n", rows, cols); /* display the number of rows & cols generating */
		sprintf(sample, "down%d.txt", j); /* create text file */
		fp = open(sample, O_WRONLY | O_APPEND | O_CREAT | O_EXCL, 0644); /* open the file with read and write permissions */
		if(fp == -1)
			fprintf(stdout, "There was an error opening the upload file.\n");
		
		for(i = 0; i < (rows * cols); i++)
		{
			if(i % cols == 0 && i != 0)
				write(fp, "\n", 1);
			write(fp, str, strlen(str)); /* write data to the text file */
		}

		close(fp);
	}
	return 0;
}
