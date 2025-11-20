#include<iostream>
using namespace std;
class cal
{
	int op1,op2,res;
	public:cal()
	       {
		       cout<<"calc cons "<<endl;
	       }
	       void input()
	       {
		       cout<<"enter 2 operands:"<<endl;
		       cin>>op1>>op2;
	       }
	       void print()
	       {
		       cout<<"op1:"<<op1<<" op2:"<<op2<<endl;
		       cout<<"res:"<<res<<endl;
	       }
	       void add()
	       {
		       res=op1+op2;
	       }
	       void sub()
	       {
		       res= op1-op2;
	       }
	       void menu()
	       {
		       char ch;
		       cout<<"enter ch:"<<endl;
		       cin>>ch;
		       switch(ch)
		       {
			       case 'a':add();
					print();
					break;
			       case 's':sub();
					print();
					break;
		       }
	       }
};
int main()
{
	cal obj;
	obj.input();
	obj.menu();
}

