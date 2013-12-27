#include <stdio.h>

int subtract(int x, int y);


int subtract(int x, int y) {
	return x - y;
}


int main() {
	int (*sub_ret)(int, int) = subtract;

	int y = (*sub_ret)(10 , 3);
	printf("the value of y is =%d\n",y);

	int z = sub_ret(10, 3);
	printf("the value of z is =%d\n",z);
}

