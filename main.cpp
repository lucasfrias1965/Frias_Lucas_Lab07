#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

//taken from https://www.cppbetterexplained.com/posts/cpp-split-string/

std::vector<std::string> split(const std::string& s, char delim){
	std::vector<std::string> parts;
	std::stringstream ss(s);	
	std::string item;
	while (std::getline(ss, item, delim)){
		parts.push_back(item);
	}
	return parts;

}

std::vector<std::string> add_padding(std::string& a, std::string& b){
	//given two number strings with no extra chars, takes the length
	//and will calculate the padding needed by one to be char addable
	std::string padded_a = a;
	std::string padded_b = b;

	if (a.length() > b.length()){
		padded_b = std::string(a.length() - b.length(), '0') + b;
	}
	else if (b.length() > a.length()){
		padded_a = std::string(b.length() - a.length(), '0') + a;
	}

	std::vector<std::string> results;
	results.push_back(padded_a);
	results.push_back(padded_b);
	return results;
}

//a and b must already be the same length (e.g. the padded final_a/final_b).
//returns 1 if a's magnitude is bigger, -1 if b's is bigger, 0 if equal
int compare_magnitude(const std::string& a, const std::string& b){
	for (size_t i = 0; i < a.length(); i++){
		if (a[i] == '.') continue;
		if (a[i] != b[i]) return (a[i] > b[i]) ? 1 : -1;
	}
	return 0;
}

bool check_place_consistency(char * a){
		
	//get the address one after
	char * a_check = a + 1;

	if ( *a_check == '\0' ){
		if (*a >= '0' && *a <= '9') return true;
		else return false;
	}

	bool found_period = false;

	//check for weird edge cases
	if (a[0] == '.'){printf("ERR: can't start with .");return false;}
	//probably dead code with the int check but 
	if ( (a[0] == '+' || a[0] == '-') && !(a[1])){ printf("ERR: just a lone symbol is not a number"); return false;}
	char last_char = a[0]; //last char seen
	
	while (*a_check){ //while a check is not null term

		switch (*a_check){
			case '+':
				printf("ERR: + occurs in weird place "); return false;
			case '-':
				printf("ERR: - occurs in weird place "); return false;
			case '.':
				//we can only have one period
				if (found_period){
					printf("ERROR: more than one period"); return false;
				}
				else {
					char prev = *(a_check - 1);
					if (!(prev >= '0' && prev <= '9')){
						printf("ERR: decimal preceded by nothing");
						return false;
					}
					found_period = true;
				}
		}
		//set last char to last char seen
		last_char = *a_check;
		a_check++;//increment char_pointer
	}
	
	//also checks for 5. or when we have A., where some decimal doesn't have a 10^-1 degree

	if (last_char == '.'){printf("ERR: cannot end with a .");return false;}

	return true;
}


void eval_statement(char * statement){
	//first we need to cut the statement
	//into two strings.
	//we "cut" them by making two buffer
	//that are zero init that are at least the
	//size of statement (being overgenerous 
	//but doesn't matter
	size_t statement_size = strlen(statement);

	//first, let's check to make sure this is
	//all just spaces, periods, plus, negative, and numbers
	//any char otherwise is bad

	for (char * st_check = statement; *st_check; *st_check++){

		if (*st_check >= '0' && *st_check <= '9') continue;
		if (*st_check == ' ') continue;
		if (*st_check == '.') continue;
		if (*st_check == '+') continue;
		if (*st_check == '-') continue;
		printf("ERROR: not valid, can't do math with %s", statement);
		return;
	}

	//now we can size our buffers right
	char a [statement_size] = {0};
	char b [statement_size] = {0};
	
	//okay now we go through statement till we get
	//our space char ' ' and then we switch to b
	for (int i = 0; *statement != ' ' && *statement; i++){
		a[i] = *statement;
		statement++;
	}

	//skip the space deliminator	

	statement++;

	//now we switch to b and fill it
	for (int i = 0; *statement != ' ' && *statement; i++){
		b[i] = *statement;
		statement++;
	}
	
	//are there still extra numbers? let's check (excluding whitespace
	while (*statement){
		if (*statement == ' '){statement++; continue;}
		printf("ERR: more than one value");
		return;
	}
		
	//boom. we now have two char buffers
	//now let's check them for consistency
	//first char ought to be a + or - or num
	//and nowhere else, so let's increment &a+1
	//additionally, should be one period so we'll keep 
	//track of the periods we find

	bool a_ok = check_place_consistency(a);
	if (!a_ok) return;
	bool b_ok = check_place_consistency(b);
	if (!b_ok) return;

	//okay now we have to pad the strings. we are going to switch to std::string 
	//just for this because of how painful char * would be

	std::string cpp_a = std::string(a);
	std::string cpp_b = std::string(b);
	
	//now we have two tracks on padding this string,
	//if it's a double or integer on either side

	bool a_is_dob = (cpp_a.find('.') != std::string::npos);
	bool b_is_dob = (cpp_b.find('.') != std::string::npos);

	bool a_is_negative = (cpp_a.find('-') != std::string::npos);
	bool b_is_negative = (cpp_b.find('-') != std::string::npos);

	//remove all + and - chars
	cpp_a.erase(std::remove(cpp_a.begin(), cpp_a.end(), '+'), cpp_a.end());
	cpp_b.erase(std::remove(cpp_b.begin(), cpp_b.end(), '+'), cpp_b.end());
	cpp_a.erase(std::remove(cpp_a.begin(), cpp_a.end(), '-'), cpp_a.end());
	cpp_b.erase(std::remove(cpp_b.begin(), cpp_b.end(), '-'), cpp_b.end());

	std::string final_a;
	std::string final_b;
	
	if (!a_is_dob && !b_is_dob ){
		//then we have two integers and we can do
		//a simple len calc
		std::vector<std::string> result = add_padding(cpp_a, cpp_b);
		final_a = result[0];
		final_b = result[1];
	}

	else {
		//one of these strings is a decimal
		//check to make sure they both have a decimal

		if (!a_is_dob) cpp_a = cpp_a + ".0";
		if (!b_is_dob) cpp_b = cpp_b + ".0";

		//okay now we calculate seperately the scaling
		//of either part, a_big, and a_small (same
		//exists for b) we scale it up, and then we add back
		std::vector<std::string> a_vec = split(cpp_a, '.'); 
		if (a_vec.size() != 2) printf("ERR: split failed");
		std::vector<std::string> b_vec = split(cpp_b, '.');
		if (b_vec.size() != 2) printf("ERR: split failed");
	
		std::string a_big = a_vec[0];		
		std::string b_big = b_vec[0];
		std::string a_small = a_vec[1];
		std::string b_small = b_vec[1];
		
		std::vector<std::string> big_parts = add_padding(a_big, b_big);	
		std::vector<std::string> little_parts = add_padding(a_small, b_small);	

		final_a = big_parts[0] + '.' +  little_parts[0];
		final_b = big_parts[1] + '.' + little_parts[1];
			
	}
	printf("%s\n", final_a.c_str());
	printf("%s\n", final_b.c_str());

	char buffer_result [256] = {0};

	int string_i = final_a.length() - 1;
	int buff_i = 0;

	//same sign -> straight addition of magnitudes, result keeps that sign.
	//different sign -> it's really a subtraction, so figure out which
	//operand is bigger in magnitude: that one "wins" and lends its sign,
	//and we subtract the smaller magnitude from it with borrowing.
	bool same_sign = (a_is_negative == b_is_negative);
	bool result_is_negative;
	const std::string * big;
	const std::string * small;

	if (same_sign){
		result_is_negative = a_is_negative;
		big = &final_a;
		small = &final_b;
	}
	else {
		int cmp = compare_magnitude(final_a, final_b);
		if (cmp >= 0){
			result_is_negative = a_is_negative;
			big = &final_a;
			small = &final_b;
		}
		else {
			result_is_negative = b_is_negative;
			big = &final_b;
			small = &final_a;
		}
	}

	if (same_sign){
		int carry = 0;
		for (; string_i >= 0; string_i--){
			if (final_a[string_i] == '.') continue;
			int a_at_i = final_a[string_i] - '0';
			int b_at_i = final_b[string_i] - '0';
			int sum_res = (a_at_i + b_at_i + carry) % 10;
			carry = (a_at_i + b_at_i + carry) / 10;
			buffer_result[buff_i] = sum_res + '0';
			buff_i++;
		}
		if (carry) buffer_result[buff_i++] = carry + '0';
	}
	else {
		int borrow = 0;
		for (; string_i >= 0; string_i--){
			if ((*big)[string_i] == '.') continue;
			int big_at_i = (*big)[string_i] - '0' - borrow;
			int small_at_i = (*small)[string_i] - '0';
			if (big_at_i < small_at_i){
				big_at_i += 10;
				borrow = 1;
			}
			else {
				borrow = 0;
			}
			buffer_result[buff_i] = (big_at_i - small_at_i) + '0';
			buff_i++;
		}
	}

	//don't report a negative sign on a zero result
	bool result_is_zero = true;
	for (int chk = 0; chk < buff_i; chk++){
		if (buffer_result[chk] != '0'){ result_is_zero = false; break; }
	}
	if (result_is_zero) result_is_negative = false;

	if (result_is_negative) printf("-");
	for (buff_i = buff_i - 1; buff_i >= 0; buff_i--) printf("%c", buffer_result[buff_i]);
	printf("\n");
}


int main(){
	eval_statement("3751234 -345.3040323402");	
	return 0;
}
