#include<iostream>
using namespace std;
class A 
{
	protected:int data1;
	          A()
		  {
			  cout<<"A constructor"<<endl;
			  cout<<"enter data1:"<<endl;
			  cin>>data1;
		  }
		  void display()
		  {
			  cout<<"data1:"<<data1<<endl;
		  }
		  ~A()
		  {
			  cout<<"A destructor"<<endl;
		  }
};
class B 
{
	
	protected:int data2;
	          B()
		  {
			  cout<<"B constructor"<<endl;
			  cout<<"enter data2:"<<endl;
			  cin>>data2;
		  }
		  void display()
		  {
			  cout<<"data2:"<<data2<<endl;
		  }
		  ~B()
		  {
			  cout<<"B destructor"<<endl;
		  }
};
class c:public A,B
{
	int res;
	public:c()
	       {
		       cout<<"c constructor"<<endl;
	       }
	       void sum()
	       {
		       res=data1+data2;
	       }
	       void show()
	       {
		      // display();//ambuguity problem 
		       A::display();
		       B::display();
		       cout<<"sum:"<<res<<endl;
	       }
	       ~c()
	       {
		       cout<<"c destructor"<<endl;
	       }
};
int main()
{
	c obj;
	obj.sum();
	obj.show();
}
