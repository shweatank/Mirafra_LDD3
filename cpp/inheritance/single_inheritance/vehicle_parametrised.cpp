#include<iostream>
using namespace std;
class vehicle
{
	string brand;
	public:vehicle(string b)
	       {
		       cout<<"parent constructor"<<endl;
		       brand=b;
	       }
	       void display()
	       {
		       cout<<"brand:"<<brand<<endl;
	       }
	       ~vehicle()
	       {
		       cout<<"parent destructor"<<endl;
	       }
};
class car:public vehicle
{
	string colour;
	public:car(string a,string b):vehicle(b)
	       {
		       cout<<"child constructor"<<endl;
		       colour=a;
	       }
	       void display()
	       {
		       vehicle::display();
		       cout<<"colour:"<<colour<<endl;
	       }
	       ~car()
	       {
		       cout<<"child destructor"<<endl;
	       }
};

int main()
{
	car obj("honda","black");
	obj.display();
}
