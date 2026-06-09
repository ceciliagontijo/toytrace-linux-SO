#include "student_api.h"
#include "trace_helpers.h"
#include <string.h>

static char saved_execve_path[256];  // ← variável global

// O execve é especial: quando ele executa com sucesso, o kernel substitui
// completamente a imagem do processo (código, dados, pilha).
// Isso significa que a string do path em args[0] deixa de existir na memória
// do filho depois que o execve termina.
// O student_format_event só é chamado após o par estar completo (entrada + saída),
// ou seja, depois que o execve já rodou e destruiu a memória.
// Por isso precisamos ler o path DURANTE A ENTRADA, enquanto a memória ainda
// existe, e guardar em uma variável global para usar depois na formatação.

int student_pair_syscall(struct syscall_pairer *pairer,
                         const struct syscall_event *ev,
                         struct syscall_event *out)
{

    if (ev->entering == 1) {  //entrada, salvar o envento inteiro dentro do pairer
        pairer->entry = *ev;
        pairer->has_entry = 1;

         // se for execve, lê o path AGORA enquanto a memória ainda existe
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

    // na saida, add o valor de retorno que so existe agora 
    *out = pairer->entry;
    out->ret = ev->ret;
    out->entering = 0;

    pairer->has_entry = 0;

    return 1; //sinaliza que esta pronto para formatar 
    
}

