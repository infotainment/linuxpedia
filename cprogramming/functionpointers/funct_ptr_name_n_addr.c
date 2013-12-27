#include <stdio.h>


void add(char *name, int x, int y);


void add(char *name, int x, int y) {
	printf("%s gives value of=%d\n",name, x + y);
}



int main() {
	
	void (*add1ptr)(char *, int, int) = add;
	void (*add2ptr)(char *, int, int) = *add;
	void (*add3ptr)(char *, int, int) = &add;
	void (*add4ptr)(char *, int, int) = **add;
	void (*add5ptr)(char *, int, int) = ***add;


	(*add1ptr)("add1ptr", 10, 2);
	(*add2ptr)("add2ptr", 11, 2);
	(*add3ptr)("add3ptr", 12, 2);
	(*add4ptr)("add4ptr", 13, 2);
	(*add5ptr)("add5ptr", 14, 2);


	add1ptr("addr1func", 10, 2);
	add2ptr("addr2func", 11, 2);
	add3ptr("addr3func", 12, 2);
	add4ptr("addr4func", 13, 2);
	add5ptr("addr5func", 14, 2);
	
}
