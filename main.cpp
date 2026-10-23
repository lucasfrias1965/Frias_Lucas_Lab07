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

//i guessss we have to add padding with a fraction, which kinda sucks 
//because we have to do a seperate function.
//basically we can't prepend for the small_art we have to append
//so that 0.2 because 0.20

std::vector<std::string> add_padding_fraction(std::string& a, std::string& b){
	std::string padded_a = a;
	std::string padded_b = b;

	//literally the same logic as the other just flipped
	if (a.length() > b.length()){
		padded_b = b + std::string(a.length() - b.length(), '0');
	}
	else if (b.length() > a.length()){
		padded_a = a + std::string(b.length() - a.length(), '0');
	}

	std::vector<std::string> results;
	results.push_back(padded_a);
	results.push_back(padded_b);
	return results;
}

//a and b must already be the same length because we padded dawg
//so this is like qsorts void * cmp function where we see which one is bigger
int compare_magnitude(const std::string& a, const std::string& b){
	//for the strings length (both are the same)
	for (size_t i = 0; i < a.length(); i++){
		//skip the period
		if (a[i] == '.') continue;
		//okay if we get any sort of char mismatch, we
		//compare which one is bigger in this here 
		//ternary statement. because chars are just
		//numbers, 0 is at 48 and 9 is whatever 58 or whatever
		//basically we just are seeing the difference in digits because
		//itll scale up
		//we return 1 for a, -1 for b
		if (a[i] != b[i]) return (a[i] > b[i]) ? 1 : -1;
	}
	//they're the same picture --pam
	//return 0
	return 0;
}

//checks that a token is a well-formed number: only digits/./+/-,
//at most one leading sign, at most one period, and not leading or
//trailing with a period. same rules as the old check_place_consistency,
//just as a plain predicate over one token instead of printing as it goes
bool is_valid_number(const std::string& s){
	if (s.empty()) return false;

	for (char c : s){
		bool allowed_char = (c >= '0' && c <= '9') || c == '.' || c == '+' || c == '-';
		if (!allowed_char) return false;
	}

	if (s.length() == 1) return (s[0] >= '0' && s[0] <= '9');

	if (s[0] == '.') return false;

	bool found_period = false;
	for (size_t i = 1; i < s.length(); i++){
		char c = s[i];
		if (c == '+' || c == '-') return false;
		if (c == '.'){
			if (found_period) return false;
			if (!(s[i - 1] >= '0' && s[i - 1] <= '9')) return false;
			found_period = true;
		}
	}

	if (s.back() == '.') return false;

	return true;
}

//strips the leading zeros that whole-number padding introduces and the
//trailing zeros that fractional padding introduces, and drops a sign
//that would otherwise show up on a zero result
std::string trim_result(const std::string& signed_value){
	bool negative = false;
	std::string value = signed_value;
	if (!value.empty() && value[0] == '-'){
		negative = true;
		value = value.substr(1);
	}

	std::string whole = value;
	std::string frac;

	size_t dot = value.find('.');
	if (dot != std::string::npos){
		whole = value.substr(0, dot);
		frac = value.substr(dot + 1);
	}

	size_t first_nonzero = whole.find_first_not_of('0');
	whole = (first_nonzero == std::string::npos) ? "0" : whole.substr(first_nonzero);

	size_t last_nonzero = frac.find_last_not_of('0');
	frac = (last_nonzero == std::string::npos) ? "" : frac.substr(0, last_nonzero + 1);

	bool is_zero = (whole == "0" && frac.empty());

	std::string result = (negative && !is_zero) ? ("-" + whole) : whole;
	if (!frac.empty()) result += "." + frac;
	return result;
}


void eval_statement(int line_number, const char * statement){
	printf("line %d: ", line_number);

	//split on whitespace and drop empty tokens from repeated spaces
	std::vector<std::string> tokens = split(std::string(statement), ' ');
	tokens.erase(std::remove(tokens.begin(), tokens.end(), ""), tokens.end());

	if (tokens.size() != 2){
		printf("ERROR: expected exactly two numbers\n");
		return;
	}

	const std::string& a_str = tokens[0];
	const std::string& b_str = tokens[1];

	bool a_valid = is_valid_number(a_str);
	bool b_valid = is_valid_number(b_str);

	if (!a_valid || !b_valid){
		printf("ERROR: invalid number");
		if (!a_valid) printf(" \"%s\"", a_str.c_str());
		if (!b_valid) printf(" \"%s\"", b_str.c_str());
		printf("\n");
		return;
	}

	//we are going to switch to std::string just for this
	//because of how painful char * would be

	std::string cpp_a = a_str;
	std::string cpp_b = b_str;

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
		std::vector<std::string> b_vec = split(cpp_b, '.');

		std::string a_big = a_vec[0];		
		std::string b_big = b_vec[0];
		std::string a_small = a_vec[1];
		std::string b_small = b_vec[1];
		
		std::vector<std::string> big_parts = add_padding(a_big, b_big);	
		//do seperate part for function
		std::vector<std::string> little_parts = add_padding_fraction(a_small, b_small);

		final_a = big_parts[0] + '.' +  little_parts[0];
		final_b = big_parts[1] + '.' + little_parts[1];
			
	}

	char buffer_result [256] = {0};

	int string_i = final_a.length() - 1;
	int buff_i = 0;

	//okay so this code is evil
	
	//there's two possible situations:
	//we have a same_sign (like (-, -) or (+, +) and we then basically just add magnitudes
	//ooorrrr we have a different sign, which then means we're doing subtraction and have to 
	//borrow which is gross :(

	bool same_sign = (a_is_negative == b_is_negative);
	bool result_is_negative = a_is_negative;

	if (same_sign){
		//this is the basic magnitude summing loop
		int carry = 0;//init carry to zero
		
		//while our string_i hasn't met the end (it's just the len of the str)
		for (; string_i >= 0; string_i--){
			//if we have a period just store it and move on

			if (final_a[string_i] == '.') {buffer_result[buff_i++] = '.'; continue;}
			
			//convert the char to an int
			int a_at_i = final_a[string_i] - '0';
			int b_at_i = final_b[string_i] - '0';

			//get the actual sum, which is the a, b and the carry we have modded by ten
			int sum_res = (a_at_i + b_at_i + carry) % 10;

			//if the carry is less than 10, this will be zero bc its int division
			//but if its NOT then it will be the carry amount left over

			carry = (a_at_i + b_at_i + carry) / 10;

			//set the buffer result to the char by doing + '0' (auto type conversion)
			buffer_result[buff_i] = sum_res + '0';
			buff_i++;//increment i
		}
		//if we still have a carry left, append it to the end...
		//we have Buff_i++ so we can increment it in the if 
		//and make it a one liner. cheesy!!
		if (carry) buffer_result[buff_i++] = carry + '0';
	}
	else {
		//okay this is the icky territory!! be warned

		//okay lets first see which is bigger
		int cmp = compare_magnitude(final_a, final_b);
		
		//now we know, so lets see if the result is negative	
		// >= 0 because if its a positive + a negative then its 0 
		// and 0 is neither, it has to be -1
		//only what the biggest number (in magnitudes) sign is, is what
		//compares it. like 123 compared to -345. |-345| > |123|
		//and therefore if -345 is negative the result is negative

		result_is_negative = (cmp >= 0) ? a_is_negative : b_is_negative;

		//okay now we determine which is the bigger in magnitude
		//and cast it to a string pointer, will be used in 
		//our subtract case		
		const std::string &big = (cmp >= 0) ? final_a : final_b;
		const std::string &small = (cmp >= 0) ? final_b : final_a;

		int borrow = 0; //set our borrow

		//same till the beginning of the string loop	
		for (; string_i >= 0; string_i--){
			//skip the period

			if (big[string_i] == '.') {buffer_result[buff_i++] = '.'; continue;}
			
			//get the a,b but we call it big, small

			//we sub from borrow because if we need to borrow a value
			//we need to subtract from the previpus part
			int big_at_i = big[string_i] - '0' - borrow;
			int small_at_i = small[string_i] - '0';

			//we have to borrow a value, like
			//when we do subtraction like
			// 70 <- here we gotta like
			//- 9  kinda pull what the value
			//---- is by subracting 1 from 7
			// 6 (0 + 10)
			//-     9
			//---------
			//and then 10-9 = 1
			//and then 6 - 0 = 6
			//so the answer is 61.

			if (big_at_i < small_at_i){
				//so therefore if the num that's smaller has a bigger value in this place
				//increment by 10
				big_at_i += 10;
				//set our borrow
				borrow = 1;
			}
			else borrow = 0;// otherwise clear our borrow
			
			//set the result as the big - small + '0' to scale to a char and increment
			buffer_result[buff_i] = (big_at_i - small_at_i) + '0';
			//increment buff_i++
			buff_i++;
		}
	}

	//buffer_result holds the magnitude digits in reverse order; flip it
	//back around and hand the signed string to trim_result, which strips
	//the padding zeros and decides whether the sign survives
	std::string raw_magnitude;
	for (int i = buff_i - 1; i >= 0; i--) raw_magnitude += buffer_result[i];

	std::string signed_result = (result_is_negative ? "-" : "") + raw_magnitude;

	printf("%s + %s = %s\n", a_str.c_str(), b_str.c_str(), trim_result(signed_result).c_str());
}


int main(){
	FILE * fptr = fopen("test.txt", "r");
	if (!fptr){
		printf("ERROR: could not open test.txt\n");
		return 1;
	}

	char str_buffer [1024] = {0};
	int str_buffer_i = 0;
	int line_number = 0;

	int current_c;

	do {
		current_c = fgetc(fptr);
		if (current_c == '\n'){
			str_buffer[str_buffer_i] = '\0';
			line_number++;
			eval_statement(line_number, str_buffer);
			str_buffer_i = 0;
			continue;
		}
		if (current_c != EOF){
			str_buffer[str_buffer_i] = (char)current_c;
			str_buffer_i++;
		}

	} while (current_c != EOF);

	fclose(fptr);
	return 0;
}
