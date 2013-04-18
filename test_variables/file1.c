#include <stdio.h>
#include "globals.h"

int main(void)
{	
    printf("main: %d\n", GLOBAL);
    func2();

    return 0;
}

