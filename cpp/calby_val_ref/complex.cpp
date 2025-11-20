#include<iostream>
using namespace std;
class comp
{
	int real,img;
	public:comp()
	       {
		       real=img=0;
		       cout<<"default cons called"<<endl;
	       }
	       void input();
	       void print();
	       comp add(comp c1);
	       comp sub(comp &c1);
	       comp mul(comp &c1);
	       bool compare(comp &c1);
};
void comp::input()
{
	cout<<"enter real and img:"<<endl;
	cin>>real>>img;
}
void comp::print()
{
	cout<<real;
	if(img>=0)
		cout<<"+";
	cout<<img<<"j"<<endl;
}
comp comp::add(comp c1)
{
	comp temp;
	temp.real=real+c1.real;
	temp.img=img+c1.img;
	return temp;
}
comp comp:: sub(comp &c1)
{
	comp temp;
	temp.real=real-c1.real;
	temp.img=img-c1.img;
	return temp;
}
comp comp:: mul(comp &c1)
{
	comp temp;
	temp.real=(real*c1.real)+(img*c1.img*-1);
	temp.img=(real*c1.img)+(img*c1.real);
	return temp;
}
bool comp :: compare(comp &c1)
{
	if((real==c1.real) && (img ==c1.img))
		return true;
	return false;
}
int main()
{
	comp c1,c2,c3,c4,c5;
	c1.input();
	c2.input();
	c3=c1.add(c2);
	c4=c1.sub(c2);
	c5=c1.mul(c2);
	c1.print();
	c2.print();
	c3.print();
	c4.print();
	c5.print();
	if(c1.compare(c2))
		cout<<"both are same"<<endl;
	else
		cout<<"both are not same"<<endl;
}
