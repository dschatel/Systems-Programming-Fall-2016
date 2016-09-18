/*
 * tokenizer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */
enum FSMState {State_0, State_1, State_2, State_3, State_4, State_5, State_6, State_7, State_8, State_9, State_10, MalState, EndState}; //Enum to hold the current state of the FSM

enum tokenType {Zero, Malformed, Octal, Hex, Float, Decimal}; // Enum to hold the current token type for purposes of output

struct TokenizerT_ {
	char *fullString;
	char *current;
	char *tokStart;
	enum FSMState currState;
	enum tokenType tokType;
	int tokLength;
};

typedef struct TokenizerT_ TokenizerT;

/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 * 
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate( char * ts ) {
	
	TokenizerT *token = malloc(sizeof(TokenizerT));
	
	//Dynamically allocate space for a copy of the command line string and copy it over
	token->fullString = malloc(sizeof (char) * strlen(ts));
	strcpy(token->fullString, ts); 
	
	token->current = token->fullString;
	token->tokStart = token->current;
	token->currState = State_0;
	token->tokLength = 0;
	token ->tokType = Malformed;

  return token;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy( TokenizerT * tk ) {
	free(tk->fullString);
	free(tk);
}


/*
If the current character is an octal number (0-7), returns 1. Otherwise returns 0
*/
int isOctal(char c) {
	if (isdigit(c) && c != '8' && c != '9')
		return 1;
	else
		return 0;
}

/*
Finds the initial state of the string from the first character. 0 sets FSM to state leading to Zero, Octal, Hex or Float. 1-9 sets FSM
to state leading to Decimal and Float. If the character is whitespace, recursively looks to the next character. If null, ends token.
Otherwise, returns a malformed token state.
*/
enum FSMState findState (TokenizerT *tok) {
	
	if (*tok->current == '0')
		return State_1;
	else if (isdigit(*tok->current) && *tok->current != '0')
		return State_2;
	else if (isspace(*tok->current)) {
		tok->current++;
		tok->tokStart++;
		return findState(tok);
	}
	else if (*tok->current == '\0')
		return EndState;
	else {
		return MalState;
	}
		
}


//Checks next character following a 0 character and sets state accordingly
enum FSMState FSMState_1 (TokenizerT *tok) {
	
	if(isOctal(*tok->current) == 1)
		return State_3;
	else if (*tok->current == 'x' || *tok->current == 'X')
		return State_4;
	else if (*tok->current == '.')
		return State_5;
	else {
		tok->tokType = Zero; 
		return EndState; 
	}
}

//Checks next character following a 1-9 character and sets state accordingly
//Pre-emptively sets token type to Decimal if moving on to Float state in case invalid
//characters follow
enum FSMState FSMState_2 (TokenizerT *tok) {
	
	
	if(isdigit(*tok->current))
		return State_2;
	else if (*tok->current == '.')
		return State_5;
	else if (*tok->current == 'e' || *tok->current == 'E') {
		tok->tokType = Decimal;
		return State_8;
	}
	else {
		tok->tokType = Decimal;
		return EndState;
	}
}

//Checks for further octal numbers
enum FSMState FSMState_3 (TokenizerT *tok) {
	
	if (isOctal(*tok->current) == 1)
		return State_3;
	else {
		tok->tokType = Octal;
		return EndState;
	}
}

//checks character following a hex 'x' or 'X' and sets state accordingly.
//If the character would result in an invalid token, 'walks back' the pointer and returns a Zero token
enum FSMState FSMState_4 (TokenizerT *tok) {
	
	if(isxdigit(*tok->current))
		return State_6;
	else {
		tok->current--;
		tok->tokLength--;
		tok->tokType = Zero; 
		return EndState;
	}
}

//Checks character following a decimal '.' and sets the state accordingly.
//If the character would result in an invalid token, 'walks back' the pointer and returns
//Either a Zero or Decimal token.
enum FSMState FSMState_5 (TokenizerT *tok) {
	
	if(isdigit(*tok->current))
		return State_7;
	else {
		tok->current--;
		tok->tokLength--;
		if (*tok->tokStart == '0' && tok->tokLength == 1)
			tok->tokType = Zero; 
		else
			tok->tokType = Decimal;
		return EndState;
	}
}

//Checks for further hex numbers
enum FSMState FSMState_6 (TokenizerT *tok) {
	
	if(isxdigit(*tok->current))
		return State_6;
	else {
		tok->tokType = Hex;
		return EndState;
	}
}

//Checks for further 0-9 digits following a decimal '.'
//Pre-emptively sets token type to Float in case characters following
//'E' or 'e' would result in an invalid token
enum FSMState FSMState_7 (TokenizerT *tok) {
	
	if(isdigit(*tok->current))
		return State_7;
	else if (*tok->current == 'e' || *tok->current == 'E') {
		tok->tokType = Float;
		return State_8;
	}
	else {
		tok->tokType = Float;
		return EndState;
	}
}

//Checks for valid float numbers following an 'E' or 'e'.
//If invalid character, 'walks back' pointer and returns a valid Decimal or Float
enum FSMState FSMState_8 (TokenizerT *tok) {
	
	if (*tok->current == '+' || *tok->current == '-')
		return State_9;
	else if(isdigit(*tok->current))
		return State_10;
	else {
		tok->current--;
		tok->tokLength--;
		return EndState;
	}
}

//Checks for valid float numbers following a '+' or '-'
//If invalid character, 'walks back' pointer and returns a valid Decimal or Float
enum FSMState FSMState_9 (TokenizerT *tok) {
	
	if(isdigit(*tok->current))
		return State_10;
	else {
		tok->current-=2;
		tok->tokLength-=2;
		return EndState;
	}
}

//Checks for further valid digits in a Float state
enum FSMState FSMState_10 (TokenizerT *tok) {
	
	if(isdigit(*tok->current))
		return State_10;
	else {
		tok->tokType = Float;
		return EndState;
	}
}

//Returns a single-character malformed token
enum FSMState FSMMalState (TokenizerT *tok) {
		tok->tokType = Malformed;
		return EndState;
		
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken( TokenizerT * tk ) {
	
	tk->tokStart = tk->current;
	tk->tokLength = 0;
	
	tk->currState = findState(tk);
	
	//Iterates through Finite State Machine by character and changes state accordingly.
	while(tk->currState != EndState) {
		
		tk->tokLength++;
		tk->current++;
		
		switch(tk->currState) {
			
			case State_1: tk->currState = FSMState_1(tk); break;
			case State_2: tk->currState = FSMState_2(tk); break;
			case State_3: tk->currState = FSMState_3(tk); break;
			case State_4: tk->currState = FSMState_4(tk); break;
			case State_5: tk->currState = FSMState_5(tk); break;
			case State_6: tk->currState = FSMState_6(tk); break;
			case State_7: tk->currState = FSMState_7(tk); break;
			case State_8: tk->currState = FSMState_8(tk); break;
			case State_9: tk->currState = FSMState_9(tk); break;
			case State_10: tk->currState = FSMState_10(tk); break;
			case MalState: tk->currState = FSMMalState(tk); break;
			default: break;
			
		}
	}
	
	//Creates a token by copying a substring from the original string and appends a null terminating character on the end
	if (tk->tokLength > 0) {
		char * token = malloc(tk->tokLength +1);
		strncpy(token, tk->tokStart, tk->tokLength);
		token[tk->tokLength] = '\0';
		return token;
	}
	
	return NULL;
	
}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {
	
	TokenizerT *token;
	char * word;
	
	token = TKCreate(argv[1]);
	
	while ((word = TKGetNextToken(token)) != NULL) {
		switch(token->tokType) {
			case Octal: printf("Octal %s\n", word); break;
			case Decimal: printf("Decimal %s\n", word); break;
			case Hex: printf("Hexadecimal %s\n", word); break;
			case Zero: printf("Zero %s\n", word); break;
			case Float: printf("Float %s\n", word); break;
			case Malformed: printf("ERROR: Malformed [0x%02x]\n", * word); break;
			default: printf("Incorrect Input!\n"); break;
		}
		
		free(word);
		token->currState = State_0;

	}
	
	TKDestroy(token);

  return 0;
}
