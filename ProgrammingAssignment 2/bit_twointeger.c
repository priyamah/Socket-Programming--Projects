#include <stdio.h>
void isopposite(int x,int y);

int main(void) {
	int x,y;
	printf("Enter two numbers\n");
	scanf("%d %d",&x,&y);
	isopposite(x,y);
	return 0;
}
void isopposite(int x,int y){
	if((x^y)<0)
		printf("Both integers are opposite\n");
	else
		printf("Both integers are having same sign\n");
}
