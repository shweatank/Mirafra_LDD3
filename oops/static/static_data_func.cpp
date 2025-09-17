#include<iostream>
using namespace std;
class example
{
	string name;
	static int cnt;
	public:example(string s)
	{
		cout<<"contructor"<<endl;
		name=s;
		cnt++;
	}
       ~example()
	{
		cout<<"destructor"<<endl;
	}
	void print()
	{
		cout<<"count:"<<cnt<<endl;
		cout<<"name:"<<name<<endl;
	}
};
int example::cnt=0;
int main()
{
	example e1("apple");
	e1.print();
	example e2("grapes");
	e2.print();
	example e3("bananas");
	e3.print();
}
