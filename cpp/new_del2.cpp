#include<iostream>
using namespace std;
int main()
{
	int *ptr;
	ptr=new int[5];
	for(int i=0;i<5;i++)
	{
		cout<<"enter data:"<<endl;
		cin>>ptr[i];
	}
	for(int i=0;i<5;i++)
	{
		cout<<ptr[i]<<" ";
	}
	cout<<endl;
	delete[] ptr;
	ptr=NULL;
}
