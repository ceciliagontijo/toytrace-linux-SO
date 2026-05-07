SEMANA 1 - Projeto SO

Onde o programa começa?
- Começa no CLI, pela chamada da função parse_args, que retornará pro main a chamada de sistema que o programa
realizou e o tipo dela.

Onde o processo alvo é criado?
- No runtime, na função launch_tracee.

Onde o runtime chama o callback?
- Na função trace_programm, que chama a função trace_observer sendo um de seus argumentos (callback).

Quais arquivos o grupo deve modificar?
- Runtime, pairer e formatter.

Qual TODO aparece primeiro ao executar o scaffold?
- erro: TODO Semana 2: implementar launch_tracee()

Qual a principal dúvida técnica do grupo neste momento?
- Entender como o toytrace combina as duas interrupções de uma syscall, a de entrada e a de saída, para imprimir no 
final uma linha completa sobre o que foi executado.

Mapa mental das funções:

	main
	|- trace_program
	|	|- launch_trace
	|	|- wait_for_initial_stop
	|	|- configure_trace_options
	|	|- resume_until_next_syscall
	|	|- wait_for_syscall_stop
	|	|- fill_event_from_regs
	|	|- observer
	|- trace_observer [callback]
	|	|- se [condição verdadeira]
	|	|   |- student_debug_raw_event
	|	|- senão 
	|	|   |- student_pair_syscall
	|	|   |- student_format_event
	|- trace_program
	        |- resume_until_next_syscall
