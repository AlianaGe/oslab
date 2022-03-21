#define __LIBRARY__
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

_syscall1(int, iam, const char*, name);
_syscall2(int, whoami,char*,name,unsigned int,size);

int main(int argc,char ** argv)
{
	// iam(argv[1]);
	char name[30]="AliAli";
	char after[30];
	int len1=iam(name);
	int len2=whoami(after,30);
	printf("%s,%d,%d",after,len1,len2);
	return 0;
}