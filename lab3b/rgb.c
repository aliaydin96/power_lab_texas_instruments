#define RCGCGPIO (*((unsigned int *)0x400FE608U)) // clock gating control

#define GPIOF_BASE 0x40025000U
#define GPIOF_DIR (*((unsigned int *)(GPIOF_BASE + 0x400U)))
#define GPIOF_DEN (*((unsigned int *)(GPIOF_BASE + 0x51CU)))
#define GPIOF_DATA (*((unsigned int *)(GPIOF_BASE + 0x3FCU)))


void main(void)
{
    RCGCGPIO = 0x20U;  // enable clock for GPIOF
    GPIOF_DIR = 0x0EU;  // set pins 1,2 and 3 as outputs
    GPIOF_DEN = 0x0EU;

    while(1){
        GPIOF_DATA = 0x02U;
        int counter=0;
        while(counter<500000){
            ++counter;
        }
        GPIOF_DATA = 0x00U;
        GPIOF_DATA = 0x04U;
        counter=0;
        while(counter<500000){
            ++counter;
        }
        GPIOF_DATA = 0x00U;
        GPIOF_DATA = 0x06U;
        counter=0;
        while(counter<500000){
            ++counter;
        }
        GPIOF_DATA = 0x00U;
        GPIOF_DATA = 0x08U;
         counter=0;
         while(counter<500000){
             ++counter;
         }
         GPIOF_DATA = 0x00U;
         GPIOF_DATA = 0x0aU;
         counter=0;
         while(counter<500000){
              ++counter;
         }
         GPIOF_DATA = 0x00U;
         GPIOF_DATA = 0x0cU;
          counter=0;
          while(counter<500000){
              ++counter;
          }
          GPIOF_DATA = 0x00U;
          GPIOF_DATA = 0x0eU;
           counter=0;
           while(counter<500000){
               ++counter;
           }
          GPIOF_DATA = 0x00U;


    }
}
