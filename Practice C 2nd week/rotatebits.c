#include<stdio.h>
int left(unsigned int num,int rot)
{
	rot=rot%32;
	if(rot==0)
		return num;
	return (num<<rot)|(num>>(32-rot));
}
int right(unsigned int num,int rot)
{
	rot=rot%32;
	if(rot==0)
		return num;
	return (num>>rot)|(num<<(32-rot));
}
void main()
{
	int num,rot;
	printf("Enter the number and rotations : ");
	scanf("%d%d",&num,&rot);
	printf("Leftrotate = %d\nRightrotate = %d\n",left(num,rot),right(num,rot));
}
