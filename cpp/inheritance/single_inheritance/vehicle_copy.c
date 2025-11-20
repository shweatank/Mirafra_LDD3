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
	       vehicle(const vehicle &obj)
	       {
		       cout<<"parent copy cons"<<endl;
		       brand=obj.brand;
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
	       car(const car &obj):vehicle(obj)
		{
		       cout<<"child copy cons"<<endl;
		       colour=obj.colour;
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
	car obj1("black","honda");
	obj1.display();
	car obj2=obj1;
	obj2.display();
}
