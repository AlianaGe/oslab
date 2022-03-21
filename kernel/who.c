#define __LIBRARY__                    /* 有它，_syscall1等才有效。详见unistd.h */
// #include <unistd.h>                /* 有它，编译器才能获知自定义的系统调用的编号 */
#include <errno.h>
#include <asm/segment.h>

char s[24];//23+'\0' 系统空间保存的共享数据
int len = 0;

// int 0x80 中断表中的函数分布在各个地方实现，最后一起都会一起链接起来
/*
将字符串参数name的内容拷贝到内核中保存下来。
要求name的长度不能超过23个字符。
返回值是拷贝的字符数。如果name的字符个数超过了23，则返回“-1”，并置errno为EINVAL。
*/
int sys_iam(const char * name){
	int i;
	for(i=0;i<24;++i){
		// 用户空间 => 系统空间
		s[i]=get_fs_byte(name);
		if(s[i]=='\0') break;
	}
	len=i;
	if(len<25) {
		// printk(s);
		printk("string=%s,len=%d",s,len);
		return len;
	}
	else {
		printk("too long");
		return -(EINVAL);
	}
}

/*
将内核中由iam()保存的名字拷贝到name指向的用户地址空间中，
同时确保不会对name越界访存（name的大小由size说明）。
返回值是拷贝的字符数。如果size小于需要的空间，则返回“-1”，并置errno为EINVAL。
*/
int sys_whoami(char* name, unsigned int size){
	int i;
	if(size<len) return -(EINVAL);
	for(i=0;i<len;++i){
		// 系统空间 => 用户空间
		put_fs_byte(s[i],name+i);
	}
	return len;
}