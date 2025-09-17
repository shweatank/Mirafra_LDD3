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
        static void print()
	{
		cout<<"count:"<<cnt<<endl;
	}
};
int example::cnt=0;
int main()
{
	example e1("apple");
	example::print();
	example e2("grapes");
	example::print();
	example e3("bananas");
	example::print();
}
