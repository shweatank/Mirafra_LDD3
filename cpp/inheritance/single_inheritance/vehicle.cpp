#include<iostream>
using namespace std;
class vehicle
{
	string brand;
	public:vehicle()
	       {
		       cout<<"parent constructor"<<endl;
		       cout<<"enter brand:"<<endl;
		       cin>>brand;
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
	public:car()
	       {
		       cout<<"child constructor"<<endl;
		       cout<<"enter colour:"<<endl;
		       cin>>colour;
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
	car obj;
	obj.display();
}
