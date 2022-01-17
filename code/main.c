#include <stdio.h>
#include "fila.h"
#include "fileb.c"

int main()
{
    int bit_pushed = BITMACRO(3);
    printf("\nHello, world!\n the variable x is: %d\n 1 bitshifted 3 times to the left gives: %d", the_variable_x(), bit_pushed);

    return 0;
}