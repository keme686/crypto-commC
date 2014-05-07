#include<stdio.h>
#include "../field.h"

int main(){

uint8_t a = 5;
uint8_t b =3;

 uint8_t c = field_add(a, b);
 printf("c= %d \n", c);

 
return 0;
}
