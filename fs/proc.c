#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/segment.h>
#include <linux/fs.h>
#include <stdarg.h>
#include <unistd.h>
extern int vsprintf(char *buf, const char *fmt, va_list args);
#define PAGE_SIZE 4096
#define set_bit(bitnr, addr) ({ \
register int __res ; \
__asm__("bt %2,%3;setb %%al":"=a" (__res):"a" (0),"r" (bitnr),"m" (*(addr))); \
__res; })
char tmp[PAGE_SIZE] = {'\0'};
/*proc_read(inode->i_zone[0], buf, count, &file->f_pos)*/
/*输出结果到字符串中,fmt==format*/
int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;
    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}

/* process info
 * pid    state    father    counter    start_time
 */
int get_psinfo()
{
    int bp = 0; /* buffer position*/
    bp += sprintf(tmp + bp, "%s", "pid\tstate\tfather\tcounter\tstart_time\n");

    int i, next, c;
    struct task_struct **p;

    for (p = &LAST_TASK; p > &FIRST_TASK; --p)
    {
        if (*p)
        {
            bp += sprintf(tmp + bp, "%d\t%d\t%d\t%d\t%d\n",
                          (*p)->pid, (*p)->state, (*p)->father, (*p)->counter, (*p)->start_time);
        }
    }
    return bp;
}

/*
total_blocks:62000;
free_blocks:39037;
used_blocks:22963;
total_inodes:20666;
*/
int get_hdinfo()
{
    struct super_block *sb;
    int bp = 0;
    int i, used;
    sb = get_super(0x301); /* super block */
    bp += sprintf(tmp + bp, "total_blocks:%d", sb->s_nzones);
    used = 0;
    i = sb->s_nzones;
    while (--i >= 0)
    {
        if (set_bit(i & 8191, sb->s_zmap[i >> 13]->b_data))
            used++;
    }
    bp += sprintf(tmp + bp, "free_blocks:%d\n", sb->s_nzones - used);
    bp += sprintf(tmp + bp, "used_blocks:%d\n", used);
    bp += sprintf(tmp + bp, "total_inodes:%d\n", sb->s_ninodes);
    return bp;
}

/*根据设备编号，把不同的内容写入到用户空间的buf。
写入的数据要从f_pos指向的位置开始，每次最多写count个字节，
并根据实际写入的字节数调整f_pos的值，最后返回实际写入的字节数。
当设备编号表明要读的是psinfo的内容时，就要按照psinfo的形式组织数据。*/
int proc_read(int dev, char *buf, int count, unsigned long *pos)
{
    int i;
    int bp = 0;
    if (dev == 0)
    {
        bp = get_psinfo();
    }
    if (dev == 1)
    {
        bp = get_hdinfo();
    }
    for (i = 0; i < bp; i++)
    {
        if (tmp[i + *pos] == '\0')
            break;
        put_fs_byte(tmp[i + *pos], buf + i + *pos);
    }
    *pos += i;
    return i;
}
