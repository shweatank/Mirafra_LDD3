#include<iostream>
using namespace std;
class A
{
	protected:int x;
	public:A()
	       {
		       cout<<"A CONSTRUCTOR"<<endl;
		       cout<<"enter x value:"<<endl;
		       cin>>x;
	       }
	       ~A()
	       {
		       cout<<"A destructor"<<endl;
	       }
};
class B:virtual public A
{
	public:B()
	       {
		       cout<<"B CONSTRUCTOR"<<endl;   
	       }
	       ~B()
	       {
		       cout<<"B destructor"<<endl;
	       }
};
class C:virtual public A
{
	public:C()
	       {
		       cout<<"C CONSTRUCTOR"<<endl;
	       }
	       ~C()
	       {
		       cout<<"C destructor"<<endl;
	       }
};

class D:public B,public C
{
	public:D()
	       {
		       cout<<"D CONSTRUCTOR"<<endl;
	       }
	       void display()
	       {
		       cout<<"x value:"<<x<<endl; //no ambiguity with virtual
	       }
	       ~D()
	       {
		       cout<<"D destructor"<<endl;
	       }
};

int main()
{
	D obj;
	obj.display();
}
