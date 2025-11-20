#include<iostream>
using namespace std;
class arr
{
	int *ptr,size;
	public:arr()
	       {
		       cout<<"default cons"<<endl;
		       cout<<"enter size:"<<endl;
		       cin>>size;
		       ptr=new int[size];
		       for(int i=0;i<size;i++)
		       {
			       cout<<"enter elements:"<<endl;
			       cin>>ptr[i];
		       }
	       }
	       arr(int *p,int s)
	       {
		       cout<<"parametrised cons"<<endl;
		       size=s;
		       ptr=new int[size];
		       for(int i=0;i<size;i++)
			       ptr[i]=p[i];
	       }
	       void display()
	       {
		       for(int i=0;i<size;i++)
			       cout<<ptr[i]<<" ";
		       cout<<endl;
	       }
	       void sort()
	       {
		       int j,key;
		       for(int i=1;i<size;i++)
		       {
			       j=i-1;
			       key=ptr[i];
			       while(j>=0 && ptr[j]>key)
			       {
				       ptr[j+1]=ptr[j];
				       j--;
			       }
			       ptr[j+1]=key;
		       }
	       }
};
int main()
{
	int a[5]={5,2,8,6,3};
	arr a1(a,5);
	a1.display();
	arr a2=a1;
	a2.display();
	a2.sort();           //here changing only a2 but a1 also changed bcz of shallow copy
	a1.display();
	a2.display();
}
