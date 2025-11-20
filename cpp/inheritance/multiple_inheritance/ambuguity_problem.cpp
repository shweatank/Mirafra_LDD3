#include<iostream>
using namespace std;
class A
{
	protected:int data1;
	public:A()
	       {
		       cout<<"A Constructor"<<endl;
		       cout<<"enter data1:"<<endl;
		       cin>>data1;
	       }
	       void display()
	       {
		       cout<<"data1:"<<data1<<endl;
	       }
	       ~A()
	       {
		       cout<<"A destructor"<<endl;
	       }
};
class B
{
	protected:int data2;
	public:B()
	       {
		       cout<<"B Constructor"<<endl;
		       cout<<"enter data2:"<<endl;
		       cin>>data2;
	       }
	       void display()
	       {
		       cout<<"data2:"<<data2<<endl;
	       }
	       ~B()
	       {
		       cout<<"B destructor"<<endl;
	       }
};
class C:public A,B
{
	int sum;
	public:C()
	       {
		       cout<<"C constructor"<<endl;
	       }
	       void cal_sum()
	       {
		       sum=data1+data2;
	       }
	       void show()
	       {
		       //display()   //ambuguity problem
		       A::display();
		       B::display();
		       cout<<"sum:"<<sum<<endl;
	       }
		~C()
	       {
		       cout<<"C destructor"<<endl;
	       }
};
int main()
{
	C obj;
	obj.cal_sum();
	obj.show();
}
