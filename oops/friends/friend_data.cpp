#include<iostream>
using namespace std;
class sample
{
	int data1,data2;
	public:sample()
	       {
		       cout<<"constructor"<<endl;
		       cout<<"enter data1 and data2:"<<endl;
		       cin>>data1>>data2;
	       }
	       void print()
	       {
		       cout<<"data1:"<<data1<<endl;
		       cout<<"data2:"<<data2<<endl;
	       }
	       ~sample()
	       {
		       cout<<"destructor"<<endl;
	       }
	       friend void sum(sample &);

};
void sum(sample &obj)
{
	cout<<"sum:"<<obj.data1+obj.data2<<endl;
}
int main()
{
	sample obj;
	obj.print();
	sum(obj);
}
