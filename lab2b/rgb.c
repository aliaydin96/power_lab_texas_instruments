
void main(void)
{
    *((unsigned int *)0x400FE608U) = 0x20U;
    *((unsigned int *)0x40025400U) = 0x0EU;
    *((unsigned int *)0x4002551CU) = 0x0EU;
    while(1){
        *((unsigned int *)0x400253FCU) = 0x02U;
        int counter=0;
        while(counter<500000){
            ++counter;
        }
        *((unsigned int *)0x400253FCU) = 0x00U;
        *((unsigned int *)0x400253FCU) = 0x04U;
        counter=0;
        while(counter<500000){
            ++counter;
        }
        *((unsigned int *)0x400253FCU) = 0x00U;
        *((unsigned int *)0x400253FCU) = 0x06U;
        counter=0;
        while(counter<500000){
            ++counter;
        }
        *((unsigned int *)0x400253FCU) = 0x00U;
        *((unsigned int *)0x400253FCU) = 0x08U;
         counter=0;
         while(counter<500000){
             ++counter;
         }
         *((unsigned int *)0x400253FCU) = 0x00U;
         *((unsigned int *)0x400253FCU) = 0x0aU;
         counter=0;
         while(counter<500000){
              ++counter;
         }
         *((unsigned int *)0x400253FCU) = 0x00U;
         *((unsigned int *)0x400253FCU) = 0x0cU;
          counter=0;
          while(counter<500000){
              ++counter;
          }
          *((unsigned int *)0x400253FCU) = 0x00U;
          *((unsigned int *)0x400253FCU) = 0x0eU;
           counter=0;
           while(counter<500000){
               ++counter;
           }
        *((unsigned int *)0x400253FCU) = 0x00U;
        counter=0;

    }
}
