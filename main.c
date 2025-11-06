#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT_SIZE 100

int main(void)
{
	char input[MAX_INPUT_SIZE];
	printf("> ");

	do { 
		printf("%s\n> ", input);
	} while(scanf("%s", input) != EOF);

	printf("\n");

	return 0;
}
