#define _XOPEN_SOURCE 700
#include "runner.h"
#include "cmd/cmd.h"
#include "parser/parser.h"
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

static int is_builtin_nofork(const char *cmd) {
    if (!cmd) return 0;
    if (strcmp(cmd, "cd") == 0) return 1;
    if (strcmp(cmd, "fg") == 0) return 1;
    if (strcmp(cmd, "bg") == 0) return 1;
    if (strcmp(cmd, "jobs") == 0) return 1;
    return 0;
}

static int check_background(command_t *cmd) {
    int len = 0;
    while(cmd->args[len]) len++;
    if (len > 0 && strcmp(cmd->args[len-1], "&") == 0) {
        // free(cmd->args[len-1]); // Do not free, main owns this string
        cmd->args[len-1] = NULL;
        return 1;
    }
    return 0;
}

static void reset_signals() {
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
}

// execute the commands as they come with pipe chaining
static int execute_pipeline(command_t* commands, int cmd_count)
{
    int i;
    pid_t pid;
    int bg = 0;
    
    // Check background on the last command
    if (check_background(&commands[cmd_count-1])) {
        bg = 1;
    }

    // Single command handling
    if (cmd_count == 1) {
        if (!bg && is_builtin_nofork(commands[0].args[0])) {
            int saved_stdin = dup(STDIN_FILENO);
            int saved_stdout = dup(STDOUT_FILENO);
            int saved_stderr = dup(STDERR_FILENO);
            int fd_in = -1, fd_out = -1, fd_err = -1;

            if (commands[0].input_file) {
                fd_in = open(commands[0].input_file, O_RDONLY);
                if (fd_in == -1) { perror("open input"); return 1; }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if (commands[0].output_file) {
                int flags = commands[0].append ? (O_WRONLY | O_CREAT | O_APPEND) : (O_WRONLY | O_CREAT | O_TRUNC);
                fd_out = open(commands[0].output_file, flags, 0644);
                if (fd_out == -1) { perror("open output"); return 1; }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            if (commands[0].error_file) {
                fd_err = open(commands[0].error_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_err == -1) { perror("open error"); return 1; }
                dup2(fd_err, STDERR_FILENO);
                close(fd_err);
            }

            function_handler(commands[0].args, false);

            dup2(saved_stdin, STDIN_FILENO); close(saved_stdin);
            dup2(saved_stdout, STDOUT_FILENO); close(saved_stdout);
            dup2(saved_stderr, STDERR_FILENO); close(saved_stderr);
            return 1;
        }

        // Forking for single command
        
        // Block SIGCHLD to safely handle job addition
        sigset_t mask, prev_mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, &prev_mask);

        pid = fork();
        if (pid == 0) {
            // Child
            setpgid(0, 0);
            if (!bg) tcsetpgrp(STDIN_FILENO, getpid());
            
            // Restore signals
            sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);
            signal(SIGCHLD, SIG_DFL);

            // Redirection
            if (commands[0].input_file) {
                int fd = open(commands[0].input_file, O_RDONLY);
                if (fd == -1) { perror("child open input"); exit(1); }
                dup2(fd, STDIN_FILENO); close(fd);
            }
            if (commands[0].output_file) {
                int flags = commands[0].append ? (O_WRONLY | O_CREAT | O_APPEND) : (O_WRONLY | O_CREAT | O_TRUNC);
                int fd = open(commands[0].output_file, flags, 0644);
                if (fd == -1) { perror("child open output"); exit(1); }
                dup2(fd, STDOUT_FILENO); close(fd);
            }
            if (commands[0].error_file) {
                int fd = open(commands[0].error_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) { perror("child open error"); exit(1); }
                dup2(fd, STDERR_FILENO); close(fd);
            }

            function_handler(commands[0].args, true); // exit_fork=true
            exit(0);
        } else if (pid > 0) {
            // Parent
            setpgid(pid, pid);
            if (!bg) {
                tcsetpgrp(STDIN_FILENO, pid);
                int status;
                waitpid(pid, &status, WUNTRACED);
                tcsetpgrp(STDIN_FILENO, getpgrp());
                
                if (WIFSTOPPED(status)) {
                    add_job(pid, commands[0].args[0]);
                    // Update to stopped
                    for (int j = 0; j < MAX_JOBS; j++) {
                        if (jobs_list[j].pid == pid) {
                            jobs_list[j].status = 1;
                            printf("\n[%d] Stopped %s\n", j+1, jobs_list[j].command);
                            break;
                        }
                    }
                }
            } else {
                add_job(pid, commands[0].args[0]);
                printf("[%d] %d\n", 1, pid); // TODO: print correct job ID
            }
            sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            return 1;
        } else {
            perror("fork");
            sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            return -1;
        }
    }

    // Pipeline Logic
    int pipefds[2 * (cmd_count - 1)];
    for (i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipefds + i * 2) == -1) {
            perror("pipe");
            return -1;
        }
    }

    sigset_t mask, prev_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, &prev_mask);

    pid_t group_pid = 0;
    int *child_pids = malloc(sizeof(int) * cmd_count);

    for (i = 0; i < cmd_count; i++) {
        pid = fork();
        if (pid == 0) {
            // Child
            pid_t my_pid = getpid();
            if (group_pid == 0) group_pid = my_pid; // First child is leader (approx)
            // Wait, child doesn't know group_pid if it's not the first one.
            // Actually, we need to pass group_pid.
            // For i=0, group_pid = my_pid.
            // For i>0, we need to know the pid of i=0.
            // We can't know it easily unless we pipe it or assume correct setpgid in parent.
            // Better: Child i=0 sets its own group.
            // Child i>0 sets its group to... wait.
            // Standard: Parent sets group for all children.
            // Children also set group to avoid race.
            
            // Simplified: All children call setpgid(0, 0) -> they become their own leaders?
            // No, they must be in SAME group.
            // We need the PID of the first child.
            // But checking that requires IPC or passing it.
            // For now, let's let Parent do setpgid.
            // Child waits? No.
            
            // To make this robust is hard in one go.
            // Let's just make each child its own group leader for now? No, pipe won't work well with signals.
            // Let's assume parent sets it fast enough or use 0 for i=0.
            // But for i>0, they need to join i=0.
            // This requires the parent to have forked i=0 and got its PID.
            // So this loop works.
            
            // However, inside child 'i', 'group_pid' variable is 0! (Copy of parent's at start of loop).
            // We need to pass the first child's PID.
            // But we are in a loop.
            // Variable 'group_pid' is updated in PARENT, but CHILD sees the value from BEFORE fork?
            // No, 'group_pid' is local.
            // At i=0: group_pid=0. Fork. Child sees 0. Parent sees PID0. Parent sets group_pid=PID0.
            // At i=1: group_pid=PID0. Fork. Child sees PID0. Parent sees PID0.
            // So it WORKS for i>0!
            // Only i=0 child needs to know it is the leader.
            
            if (i == 0) group_pid = getpid(); // I am leader
            setpgid(0, group_pid);
            if (i==0 && !bg) tcsetpgrp(STDIN_FILENO, group_pid);

            sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            reset_signals();

            // IO Setup
            if (i == 0) {
                if (commands[i].input_file) {
                    int fd = open(commands[i].input_file, O_RDONLY);
                    if (fd != -1) { dup2(fd, STDIN_FILENO); close(fd); }
                }
            } else {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }

            if (i == cmd_count - 1) {
                if (commands[i].output_file) {
                    int flags = commands[i].append ? (O_WRONLY | O_CREAT | O_APPEND) : (O_WRONLY | O_CREAT | O_TRUNC);
                    int fd = open(commands[i].output_file, flags, 0644);
                    if (fd != -1) { dup2(fd, STDOUT_FILENO); close(fd); }
                }
            } else {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }

            for (int j = 0; j < 2 * (cmd_count - 1); j++) close(pipefds[j]);

            function_handler(commands[i].args, true);
            exit(0);
        } else if (pid > 0) {
            child_pids[i] = pid;
            if (i == 0) group_pid = pid;
            setpgid(pid, group_pid);
        } else {
            perror("fork");
        }
    }

    for (i = 0; i < 2 * (cmd_count - 1); i++) close(pipefds[i]);

    if (!bg) {
        tcsetpgrp(STDIN_FILENO, group_pid);
        for (i = 0; i < cmd_count; i++) {
            int status;
            waitpid(child_pids[i], &status, WUNTRACED);
            if (WIFSTOPPED(status) && i == cmd_count-1) { // Only track last one? Or leader?
                 // Track leader usually.
                 add_job(group_pid, commands[0].args[0]);
                 for (int j=0; j<MAX_JOBS; j++) if(jobs_list[j].pid == group_pid) jobs_list[j].status = 1;
            }
        }
        tcsetpgrp(STDIN_FILENO, getpgrp());
    } else {
        add_job(group_pid, commands[0].args[0]);
        printf("[%d] %d\n", 1, group_pid);
    }

    free(child_pids);
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
    return 1;
}

// runs one command 
int run(char** line)
{
	if (line[0] == NULL) {
		// An empty command ?
		return 1;
	}

	command_t* commands;
	
	int cmd_count = parse_pipeline(line, &commands);
	
	// prints the chained commands if debug is set to 1
	for (int i = 0; i < cmd_count*debug; i++){
		printf("Command %d: name %s\ninput: %s\noutput: %s\nerror: %s\n",
			i + 1,
			commands[i].args[0],
			commands[i].input_file,
			commands[i].output_file,
			commands[i].error_file
		);
	}

	int result = execute_pipeline(commands, cmd_count);

	free_commands(commands, cmd_count);
	return result;
}

