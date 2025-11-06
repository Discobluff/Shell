#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT_SIZE 100

int main(void)
{
	char input[MAX_INPUT_SIZE];
	printf("> ");

	while(scanf("%s", input) != EOF) { 
		printf("%s\n> ", input);
		system(input);
	}

	printf("\n");

	return 0;
}
