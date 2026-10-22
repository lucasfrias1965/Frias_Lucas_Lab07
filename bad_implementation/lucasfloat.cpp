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

static inline lucas_number set_bit(lucas_number value, uint8_t bit_pos, bool bool_val){
	return lucas_number {value.value | (bool_val << bit_pos), value.is_int};
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

    //check the special IEEE-754 strings before any sign/digit validation,
    //since they don't look like ordinary signed numbers at all
    if (!strcmp(c_str, "inf")){
        return ResultType {lucas_number{2047ULL << 52 | 0ULL << 63, false}, false};
    }
    if (!strcmp(c_str, "-inf")){
        return ResultType {lucas_number{(2047ULL << 52) | 1ULL << 63, false}, false};
    }
    if (!strcmp(c_str, "+0")){
        return ResultType{ lucas_number {0, false}, false};
    }
    if (!strcmp(c_str, "-0")){
        return ResultType{ lucas_number {0, false}, false};
    }
    if (!strcmp(c_str, "NaN")){
        //technically sNaN but makes no difference
        return ResultType{ lucas_number {2047ULL << 52 | 1ULL, false}, false};
    }

    //the first thing we must do is get the sign!!
    //the sign is defined as the value that the string is in
    //it defaults to positive but to be negative MUST include
    //the - sign in the front. we also need to check the string
    //because we have to do crazy input validation

 
    char * sign_check_ptr = c_str;
   
    bool is_positive = true;
    bool all_numbers = true; 
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
    

    //this is very important!! we have to increment the sign
   //check ptr
    sign_check_ptr++;


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
		break;
        }
    }

    //is the string interger-esque? we can't know that it IS an
    //integer, but this decides what path we should take
    bool string_seems_inty = !found_period;
    if (string_seems_inty && all_numbers){
        //this is an integer validation/handling code let's parse it
        //first, let's make sure all chars are '0' - '9'


        //holy hideous one liner -> basically, what this does is inits a pointer which we increment and validate that
        //only integer types are actually returned so we're gonna validate whether this is okay
    
        for (char * num_check = (*c_str == '+' || *c_str == '-') ? c_str + 1 : c_str; *num_check != '\0'; num_check++) if ( !('0' <= *num_check && '9' >= *num_check) ) { printf("ERR: char %c is not a valid number", *num_check);return nothing_error;}
    
        //okay so now we validated it's like at least a number
        //so let's calculate it!!!

        //instead of manually incrementing through to get the size,
        //we're gonna use strlen to get the len, and then account for
        //if we have a + or - and remove 1 from that 
         
        const size_t number_size = (*c_str != '+' &&  *c_str != '-') ? strlen(c_str) : strlen(c_str)-1;

        //make our lucas number
        lucas_number integer_result = {0, true};

	//skip the sign char itself, it's not a digit to accumulate
	if (*c_str == '+' || *c_str == '-') c_str++;

	//increment for every element in the size of our str (at least one long guarenteed)

       	for (size_t i = number_size; *c_str != '\0'; i--){
		//okay so while we have no null terminator

		integer_result.value += pow(10, i-1) * (*c_str - '0');
		//increment our value by the power of the position (based on the size of the integer)

		//increment the char pointer by 1
		c_str++;
	}

        return ResultType{integer_result, false};

    }
    else {
	//this is our double IEEE 756 implementation
	//basically, we are going to parse the string and have the data be a IEEE 754
	//specifically, double precision floating point format
	//i'm deriving all of this from this wikipedia article
	//https://en.wikipedia.org/wiki/Double-precision_floating-point_format
	lucas_number float_res = {0, false};

	//this does NOT use a standard lib function. this is a modified version of 
	//this code (stackoverflow.com/questions/43926665/converting-string-to-float-without-atof-in-c)
	//which i modified for double precision

	double res = 0, fact = 1;

	//the special-value strings (inf/-inf/+0/-0/NaN) are already handled at
	//the top of the function, before we ever get here

	//skip the sign char itself; is_positive was already determined earlier,
	//sign gets applied to res directly at the end instead of being smuggled
	//through fact (fact is only the fractional place-value scale now)
	if (!is_positive) c_str++;

	//this is aura, nullterm is 0 so it would be a false boolean
	//i'm writing all my null term checks like this from now on
	for (bool point_seen = 0; *c_str; c_str++){
		//okay if the pointer is seen then it's true, and we continue
		if (*c_str == '.'){
			//the point has been seen!!
			point_seen = true;
			continue;
		}

		//get the integer value and store it
		//
		int num_part = *c_str - '0';

		if (num_part >= 0 && num_part <= 9){
			if (point_seen){
				//if the point's been seen, divide the factor by 10 (it's the next
				//decimal place) and scale this digit by it: 0.1 -> 0.01 -> 0.001
				fact /= 10.0f;
				res += num_part * fact;
			} else {
				//no point yet, so we're still in the integer part: shift res up
				//and append the digit
				res = res * 10.0f + (double) num_part;
			}
		}
	}

	if (!is_positive) res = -res;

	//bit-copy the IEEE-754 double into the uint64_t field, not a numeric cast
	memcpy(&float_res.value, &res, sizeof(double));

	return ResultType{float_res, false};

    }

}
