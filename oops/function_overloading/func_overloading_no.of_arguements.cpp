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
	     void add(int a,int b,int c)
	       {
		       cout<<"sum:"<<a+b+c<<endl;
               }
	     void add(int a,int b,int c,int d)
	       {
		       cout<<"sum:"<<a+b+c+d<<endl;
               }
	     void add(int a,int b,int c,int d,int e)
	       {
		       cout<<"sum:"<<a+b+c+d+e<<endl;
               }

};
int main()
{
	addition obj;
	obj.add(10,20);
	obj.add(10,20,30);
	obj.add(10,20,30,40);
	obj.add(10,20,30,40,50);
}
