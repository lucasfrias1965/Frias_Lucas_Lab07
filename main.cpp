#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


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
		printf("ERROR: not valid");
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
	char * a_check = a + 1;
	bool found_period = false;
	
	//quick gotcha -> make sure we don't have a .23 or something
	if (a[0] == '.'){printf("ERR: can't start with .");return;}
	char last_char = a[0];
	while (*a_check){
		switch (*a_check){
			case '+':
				printf("ERR: + occurs in weird place "); return;
			case '-':
				printf("ERR: - occurs in weird place "); return;
			case '.':
				if (found_period){
					printf("ERROR: more than one period"); return;
				}
				else {
					found_period = true;
				}
		}
		last_char = *a_check;
		a_check++;
	}
	
	//also checks for 5.

	if (last_char == '.'){printf("ERR: cannot end with a .");return;}

		
	
}


int main(){
	
}
