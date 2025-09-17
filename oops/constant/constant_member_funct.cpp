#include<iostream>
using namespace std;
class constant
{
	int data1;
	const int data2;
	public:constant():data2(10)
	       {
		       cout<<"constructor"<<endl;
		       cout<<"enter data1:"<<endl;
		       cin>>data1;
	       }
	constant(int a,int b):data2(b)
	{
		cout<<"parameterized constutor"<<endl;
		data1=a;
	}
	constant(constant &obj):data2(obj.data2)
	{
		data1=obj.data1;
	}
	void print()
	{
		cout<<"data1:"<<data1<<endl;
		cout<<"data2:"<<data2<<endl;
	//	cout<<"data2:"<<data2++;//not allowed
	}
	//in const function members are not allowed to modify
	void inc()const
	{
		cout<<"data1:"<<data1++;
		cout<<"data2:"<<data2++;

	}
	~constant()
	{
		cout<<"destructor"<<endl;
	}
};
int main()
{
	constant obj1;
	obj1.print();
	constant obj2(10,20);
	obj2.print();
	constant obj3=obj1;
	obj3.print();
}

