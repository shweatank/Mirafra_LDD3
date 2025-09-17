#include<iostream>
using namespace std;
class rectangle
{
	float len,bre,area,peri;
	string name;
	public:rectangle(string name,int x,int y)
	       {
		       cout<<"iam in a parameterized contsructor"<<endl;
		       this->name=name;
		       len=x;
		       bre=y;
		       
	       }
	       rectangle(rectangle &obj)
	       {
		       cout<<"iam in a copy constructor"<<endl;
		       name=obj.name;
		       len=obj.len;
		       bre=obj.bre;
	       }
	       void cal_area(void)
	       {
		       area=len*bre;
	       }
	       void cal_peri(void)
	       {
		       peri=2*(len+bre);
	       }
	       void print()
	       {
		       cout<<"name:"<<name<<endl;
		       cout<<"len:"<<len<<endl;
		       cout<<"bre:"<<bre<<endl;
		       cout<<"area:"<<area<<endl;
		       cout<<"peri:"<<peri<<endl;
	       }
	       ~rectangle()
	       {
		       cout<<"iam in a destructor"<<endl;
	       }

};
int main()
{
	rectangle obj1("rectangle",10,20);
	rectangle obj2=obj1;
	obj1.cal_area();
	obj1.cal_peri();
	obj1.print();
	obj2.cal_area();
	obj2.cal_peri();
	obj2.print();
}
