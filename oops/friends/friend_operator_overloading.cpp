#include<iostream>
using namespace std;
class com
{
	int real,img;
	public:void input()
	       {
		       cout<<"enter real and img:"<<endl;
		       cin>>real>>img;
	       }
	       void print()
	       {
		       cout<<real;
		       if(img>0)
                        cout<<"+";
		       cout<<img<<"j"<<endl;
	       }

com &operator=(const com &e1)
{
	if(this !=&e1)
	{
		real=e1.real;
		img=e1.img;

	}
	return *this;

}
	       friend com  operator+(com &,com &);
	       friend com  operator-(com &,com &);
	       friend com  operator*(com &,com &);
	       friend com & operator++(com &);
	       friend com & operator++(com &,int);
	       friend com & operator+(int,com &);
};

com operator+(com &e1,com &e2)
{
	com temp;
	temp.real=e1.real+e2.real;
	temp.img=e1.img+e2.img;
	return temp;
}
com operator-(com &e1,com &e2)
{
	 com temp;
	temp.real=e1.real-e2.real;
	temp.img=e1.img-e2.img;
	return temp;
}
com operator*(com &e1,com &e2)
{
	 com temp;
	temp.real=(e1.real*e2.real)+(e1.img *e2.img*(-1));
	temp.img=(e1.img*e2.real)+(e1.real *e2.img);
	return temp;
}

com &operator++(com &e1)
{
	++e1.real;
	++e1.img;
	return e1;
}
com &operator++(com &e1,int)
{
	static com temp;
	temp.real=e1.real++;
	temp.img=e1.img++;
	return temp;

}
/*com&operator+(com &e1,int x)
{
        static com temp;
        temp.real=e1.real+x;
        temp.img=e1.img+0;
        return temp;


}*/
com&operator+(int x,com &e1)
{
        static com temp;
        temp.real=x+e1.real;
        temp.img=0+e1.img;
        return temp;

}

int main()
{
	com e1,e2,e3,e4,e5,e6,e7,e8,e9;
	e1.input();
	e2.input();
	e3=e1+e2;
	e4=e1-e2;
	e5=e1*e2;
	e6=++e1;
	e7=e2++;
	e8=10+e1;
	e9=e1;
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
	e6.print();
	cout<<"e7:"<<endl;
	e7.print();
	cout<<"e8:"<<endl;
	e8.print();
	cout<<"e9:"<<endl;
	e9.print();
}
