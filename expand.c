// Project 7: David Mendoza, Fall 2021 
// You can specify in a file A to include the contents of some other file B.
// File B may itself contain directives to include yet other txt and so on. The process of “file expansion”
// takes as input a file such as A and produces its ”expanded version.” 

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

// struct with stack method
typedef struct stack_struct {
  char** strings;  // pointer to array of strings
  int length;
  int size;  // size of array of strings
  int txt;
  int noChars;  // returns 0 or 1 if a char is present, 1 if there, 0 if not
  char* storage;
} STACK;


// default constructor for stack
STACK* create_stack(){
  STACK* stack = malloc(sizeof(STACK));
  stack->size = 4;
  stack->length = -1;
	stack->txt = -1;
  stack->strings = malloc(4 * sizeof(char*));
	stack->noChars = 0;	
  stack->storage = malloc(256 * sizeof(char));
  for (int i = 0; i < stack->size; i++){
     stack->strings[i] = NULL;
  }
  return stack;
}

// modification of quotation marks
int del_space(char* string, int spaces) {
  if (strlen(string) <= 7){
    return 0;
  }

	char* curStr = string;
  int temp = spaces;
  int first_quote = 0;  // starting point of comparison
  int starting = 0;  // quote mark is there
  int end = 0;
  int quote_marks = 0;
	for (int i = 0; i < strlen(string); i++) {
    if (string[i] != ' ') {
      if (string[i] == '"'){
        starting = 1;
        break;
      }
    }
    temp++;
  }

  if (string[strlen(string) - 1] == '"') {
    end = 1;
  }
	else if (string[strlen(string) - 2] == '"' ) {
		end = 1;
	}

  for (int k = temp; k < strlen(string) - spaces; k++) {
    if (k != temp && string[k] == '"') {
				end = 1;  // end of quoted string
				break;
    }
    curStr[first_quote] = string[k];
    first_quote++;  // position increases until " found
  }

  for (int j = first_quote; j < strlen(string); j++){
    curStr[j] = ' ';
  }

  if (end && starting) {
    quote_marks = 1;
  }

  memmove(curStr, &string[0], first_quote - spaces);
  curStr[first_quote] = '\0';  // pass as null
  string = curStr;
  return quote_marks;
}

// checks if file can be opened
int checkFile(char *str) {
  char *tempStr = str;
	// check the file named by the pathname pointed to by the path argument for accessibility 
  // existence test (F_OK).
  if(access(tempStr, F_OK) == 0) {
    // able to open the file
    return 1;
  } 
  // the file can't be opened
  return 0;
}

	// copies the string pointed by the stack
void check_error_file(char* string, STACK* s) {
  strcpy(s->storage, string);
}

// pops off an element in the stack, removing the front
void pop(STACK* s) {
  if (s->txt < 0) {  // no text
    return;
  }
  s->txt--;  // pop off a char
}

int isCopy(STACK* s, char* check) {
  for (int i = 0; i <= s -> txt; i ++) {
    if (!(strcmp(s->strings[i], check))) {
      return 1;
    }
  }
  return 0;
}

// pushes an element into the stack, implementation in this code
int push(STACK* s, char* string) {
  if (isCopy(s, string)) { 
    return 0;
  }

  // pushed 1 element, current text and the length of string increase by 1
  s->txt = s->txt + 1;  
  s->length = s->length + 1;

  // dynamically allocate the size of stack if at max capacity
	if (s->txt >= s-> size) {
   s->strings = realloc(s->strings, s->size * 2 * sizeof(char*));
   s->size *= 2;
   for (int i = s->txt + 1; i < s->size; i++) {
     s->strings[i] = NULL;  // set rest of file to null
   }
  }
  s->strings[s->txt] = string;  // = to current string
  return 1;
}

// checks empty
int check_empty(STACK* s) {
  return s->noChars;
}

// frees up memory allocated w/ stack
void clear(STACK* s) {
	free(s->storage);
  free(s->strings);
  free(s);
}

// helper fxn to close the file
void close_file(FILE* f, STACK* s, char* substr) {
  pop(s);
  fclose(f); 
}

// Reads the file as a "binary tree", if it failed to read the file, it returns 0, -1, or -2 if file reading fails otherwise it returns 1
int open_File(FILE* f, STACK* s) {
  char characters[257];  
	while (fgets(characters, 256, f) != NULL) {   
		if (characters[0] == '#' && characters[7] == 'e'){      
			char single_string[257];      
			memcpy(single_string, &characters[8], 256);
      single_string[256] = '\0';      
			int quo_mark = del_space(single_string, 1);      
			if ((!checkFile(single_string) || !quo_mark)) {
        strcpy(s -> storage, single_string);
        s->noChars = 1;
        if (!quo_mark){
          close_file(f, s, single_string);    
          return -1;
        }
				if (!checkFile(single_string)){
          close_file(f, s, single_string);    
          return -2;
        }
      }

      int inital_success = push(s, single_string);
      FILE* file = fopen(single_string, "r");
      
			if (inital_success != 1) {
        fclose(file);
        close_file(f, s, single_string);
        return inital_success;
      }

      int reading_success = open_File(file, s);
      
			if (reading_success != 1){
        close_file(f, s, single_string);
        return reading_success;
      }
    }
    // else, successful
    else {
      printf("%s", characters);
    }

  }
  pop(s);
  fclose(f);
  return 1;
}

char* get_trouble(STACK* s){
  if (!strcmp(s->storage, " ")){ 
    return "";
  }
  return s->storage;
}

int main(int argc, char* argv[]) {
    //declaration of variables for the array of char
    char* filename = "";
		// You may assume that each line in the filename txt has no more than 256 characters
		char characters[257];
		// to check if the variable is it passed
    int test_pass = -8; 
		
		// assigining each of the characters from the variable from the argument
    for (int i = 0; i < argc; i++){
			// assign into the filename
      filename = argv[i];
    }
    
		// opening the file for reading and The file must exist
    FILE* openFile = fopen(filename, "r");
    STACK* myStac = create_stack();
    
		// while file isnt null and the maximum amnt of characters is 256
    while (fgets(characters, 256, openFile) != NULL) {
      if (characters[7] == 'e' & characters[0] == '#'){
				// declaration of variable in a single character
        char single_string[257];
        memcpy(single_string, &characters[8], 256);
        single_string[256] = '\0';
        int quo_mark = del_space(single_string, 1);
				// if able to open the file
        if ((!checkFile(single_string) || !quo_mark)){ 
          check_error_file(single_string, myStac);
          if (!quo_mark){
            test_pass = -1;
            break;
          }

          if (!checkFile(single_string)){
            test_pass = -2;
            break;
          }
        }

        if (!strcmp(filename, single_string)){
          test_pass = 0;
          pop(myStac);
          break;
        }

        FILE* file = fopen(single_string, "r");
        push(myStac, single_string);
        test_pass = open_File(file, myStac);

        if (test_pass != 1) { 
					STACK* s;
          if (!check_empty(myStac)){
            check_error_file(single_string, myStac);
          }
          break;
        }
      }

      else{
        printf("%s", characters);
      }
    }

    if (test_pass == -1){
      fprintf(stderr, "**error: No file found after '#include'!\n");
			fprintf(stderr, "Expansion terminated.");
    }

    else if (test_pass == -2) {
     fprintf(stderr, "**error: File '%s' doesn't exist!\nExpansion terminated.", get_trouble(myStac));		 
    }

    else if (test_pass == 0) {
			fprintf(stderr, "**error: Infinite recursion detected!\n");
			fprintf(stderr, "Expansion terminated.");
    }

    clear(myStac);
    fclose(openFile);
}