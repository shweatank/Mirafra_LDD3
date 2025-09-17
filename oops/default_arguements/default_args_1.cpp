#include<iostream>
using namespace std;
class sample
{
	public:sample()
	       {
		       cout<<"constructor"<<endl;

	       }
	       void print(string name="kusuma",int a=0,int b=10,int c=0)
	       {
		       cout<<"name:"<<name<<endl;
		       cout<<"a:"<<a<<endl<<"b:"<<b<<endl<<"c:"<<c<<endl;
	       }
	       ~sample()
	       {
		       cout<<"destructor"<<endl;
	       }

};
int main()
{
	sample obj;
	obj.print();
	obj.print("kusuma",10,20,30);
}

