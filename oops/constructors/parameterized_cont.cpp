#include<iostream>
using namespace std;
class rectangle
{
	float len,bre,area,peri;
	string name;
	public:rectangle(string name,int x,int y)
	       {
		       cout<<"iam in a parameterized contsructor"<<endl;
		       cout<<"len:"<<len<<endl;
		       this->name=name;
		       len=x;
		       bre=y;
		       
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
	rectangle obj("rectangle",10,20);
	obj.cal_area();
	obj.cal_peri();
	obj.print();
}
