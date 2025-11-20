#include<iostream>
using namespace std;
int main()
{
	int *ptr;
	ptr=new int(50);
	cout<<"data:"<<*ptr<<endl;
	delete ptr;
	ptr=NULL;
}
