#include<iostream>
using namespace std;
class vehicle
{
	string brand;
	protected:vehicle()
		  {
			  cout<<"parent constructor"<<endl;
			  cout<<"enter a brand:"<<endl;
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
class car:protected vehicle
{
	string color;
	public:car()
	       {
		       cout<<"child constructor"<<endl;
		       cout<<"enter car color:"<<endl;
		       cin>>color;
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
	car obj;//1st parent con after child automatically
	obj.display();//child display only
}
