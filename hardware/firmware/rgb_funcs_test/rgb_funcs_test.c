//
//
//
//
//

#include <stdio.h>
#include <stdint.h>


#define setRGB(r,g,b)  printf("%d,%d,%d", r,g,b);

#include "../color_funcs.h"


int main(void)
{
    printf("rgb_funcs_test\n");

#if 0
    rgb_t c1 = {0,0,0};
    rgb_t c2 = {100,10,50};
    int steps = 23;
#endif

#if 1
    rgb_t c1 = {100,10,50};
    rgb_t c2 = {0,204,30};
    int steps = 53;
#endif

    rgb_setCurr( &c1 );
    rgb_setDest( &c2, steps );  
    printf("step: %d,%d,%d \n", step100x.r, step100x.g,step100x.b);

    for( int i=0; i<steps+4; i++ ) {
        printf("%d:",i);
        rgb_updateCurrent();
        printf("\n");
        //printf("%d: %d,%d,%d \n", i, 
        //       curr100x.r/100, curr100x.g/100, curr100x.b/100 );
    }

    return 0;
}
