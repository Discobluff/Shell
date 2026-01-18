#include "parser.h"

char *strCopy(char *input, int indexStart, int indexEnd){
	int len = indexEnd-indexStart+2;
	char *res = malloc(sizeof(char)*len);
	for (int i=indexStart;i<=indexEnd;i++){
		res[i-indexStart] = input[i];
	}
	res[len-1] = '\0';
	return res;
}

const int nb_commands_without_fork = 6;
static const char *commands_without_fork[] = {"cd", "myinfo", "myenv", "mymaps", "mydump","mylof"};

int split_string_with_delimiters(char *line, char ***split, ActionCond **actionsCond){
	int split_count = 0;
	int i = 0;

	bool quote = false;
	while (line[i] != '\0') {
		if (!quote && (line[i] == '"' || line[i] == '\'')){
			quote = true;
		}
		else{
			if (quote && (line[i] == '"' || line[i] == '\'')){
				quote = false;
			}
		}
		if (quote){
			i++;
			continue;
		}
		if (line[i] == ';') {
			split_count++;
		}
		if ((line[i] == '&' && line[i+1] == '&') || (line[i] == '|' && line[i+1] == '|')){
			split_count++;
			i++;
		}
		i++;
	}
	split_count++;
	(*split) = malloc(sizeof(char*)*split_count);
	if (!(*split)){
		perror("shell: allocation error\n");
		exit(EXIT_FAILURE);
	}
	if (split_count == 1){
		(*actionsCond) = NULL;
	}
	else{
		(*actionsCond) = malloc(sizeof(ActionCond)*(split_count-1));
		if (!(*actionsCond)){
			perror("shell: allocation error\n");
			exit(EXIT_FAILURE);
		}
	}
	int indexSplit = 0;
	int indexLineStart = 0;
	int indexLineEnd = 0;
	quote = false;
	while (line[indexLineEnd] != '\0'){
		if (!quote && (line[indexLineEnd] == '"' || line[indexLineEnd] == '\'')){
			quote = true;
		}
		else{
			if (quote && (line[indexLineEnd] == '"' || line[indexLineEnd] == '\'')){
				quote = false;
			}
		}
		if (quote){
			indexLineEnd++;
			continue;
		}
		if (line[indexLineEnd] == ';'){
			(*split)[indexSplit] = strCopy(line, indexLineStart, indexLineEnd-1);
			(*actionsCond)[indexSplit] = COND_NONE;
			indexSplit++;
			indexLineStart = indexLineEnd+1;
		}
		else if (line[indexLineEnd] == '&' && line[indexLineEnd+1] == '&'){
			(*split)[indexSplit] = strCopy(line, indexLineStart, indexLineEnd-1);
			(*actionsCond)[indexSplit] = COND_AND;
			indexSplit++;
			indexLineEnd++;
			indexLineStart = indexLineEnd+1;
		}
		else if (line[indexLineEnd] == '|' && line[indexLineEnd+1] == '|'){
			(*split)[indexSplit] = strCopy(line, indexLineStart, indexLineEnd-1);
			(*actionsCond)[indexSplit] = COND_OR;
			indexSplit++;
			indexLineEnd++;
			indexLineStart = indexLineEnd+1;
		}
		indexLineEnd++;
	}
	(*split)[indexSplit] = strCopy(line, indexLineStart, indexLineEnd-1);
	return split_count;
}

int split_to_args_list(char *line, char ****args_list, ActionCond **actionsCond){
	char **split;
	int count = split_string_with_delimiters(line, &split, actionsCond);
	(*args_list) = malloc(sizeof(char**)*count);
	for (int i=0;i<count;i++){
		(*args_list)[i] = split_to_args(split[i]);
		free(split[i]);
	}
	free(split);
	return count;
}

char *strCopyIgnoreQuote(char *input, int indexStart, int indexEnd){
	int size = 1;
	for (int i=indexStart; i<=indexEnd;i++){
		if (input[i] == '\\' && input[i+1] == ' '){
			size++;
			i++;
			continue;
		}
		if (input[i] != '"' && input[i] != '\''){
			size++;
		}
	}
	int shiftQuote = 0;
	char *res = malloc(sizeof(char)*size);
	for (int i=indexStart;i<=indexEnd;i++){
		if (input[i] == '"' || input[i] == '\'' || input[i] == '\\'){
			shiftQuote++;
		}
		else{
			res[i-indexStart- shiftQuote] = input[i];
		}
	}
	res[size-1] = '\0';
	return res;
}

char **split_to_args(char *line){
	int bufsize = TOKEN_BUFSIZE, position = 0;
	char** tokens = malloc(bufsize * sizeof(char*));

	if (!tokens) {
		perror("shell: allocation error\n");
		exit(EXIT_FAILURE);
  	}
	int indexStart = 0;
	int indexEnd = 0;
	bool quote = false;
	while (line[indexEnd] != '\0'){
		if (!quote && (line[indexEnd] == '"' || line[indexEnd] == '\'')){
			quote = true;
		}
		else{
			if (quote && (line[indexEnd] == '"' || line[indexEnd] == '\'')){
				quote = false;
			}
		}
		if (quote){
			indexEnd++;
			continue;
		}
		bool delimEncountered = false;
		for (int indexDelim=0; indexDelim<(int)strlen(TOKEN_DELIM);indexDelim++){
			if (line[indexEnd] == TOKEN_DELIM[indexDelim] && !(indexEnd>0 && line[indexEnd-1] == '\\')){
				if (indexStart < indexEnd){
					char *token = strCopyIgnoreQuote(line, indexStart, indexEnd-1);
					tokens[position] = token;
					position++;
				}
					delimEncountered = true;
					indexEnd++;
					indexStart = indexEnd;
					break;
			}
		}
		if (!delimEncountered){
			indexEnd++;
		}
	}
	if (indexStart <indexEnd){
		char *token = strCopyIgnoreQuote(line, indexStart, indexEnd-1);
		tokens[position] = token;
		position++;
	}
	tokens[position] = NULL;
	return tokens;
}

// gets line from stdin & ends program when EOF is encountered
char* get_line(void)
{
	char *line = NULL;
	size_t bufsize = 0;

	if (getline(&line, &bufsize, stdin) == -1){
		if (feof(stdin)) {
			free(line);
			printf("\n");
			exit(EXIT_SUCCESS);
		} else  {
			perror("readline");
			exit(EXIT_FAILURE);
		}
	}
	return line;
}



static void init_command(command_t* cmd)
{
	cmd->args = NULL;
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->append = false;
	cmd->error_file = NULL;
	cmd->need_fork = true;
}


static char* expand_variable(const char* token)
{
	const char* var_name = token + 1; // go past the '$'

	// special variables $? / $$ ...
	if (*var_name == '$') {
		static char buf[16];
		snprintf(buf, sizeof(buf), "%d", getpid());
		return strdup(buf);
	}
	else if (!isalnum(*var_name)) {
		fprintf(stderr, "oops this is not implemented yet... (only done $VAR and $$)\n");
		return NULL; 
	}

	char* value = getenv(var_name);
	if (value) {
		return strdup(value);
	}

	return strdup("");
}


// returns the number of commands separated by pipes and puts them in commands
int parse_pipeline(char** line, command_t** commands)
{
	int cmd_count = 0;
	int i = 0;

	// Compter le nombre de commandes (nombre de pipes + 1)
	while (line[i] != NULL) {
		if (strcmp(line[i], "|") == 0) {
			cmd_count++;
		}
		i++;
	}
	cmd_count++;

	*commands = malloc(sizeof(command_t) * cmd_count);
	if (!*commands) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	int cmd_idx = 0;
	int start = 0;

	for (i = 0; line[i] != NULL || (i > start && cmd_idx < cmd_count); ) {
		init_command(&(*commands)[cmd_idx]);
		
		int end = start;
		while (line[end] != NULL && strcmp(line[end], "|") != 0) {
			end++;
		}
		
		int arg_count = 0;
		int j = start;
		
		while (j < end) {
			if (strcmp(line[j], "<") == 0
				 || strcmp(line[j], ">") == 0
				 || strcmp(line[j], ">>") == 0
				 || strcmp(line[j], "2>") == 0) {
				j += 2;
			} else {
				arg_count++;
				j++;
			}
		}
		
		char** args = malloc(sizeof(char*) * (arg_count + 1));
		if (!args) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		
		int arg_idx = 0;
		j = start;
		
		while (j < end) {
			for (int k=0;k<nb_commands_without_fork;k++){
				if (strcmp(line[j], commands_without_fork[k]) == 0){
					(*commands)[cmd_idx].need_fork = false;
					break;
				}
			}
			if (debug) printf("on token %s\n", line[j]);

			if (strcmp(line[j], "<") == 0) {
				if (j + 1 < end && line[j + 1] != NULL) {
					(*commands)[cmd_idx].input_file = line[j + 1];
					j += 2;
				} else {
					fprintf(stderr, "Error: missing filename after '<'\n");
					j++;
				}
			} 
			else if (strcmp(line[j], ">") == 0) {
				if (j + 1 < end && line[j + 1] != NULL) {
					(*commands)[cmd_idx].output_file = line[j + 1];
					(*commands)[cmd_idx].append = false;
					j += 2;
				} else {
					fprintf(stderr, "Error: missing filename after '>'\n");
					j++;
				}
			}
			else if (strcmp(line[j], ">>") == 0) {
				if (j + 1 < end && line[j + 1] != NULL) {
					(*commands)[cmd_idx].output_file = line[j + 1];
					(*commands)[cmd_idx].append = true;
					j += 2;
				} else {
					fprintf(stderr, "Error: missing filename after '>>'\n");
					j++;
				}
			}
			else if (strcmp(line[j], "2>") == 0) {
				if (j + 1 < end && line[j + 1] != NULL) {
					(*commands)[cmd_idx].error_file = line[j + 1];
					j += 2;
				} else {
					fprintf(stderr, "Error: missing filename after '2>'\n");
					j++;
				}
			}
			else if (strstr(line[j], "$")) {
				args[arg_idx++] = expand_variable(line[j]);
				if (debug) printf("\t> command[%d] arg %d = '%s' (to expand)\n", cmd_idx, arg_idx - 1, line[j]);
				j++;
			}
			else if (strcmp(line[j], "$(") == 0) {
				fprintf(stderr, "comming soon!\n");
			}
			else {
				args[arg_idx++] = line[j];
				if (debug) printf("\t> command[%d] arg %d = '%s'\n", cmd_idx, arg_idx - 1, line[j]);
				j++;
			}
		}
		
		args[arg_idx] = NULL;
		(*commands)[cmd_idx].args = args;
		
		if (line[end] != NULL && strcmp(line[end], "|") == 0) {
			start = end + 1;
			i = start;
		} else {
			i = end;
		}
		
		cmd_idx++;
	}

	return cmd_count;
}

// Fonction pour libérer la mémoire des commandes
void free_commands(command_t* commands, int cmd_count)
{
	for (int i = 0; i < cmd_count; i++) {
		if (commands[i].args != NULL) {
			free(commands[i].args);
		}
	}
	free(commands);
}
