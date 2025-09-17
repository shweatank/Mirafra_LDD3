#include<iostream>
using namespace std;

class comp
{
	int real,img;
	public:void input()
	       {
		       cout<<"enter real and img:"<<endl;
		       cin>>real>>img;
	       }
	       comp & operator+(comp &e)
	       {
		       static comp temp;
		       temp.real=real+e.real;
		       temp.img=img+e.img;
		       return temp;
	       }
	       comp & operator-(comp &e)
	       {
		       static comp temp;
		       temp.real=real-e.real;
		       temp.img=img-e.img;
		       return temp;
	       }

	       comp & operator*(comp &e)
	       {
		       static comp temp;
		       temp.real=real*e.real+(img * e.img*-1);
		       temp.img=(real*e.img)+(e.real *img);
		       return temp;
	       }
	       comp operator=(const comp &e)
	       {
		       if(this!=&e)
		       {
			       real=e.real;
		                img=e.img;
		       }
		       return *this;
	       }
	       void print()
	       {
	          cout<<real;
		  if(img>0)
		  cout<<"+";
		  cout<<img<<"j"<<endl;
	       }
	       comp & operator++()
	       {
		       ++real;
		       ++img;
		       return *this;
	       }
	       comp operator++(int)
	       {
		       comp temp=*this;
		       ++real;
		       ++img;
		       return temp;
	       }
};
int main()
{
    comp e1,e2,e3,e4,e5,e6;
    e1.input();
    e2.input();
    e3=e1+e2;
    e4=e1-e2;
    e5=e1*e2;
    cout<<"e1:"<<endl;
    e1.print();
    cout<<"e2:"<<endl;
    e2.print();
    cout<<"e3:"<<endl;
    e3.print();
    cout<<"e4:"<<endl;
    e4.print();  
    cout<<"e5:"<<endl;
    e5.print();
    cout<<"e6:"<<endl;
    e6=e1;
    e6.print();
    cout<<"e1:"<<endl;
    ++e1;
    e1.print();
    cout<<"e2:"<<endl;
    e2++;
    e2.print();

}
