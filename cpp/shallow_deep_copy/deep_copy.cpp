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
		       cout<<"enter elements"<<endl;
		       for(int i=0;i<size;i++)
			       cin>>ptr[i];
	       }
	       arr(const arr &a)
	       {
		       cout<<"copy constructor"<<endl;
		       size=a.size;
		       ptr=new int [size];
		       for(int i=0;i<size;i++)
			       ptr[i]=a.ptr[i];
	       }
	       void display()
	       {
		       for(int i=0;i<size;i++)
			       cout<<ptr[i]<<" ";
		       cout<<endl;
	       }
	       void sort()
	       {
		       int min,t;
		       for(int i=0;i<size-1;i++)
		       {
			       min=i;
			       for(int j=i+1;j<size;j++)
			       {
				       if(ptr[min]>ptr[j])
					       min=j;
			       }
			       if(i!=min)
			       {
				       t=ptr[i];
				       ptr[i]=ptr[min];
				       ptr[min]=t;
			       }
		       }
	       }
};
int main()
{
	arr obj1;
	arr obj2=obj1;
	obj1.display();
	obj2.display();
	obj2.sort();
	obj1.display();
	obj2.display();

}
