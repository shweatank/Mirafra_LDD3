#include<iostream>
using namespace std;
class rectangle
{
	float len,bre,area,peri;
	public:rectangle()
	       {
		       cout<<"iam in contsructor"<<endl;
		       cout<<"enter len and bre"<<endl;
		       cin>>len>>bre;
		       
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
		       cout<<"len:"<<len<<endl;
		       cout<<"bre:"<<bre<<endl;
		       cout<<"area:"<<area<<endl;
		       cout<<"peri:"<<peri<<endl;
	       }

};
int main()
{
	rectangle obj;
	obj.cal_area();
	obj.cal_peri();
	obj.print();
}
