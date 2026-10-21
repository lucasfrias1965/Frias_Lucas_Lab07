#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <cmath>

typedef struct {
        uint64_t value;
        bool is_int;
} lucas_number;




typedef struct {
        lucas_number value;
        bool error;
} ResultType;

static inline lucas_number unpack_result_type(ResultType rt){
    if (rt.error){printf("ERR, could not unpack"); exit(1);}
    else return rt.value;
}

ResultType make_lucas_number(char * c_str){
    //make_lucas_float -> given a c_str, we will
    //return a float in my stupid format.
    //this format is pretty much just me
    //implementing IEEE 754 because i'm 
    //a tryhard with too much time on his hands
    ResultType nothing_error = {lucas_number{0,0}, true};
    //quick check
    if (*c_str == '\0') {printf("ERR: nullstr");return nothing_error;}
 
    //the first thing we must do is get the sign!!
    //the sign is defined as the value that the string is in
    //it defaults to positive but to be negative MUST include
    //the - sign in the front. we also need to check the string
    //because we have to do crazy input validation

    char * sign_check_ptr = c_str;
    
    bool is_positive = true;
 
    //okay let's match the sign_check first char
    switch (*sign_check_ptr){
        case '+':
            break;
        case '-':
            is_positive = false;
            //falls through here it doesn't matter
        default:
            break;    
    }
    //okay now that we've determined the sign, let's check
    //and see if we violate the basic rule of where the sign is
    //while we're iterating, we're also gonna do a count
    //for if we have a period. if we have more than one period,
    //the string is in an invalid format too
   
    bool found_period = false; 
    for (;*sign_check_ptr != '\0'; sign_check_ptr++){
        switch (*sign_check_ptr){
            case '+':
                printf("ERR: string %s has extra +", c_str);
                return nothing_error;
            case '-':
                printf("ERR: string %s has extra +", c_str);
                return nothing_error;
            case '.':
                //if we already found a period then panic! at the disco
                //other +
                if (found_period){printf("ERR: more than one period in %s", c_str); return nothing_error;}
                else found_period = true;
            default:
                continue;                
        }
    }

    //is the string interger-esque? we can't know that it IS an
    //integer, but this decides what path we should take
    bool string_seems_inty = !found_period;
    if (string_seems_inty){
        //this is an integer validation/handling code let's parse it
        //first, let's make sure all chars are '0' - '9'


        //holy hideous one liner -> basically, what this does is inits a pointer which we increment and validate that
        //only integer types are actually returned so we're gonna validate whether this is okay
    
        for (char * num_check = c_str; *num_check != '\0'; num_check++) if ( !('0' <= *num_check && '9' >= *num_check) ) { printf("ERR: char %c is not a valid number", *num_check);return nothing_error;}
    
        //okay so now we validated it's like at least a number
        //so let's calculate it!!!

        //instead of manually incrementing through to get the size,
        //we're gonna use strlen to get the len, and then account for
        //if we have a + or - and remove 1 from that 
         
        const size_t number_size = (*c_str != '+' || *c_str != '-') ? strlen(c_str) : strlen(c_str)-1;

        //make our lucas number
        lucas_number integer_result = {0, true};

       	for (size_t i = number_size; *c_str != '\0'; i--){
		integer_result.value += pow(10, i-1) * (*c_str - '0');
		c_str++;
	}

        return ResultType{integer_result, false};

    }    

}
