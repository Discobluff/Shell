#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 100

int main(void)
{
	char input[MAX_INPUT_SIZE];
	printf("> ");
	while (fgets(input, MAX_INPUT_SIZE, stdin) != NULL) {
		input[strcspn(input, "\n")] = '\0';
		printf("> ");
		system(input);
	}

	printf("\n");

	return 0;
}
