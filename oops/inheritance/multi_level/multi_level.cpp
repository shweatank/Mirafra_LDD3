#include<iostream>
using  namespace std;
class A
{
	public:string name;
	int age;
	A(string x,int y)
	{
		cout<<"A constructor"<<endl;
		name=x;
		age=y;
	}
	void display_A()
	{
		cout<<"name:"<<name<<endl;
		cout<<"age:"<<age<<endl;

	}
	~A()
	{
		cout<<"A destructor"<<endl;
	}
};
class B:public A
{
	public:float salary;
	B(string x,int y,float z):A(x,y)
	{
		cout<<"B constructor"<<endl;
		salary=z;
	}
	void display_B()
	{
		cout<<"salary:"<<salary<<endl;

	}
	~B()
	{
		cout<<"B destructor"<<endl;
	}
};
class C:public B
{
	
	public:string com_name;
	C(string a,string x,int y,float z):B( x, y,z)
	{
		cout<<"C constructor"<<endl;
		com_name=a;
	}
	void display_C()
	{
		cout<<"off_name:"<<com_name<<endl;

	}
	~C()
	{
		cout<<"C destructor"<<endl;
}
};
int main()
{
	C obj("mirafra","kusuma",22,30000);
        obj.display_A();
	obj.display_B();
	obj.display_C();
}
