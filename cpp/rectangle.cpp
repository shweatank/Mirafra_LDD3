#include<iostream>
using namespace std;
class rect
{
	int len,bre,area,peri;
	public:rect()
	       {
		       cout<<"rect cons"<<endl;
	       }
	       void input()
	       {
		       cout<<"enter len and bre:"<<endl;
		       cin>>len>>bre;
	       }
	       void cal_area()
	       {
		       area=len*bre;
	       }
	       void cal_peri()
	       {
		       peri=2*(len+bre);
	       }
	       void print()
	       {
		       cout<<"len:"<<len<<" bre:"<<bre<<endl;
		       cout<<"area:"<<area<<" peri:"<<peri<<endl;
	       }
	       ~rect()
	       {
		       cout<<"rect des"<<endl;
	       }
};
int main()
{
	rect obj;
	obj.input();
	obj.cal_area();
	obj.cal_peri();
	obj.print();
}
