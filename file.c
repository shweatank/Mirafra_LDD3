/*#include<stdio.h>
void main(){
char str[11]="hello world";
FILE *fp=fopen("data.txt","w+");
fwrite(str,sizeof(str),1,fp);
rewind(fp);
char s[11];
fread(s,sizeof(s),1,fp);
printf("%s\n",s);
fclose(fp);
}

*/

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
void main(){
char str[11]="hello world";

int fp=open("data.txt",O_RDWR);
write(fp,str,sizeof(str));

lseek(fp,0,SEEK_SET);

char s[11];
read(fp,s,sizeof(s));
printf("%s\n",s);

close(fp);

}

