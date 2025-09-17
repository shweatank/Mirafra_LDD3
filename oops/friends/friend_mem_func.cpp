#include<iostream>
using namespace std;
class XYZ;
class ABC 
{
	int data1;
	public:ABC()
	       {
		       cout<<"ABC constructor"<<endl;
		       cout<<"enter data1:"<<endl;
		       cin>>data1;
	       }
	       void print()
	       {
		       cout<<"data1:"<<data1<<endl;
	       }
	       ~ABC()
	       {
		       cout<<"destructor"<<endl;
	       }
	       void sum(XYZ &);
};
class XYZ
{
	int data2;
	public:XYZ()
	       {
		       cout<<"XYZ constructor"<<endl;
		       cout<<"enter data2:"<<endl;
		       cin>>data2;
	       }
	       void print()
	       {
		       cout<<"data2:"<<data2<<endl;
	       }
	       ~XYZ()
	       {
		       cout<<"destructor"<<endl;
	       }
	       friend void ABC::sum(XYZ &);
};
void ABC::sum(XYZ & obj)
{
	cout<<"sum:"<<data1+obj.data2;
}
int main()
{
	ABC obj1;
        obj1.print();
	XYZ obj2;
	obj2.print();
	obj1.sum(obj2);
}
