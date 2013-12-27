#include <stdio.h>

void subtractandprint (int x, int y);

void subtractandprint (int x, int y) {
	int z = x - y;
	printf("The value of z is = %d\n",z);
}

int main () {
	void (*sap_ptr)(int, int) = subtractandprint;
	(*sap_ptr)(10, 2);
	sap_ptr(10, 2);
}
