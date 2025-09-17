#include<iostream>
using namespace std;
class A 
{
	public:int x;
	       A()
	       {
		       cout<<"A CONSTRUCTOR"<<endl;
		       cout<<"enter  X value:"<<endl;
		       cin>>x;
	       }
	       ~A()
	       {
		       cout<<" A destructor"<<endl;
	       }
};
class B:public A
{
	public:B()
	       {
		       cout<<"b constructor "<<endl;

	       }
	       ~B()
	       {
		       cout<<"B destructor"<<endl;
	       }
};
class C:public A
{
	public:C()
	       {
		       cout<<"c constructor"<<endl;
	       }
	       ~C()
	       {
		       cout<<"c destructor"<<endl;
	       }
};
class D:public B,public C
{
	public:D()
	       {
		       cout<<"d constructor"<<endl;
	       }
	       void display()
	       {
		      // cout<<"x:"<<x<<endl;//ambiguous
	                 cout<<"x from B:"<<B::x<<endl;
			 cout<<"x from C :"<<C::x<<endl;
	       }
	       ~D()
	       {
		       cout<<"d destructor"<<endl;
	       }
};
int main()
{
	D obj;
	obj.display();
}
