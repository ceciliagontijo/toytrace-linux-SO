#include "student_api.h"
#include "trace_helpers.h"
#include <string.h>
#include <sys/syscall.h>

char saved_execve_path[256];  


int student_pair_syscall(struct syscall_pairer *pairer,
                         const struct syscall_event *ev,
                         struct syscall_event *out)
{

    if (ev->entering == 1) {  
        pairer->entry = *ev;
        pairer->has_entry = 1;

       
        if (ev->syscall_no == SYS_execve) {
            if (read_child_string(ev->pid, ev->args[0],
                                  saved_execve_path,
                                  sizeof(saved_execve_path)) < 0)
                snprintf(saved_execve_path, sizeof(saved_execve_path), "<ilegivel>");
        }

        return 0;
    }

    if (!pairer->has_entry) {
        return -1;
    }
    *out = pairer->entry;
    out->ret = ev->ret;
    out->entering = 0;

    pairer->has_entry = 0;

    return 1;
    
}

