#include "student_api.h"

#include "syscall_names.h"
#include "trace_helpers.h"

#include <stdio.h>

extern char saved_execve_path[];  // ← acessa a variável do pairer.c

void student_debug_raw_event(const struct syscall_event *ev,
                             char *buf,
                             size_t bufsz)
{
    /*
     * Suporte de depuracao para a Semana 4:
     *
     * Esta funcao existe para inspecionar eventos crus depois que o runtime
     * ja consegue parar em syscalls e preencher struct syscall_event.
     * Ela nao e a formatacao final do projeto.
     *
     * Experimento sugerido:
     * - imprima o nome da syscall;
     * - imprima se o evento e entrada ou saida;
     * - imprima o pid;
     * - em eventos de entrada, observe os argumentos;
     * - em eventos de saida, observe o valor de retorno.
     *
     * Depois compare a saida de:
     *
     *   ./toytrace trace --raw-events -- ./tests/targets/hello_write
     *
     * A pergunta importante da Semana 4 e:
     * por que a mesma syscall aparece duas vezes?
     */
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
        /* read — você escreve aqui */
        snprintf(buf, bufsz, "read(%ld, %#lx, %ld) = %ld",
         ev->args[0],   /* fd    → número */
         ev->args[1],   /* buf   → ponteiro */
         ev->args[2],   /* count → número */
         ev->ret);      /* retorno */

    } else if (syscall == SYS_write) {
        /* write — você escreve aqui */
        snprintf(buf, bufsz,"write(%ld, %#lx, %ld) = %ld",
         ev->args[0],   
         ev->args[1],   
         ev->args[2],   
         ev->ret);      

    } else if (syscall == SYS_openat) {
        /* openat — você escreve aqui, precisa do read_child_string */
         char path[256];  // buffer local para guardar o caminho
    
        // lê a string que está na memória do processo filho
        // args[1] é o SEGUNDO argumento = o caminho do arquivo
        // (args[0] é o dirfd, que é só um número)
        if (read_child_string(ev->pid, ev->args[1], path, sizeof(path)) < 0)
            snprintf(path, sizeof(path), "<ilegivel>"); // se falhar, coloca isso
        
        snprintf(buf, bufsz, "openat(%ld, \"%s\", %#lx, %#lx) = %ld",
                ev->args[0],  // dirfd → número
                path,         // caminho que lemos → string
                ev->args[2],  // flags → hex
                ev->args[3],  // mode  → hex
                ev->ret);     // retorno
    

    } else if (syscall == SYS_execve) {
        /* execve — você escreve aqui, precisa do read_child_string */
        snprintf(buf, bufsz, "execve(\"%s\", ...) = %ld",
                saved_execve_path,
                ev->ret);. . 

    } else if (syscall == SYS_exit_group) {
        /* exit_group — você escreve aqui */
         snprintf(buf, bufsz, "exit_group(%ld)",
             ev->args[0]);

    } else {
        /* genérico — esse você já tem! */
        snprintf(buf, bufsz, "%s(%#lx, %#lx, %#lx, %#lx, %#lx, %#lx) = %ld",
                 syscall_name(ev->syscall_no),
                 ev->args[0], ev->args[1], ev->args[2],
                 ev->args[3], ev->args[4], ev->args[5],
                 ev->ret);
    }


}
