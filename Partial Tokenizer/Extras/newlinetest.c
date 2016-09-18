#include <stdio.h>

void strip(char *s) {
    char *p2 = s;
    while(*s != '\0') {
    	if(*s != '\t' && *s != '\n') {
    		*p2++ = *s++;
    	} else {
    		++s;
    	}
    }
    *p2 = '\0';
}

int main() {
    char buf[] = "0700\n1234";
    strip(buf);
    printf("%s\n", buf);
}