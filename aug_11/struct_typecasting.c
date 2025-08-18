/*#include<stdio.h>
#include<string.h>
struct data
{
	char a;
	int num;
	char msg[10];
};

int main()
{
	struct data d;
	char *buf="H123hellobye4";

       strncpy(d.str,buf,2);
	d.str[2]='\0';
	strncpy(d.str2,buf+2,5);
	d.str2[6]='\0';
	strncpy(d.str3,buf+7,3);
	d.str3[3]='\0';
	printf("str:%s\nstr2:%s\nstr3:%s\n",d.str,d.str2,d.str3);

} 
*/
#include <stdio.h>
#include <string.h>

struct Data {
    char msg[100];
    int age;
    char id;
};

int main() {
    char buffer[] = "hello how are you 34 a";
    struct Data d;
    char temp[100]; // temporary to hold msg before number

    // Step 1: Separate words manually until we reach number
    int i = 0, j = 0;
    while (buffer[i] != '\0' && !(buffer[i] >= '0' && buffer[i] <= '9')) {
        d.msg[j++] = buffer[i++];
    }
    d.msg[j] = '\0';

    // Step 2: Read age and id from remaining string
    sscanf(&buffer[i], "%d %c", &d.age, &d.id);

    // Step 3: Trim trailing spaces in message
    int len = strlen(d.msg);
    while (len > 0 && d.msg[len - 1] == ' ')
        d.msg[--len] = '\0';

    // Print results
    printf("Message: \"%s\"\n", d.msg);
    printf("Age: %d\n", d.age);
    printf("ID: '%c'\n", d.id);

    return 0;
}
