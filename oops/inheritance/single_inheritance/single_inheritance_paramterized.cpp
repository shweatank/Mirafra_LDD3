#include<iostream>
using namespace std;
class vehicle
{
	string brand;
	protected:vehicle(string b)
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
class car:protected vehicle
{
	string color;
	public:car(string a ,string b):vehicle(b)
	       {
		       cout<<"child constructor"<<endl;
	               color=a;
	       }
	       void display()
	       {
		       vehicle::display();
		       cout<<"color:"<<color<<endl;
	       }
	       ~car()
	       {
		       cout<<"child destructor"<<endl;
	       }
};
int main()
{
	car obj("toyota","red");//1st parent con after child automatically
	obj.display();//child display only
}
