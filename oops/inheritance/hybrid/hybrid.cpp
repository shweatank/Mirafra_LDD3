#include<iostream>
using namespace std;
class A 
{
	public:A()
	       {
	         cout<<"A constructor"<<endl;
	       }
	       ~A()
	       {
		       cout<<"A DESTRUCTOR"<<endl;
	       }
};
class B:public A
{
	public:B()
	       {
	         cout<<"B constructor"<<endl;
	       }
	       ~B()
	       {
		       cout<<"B DESTRUCTOR"<<endl;
	       }
};
class C
{
	public:C()
	       {
	         cout<<"C constructor"<<endl;
	       }
	       ~C()
	       {
		       cout<<"C DESTRUCTOR"<<endl;
	       }
};
class D:public B ,public C
{
	public:D()
	       {
	         cout<<"D constructor"<<endl;
	       }
	       ~D()
	       {
		       cout<<"D DESTRUCTOR"<<endl;
	       }
};
int main()
{
	D obj;
}
