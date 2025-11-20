#include<iostream>
using namespace std;
class A
{
	protected:string name;
		  int age;
	public:A(string a, int b)
	       {
		       cout<<"A constructor"<<endl;
		       name=a;
		       age=b;
	       }
	       void A_display()
	       {
		       cout<<"name:"<<name<<endl;
		       cout<<"age:"<<age<<endl;
	       }
	       ~A()
	       {
		       cout<<"A Desstructor"<<endl;
	       }
};
class B:public A
{
	protected:float salary;
	public:B(float a,string b,int c):A(b,c)
	       {
		       cout<<"B constructor"<<endl;
		       salary=a;
	       }
	       void B_display()
	       {
		       A_display();
		       cout<<"salary:"<<salary<<endl;
	       }
	       ~B()
	       {
		       cout<<"B destructor"<<endl;
	       }
};
class C:public B
{
	protected:string com_name;
	public:C(string a,float b,string c,int d):B(b,c,d)
	       {
		       cout<<"C constructor"<<endl;
		       com_name=a;
	       }
	       void C_display()
	       {
		       B_display();
		       cout<<"com_name:"<<com_name<<endl;
	       }
	       ~C()
	       {
		       cout<<"C destructor"<<endl;
	       }
};
int main()
{
	C obj("mirafra",30000,"swarna",21);
	obj.C_display();
}
