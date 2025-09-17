#include<iostream>
using namespace std;
class addition
{
	public:addition()
	       {
		       cout<<"iam in a default constructor"<<endl;
	       }
	     void add(int a,int b)
	       {
		       cout<<"sum:"<<a+b<<endl;

	       }
	     void add(int a,float b,int c)
	       {
		       cout<<"sum:"<<a+b+c<<endl;
               }
	     void add(int a,int b,float c,float d)
	       {
		       cout<<"sum:"<<a+b+c+d<<endl;
               }
	     ~addition()
	     {
		     cout<<"im in a defualt constructor"<<endl;
	     }

};
int main()
{
	addition obj;
	obj.add(10,20);
	obj.add(10,20.20,30);
	obj.add(10,20,30.3,40.7);
}
