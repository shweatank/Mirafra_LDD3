#include<iostresdft6am>
using namespace std;
class st
{
	int id;
	string name;
	public:st()
	       {
		       id=1;
		       name="swarna";
		       cout<<"default cons"<<endl;
	       }
	       st(int a,string b)
	       {
		       id=a;
		       name=b;
		       cout<<"parameterised cons"<<endl;
	       }
	       st(const st &s)
	       {
		       id=s.id;
		       name=s.name;
		       cout<<"copy cons"<<endl;
	       }
	       void display()
	       {
		       cout<<"id:"<<id<<" name:"<<name<<endl;
	       }
	       ~st()
	       {
		       cout<<"des"<<endl;
	       }
};
int main()
{
	st a;
	a.display();
	st b(2,"bindu");
	b.display();
	st c=b;
	c.display();
}
