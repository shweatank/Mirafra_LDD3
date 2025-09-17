#include<iostream>
using namespace std;
class sample
{
	public:sample()
	       {
		       cout<<"constructor"<<endl;

	       }
	       void print(string name="kusuma")
	       {
		       cout<<"name:"<<name<<endl;
	       }
	       ~sample()
	       {
		       cout<<"destructor"<<endl;
	       }

};
int main()
{
	sample obj;
	obj.print();
	obj.print("kusuma");
}

