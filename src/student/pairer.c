#include "student_api.h"

int student_pair_syscall(struct syscall_pairer *pairer,
                         const struct syscall_event *ev,
                         struct syscall_event *out)
{

    if (ev->entering == 1) {  //entrada, salvar o envento inteiro dentro do pairer
        pairer->entry = *ev;
        pairer->has_entry = 1;
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
    return 0;
}
