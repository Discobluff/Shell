#include "cmd.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>

int mymaps(char **args){
	if(args[1]==NULL){
		fprintf(stderr, "Usage: mymaps -p <PID>\n");
		return EINVAL;
	}

	if(!strcmp("-p",args[1]) == 0){
		fprintf(stderr, "Usage: mymaps -p <PID>\n");
		return EINVAL;
	}

	char *pid = args[2];
	char path[200];

	sprintf(path, "/proc/%s/maps", pid);

	FILE *file = fopen(path, "r");
	if (file == NULL) {
		perror("Error opening file");
		return EFAULT;
	}

	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		//https://man7.org/linux/man-pages/man5/proc_pid_maps.5.html
		//The format of the file is: 
		//address perms offset  dev   inode       pathname
		char address[256],perms[256], offset[256],  dev[256], pathname[256];
		long inode=0;
		address[0] = '\0'; perms[0] = '\0'; offset[0] = '\0'; dev[0] = '\0'; pathname[0] = '\0';
		
		int fields = sscanf(buffer, "%s %s %s %s %ld %s", address, perms, offset, dev, &inode, pathname);
		if(fields < 5){
			fprintf(stderr, "Error parsing line: %s", buffer);
			// EINTR 	Interrupted function.
			return EINTR;
		}
		
		bool suspicion = false;

		//Stack Overflow exploits-> Non-executable stack
		//https://security.stackexchange.com/questions/210785/how-and-why-is-executable-stack-dangerous
		//https://aeb.win.tue.nl/linux/hh/protection.html
		//(https://cwe.mitre.org/data/definitions/121.html)
		//Check if the stack is executable
		if(strstr(pathname, "[stack]") != NULL&& strchr(perms, 'x') != NULL){
				suspicion = true;
		}

		//W^X Principle -> Arbitrary Code Execution
		//https://cwe.mitre.org/data/definitions/94.html
		//https://trovent.io/en/exploiting-rwx-memory-regions/
		//https://security.stackexchange.com/questions/18936/what-attacks-does-a-wx-policy-prevent-against
		//Check memory regions with write and execute permissions (-wx, rwx, etc)
		if(strchr(perms, 'w') != NULL && strchr(perms, 'x') != NULL){
			suspicion = true;
		}

		//Malfind-like detection
		//https://github.com/volatilityfoundation/volatility/wiki/Command-Reference-Mal#malfind
		//https://github.com/volatilityfoundation/volatility/blob/master/volatility/plugins/overlays/linux/linux.py#L1171
		//https://github.com/volatilityfoundation/volatility/blob/328a178edeecaf8d2abf96618ecfcb14a7b0c860/volatility/plugins/linux/malfind.py
		if(strchr(perms, 'x') != NULL && strlen(pathname) == 0 ){
			//VM regions with no associated file with execute permissions
			suspicion = true;
		}
		// named VM regions with read and execute permissions-> only [vdso] is allowed
		if(!strlen(pathname) == 0 && strchr(perms, '[') != NULL &&strchr(perms, ']') != NULL && strchr(perms, 'x') != NULL && strstr(pathname, "[vdso]") == NULL){
			//VM regions with write and execute permissions
			suspicion = true;
		}
		printf("%-35s %-5s %-10s %-10ld %-4s %s\n", 
                   address, perms, offset, inode, 
                   suspicion ? "[*]" : "   ", 
                   pathname);		
		
	}

	fclose(file);

	return 0;
}
