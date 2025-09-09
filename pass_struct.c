#include<stdio.h>
#include<string.h>
struct book
{
   char *title;
   char *author;
   int price;
};
void display(struct book *s)
{
    printf("%s\n",s->title);
    printf("%s\n",s->author);
    printf("%d\n",s->price);
}
int main()
{
   struct book b;
   b.title="embedded";
   b.author="larance";
   b.price=12109;
   struct book *ptr=&b;
   display(ptr);
}
