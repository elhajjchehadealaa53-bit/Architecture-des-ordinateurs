/* minirisc_init.S — Startup minimal Mini-RISC
 * Objectif :
 *  - initialiser la pile (sp = __stack_top)
 *  - appeler _start (libc/newlib) qui finit par appeler main()
 *  - terminer l’émulation via ebreak
 */

    .section .minirisc_init, "ax"
    .globl _minirisc_init
    .type  _minirisc_init, @function

_minirisc_init:
    /* 1) Initialiser la pile */
    la   sp, __stack_top

    /* 2) Entrer dans le runtime C (newlib) -> main() */
    call _start

    /* 3) Si main() retourne, arrêter l’émulateur */
    ebreak

    .size _minirisc_init, . - _minirisc_init