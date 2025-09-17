#include<iostream>
using namespace std;
class sample
{
	static int cnt;
	public:sample()
	       {
		       cnt++;
	       }
	      void print()
	       {
		       cout<<"cnt:"<<cnt;
	       }
	       ~sample()
	       {
		       cout<<"destructor"<<endl;
	       }
};
int sample::cnt=0;
int main()
{
	sample e1,e2,e3;
	e1.print();

	e3.print();
}
