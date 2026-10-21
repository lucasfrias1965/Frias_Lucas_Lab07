#include <stdio.h>
#include "lucasfloat.cpp"

//hey zhe,
//due to my unconstrained malice and hatred for c++
//this lab does not use many c++ features
//thanks!!! 
//lucas frias


int main(){
    char * test = "01203\0";
    ResultType a = make_lucas_number(test);
    //we need to unpack my resulttype
    lucas_number b = unpack_result_type(a);   
    printf("\ntest value is%d\n", b.value);
    return 0;
}
