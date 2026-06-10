#include "student_api.h"
#include "syscall_names.h"
#include "trace_helpers.h"
#include <sys/syscall.h>
#include <stdio.h>

extern char saved_execve_path[];  

void student_debug_raw_event(const struct syscall_event *ev,
                             char *buf,
                             size_t bufsz)
{
 
    snprintf(buf, bufsz, "pid=%d %s %s",
             ev->pid,
             syscall_name(ev->syscall_no),
             ev->entering ? "entrada" : "saida");
}

void student_format_event(const struct syscall_event *ev,
                          char *buf,
                          size_t bufsz)
{

    long syscall = ev->syscall_no;

    if (syscall == SYS_read) {
        snprintf(buf, bufsz, "read(%ld, %#lx, %ld) = %ld",
         ev->args[0],  
         ev->args[1],   
         ev->args[2],   
         ev->ret);      

    } else if (syscall == SYS_write) {
        snprintf(buf, bufsz,"write(%ld, %#lx, %ld) = %ld",
         ev->args[0],   
         ev->args[1],   
         ev->args[2],   
         ev->ret);      

    } else if (syscall == SYS_openat) {
         char path[256];  
    
        if (read_child_string(ev->pid, ev->args[1], path, sizeof(path)) < 0)
            snprintf(path, sizeof(path), "<ilegivel>"); 
        
        snprintf(buf, bufsz, "openat(%ld, \"%s\", %#lx, %#lx) = %ld",
                ev->args[0],  
                path,         
                ev->args[2],  
                ev->args[3],  
                ev->ret);     

    } else if (syscall == SYS_execve) {
        snprintf(buf, bufsz, "execve(\"%s\", ...) = %ld",
                saved_execve_path,
                ev->ret);

    } else if (syscall == SYS_exit_group) {
     
         snprintf(buf, bufsz, "exit_group(%ld)",
             ev->args[0]);

    } else {
        snprintf(buf, bufsz, "%s(%#lx, %#lx, %#lx, %#lx, %#lx, %#lx) = %ld",
                 syscall_name(ev->syscall_no),
                 ev->args[0], ev->args[1], ev->args[2],
                 ev->args[3], ev->args[4], ev->args[5],
                 ev->ret);
    }


}
