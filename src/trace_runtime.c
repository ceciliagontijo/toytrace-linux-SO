#include "trace_runtime.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#if !defined(__x86_64__)
#error "Este runtime didatico suporta apenas Linux x86_64."
#endif

static void fill_event_from_regs(pid_t pid,
                                 int entering,
                                 const struct user_regs_struct *regs,
                                 struct syscall_event *ev)
{
  
    memset(ev, 0, sizeof(*ev));
    ev->pid = pid;
    ev->entering = entering;

    if(entering != 0)
    {
        ev->ret = regs->rax;
    }

    ev->syscall_no = regs->orig_rax;
    ev->args[0]    = regs->rdi;
    ev->args[1]    = regs->rsi;
    ev->args[2]    = regs->rdx;
    ev->args[3]    = regs->r10;
    ev->args[4]    = regs->r8;
    ev->args[5]    = regs->r9;
}

static pid_t launch_tracee(char *const argv[])
{
    pid_t filho = fork();
    	
	if(filho<0)
	{
		perror("Erro no fork");
		return -1;
	}
    else{
	if (filho == 0)
	  {
		
		if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) <0 ){
		perror("Erro no ptrace");
		_exit(1);
		}
		raise(SIGSTOP);
		execvp(argv[0], argv);
		perror("Erro no exec");
		_exit(1);
	  }
     	else 
	{
		return filho;
	}
     }
     
}

static int wait_for_initial_stop(pid_t child)
{
     int status;

     if (waitpid(child, &status, 0) < 0)
     {
        perror("Erro no waitpid");
        return -1;
     }
     
     if(WIFSTOPPED(status))
     {
        return 0;
     }
     else
     {
        return -1;
     }
}

static int configure_trace_options(pid_t child)
{

    if(ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_TRACESYSGOOD) < 0)
    {
        perror("Erro no ptrace");
		return -1;
    }
    return 0;

}

static int resume_until_next_syscall(pid_t child, int signal_to_deliver)
{
    if (ptrace (PTRACE_SYSCALL, child, NULL, signal_to_deliver) < 0)
    {
        perror("Erro no ptrace");
        return -1;
    }
    return 0;
}

static int wait_for_syscall_stop(pid_t child, int *status)
{
     while (1) 
    {
        if (waitpid(child, status, 0) < 0) {
            perror("Erro no waitpid");
            return -1;
        }

        if (WIFEXITED(*status) || WIFSIGNALED(*status)) {
            return 0;
        }

        if (WIFSTOPPED(*status) && WSTOPSIG(*status) & 0x80) {
            return 1;
        }

        ptrace(PTRACE_SYSCALL, child, NULL, 0);
    }
}


int trace_program(char *const argv[],
                  trace_observer_fn observer,
                  void *userdata)
{
    pid_t child;
    int status = 0;
    int entering = 1;

    if (argv == NULL || argv[0] == NULL) {
        fprintf(stderr, "erro: programa alvo ausente\n");
        return -1;
    }

    child = launch_tracee(argv);
    if (child < 0) {
        return -1;
    }

    if (wait_for_initial_stop(child) < 0) {
        return -1;
    }

    if (configure_trace_options(child) < 0) {
        return -1;
    }

    if (resume_until_next_syscall(child, 0) < 0) {
        return -1;
    }

    while (1) {
        struct user_regs_struct regs;
        struct syscall_event ev;
        int stop_kind;

        stop_kind = wait_for_syscall_stop(child, &status);
        if (stop_kind < 0) {
            return -1;
        }
        if (stop_kind == 0) {
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }
            if (WIFSIGNALED(status)) {
                return 128 + WTERMSIG(status);
            }
            return 0;
        }

        if (ptrace(PTRACE_GETREGS, child, NULL, &regs) < 0) 
        {
            perror("Erro PTRACE_GETREGS");
            return -1;
        }
        //memset(&regs, 0, sizeof(regs));
        fill_event_from_regs(child, entering, &regs, &ev);
        if (observer != NULL) {
            observer(&ev, userdata);
        }

        entering = !entering;

        if (resume_until_next_syscall(child, 0) < 0) {
            return -1;
        }
    }
}
