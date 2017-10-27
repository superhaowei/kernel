#include <stdlib.h>  
#include <stdio.h>  
#if 0
#include <unistd.h>  
#include <sys/stat.h>  
#include <fcntl.h> 
#endif
int main(int argc, char *argv[])
{
#if 1
   while(1){
     printf("Hello world!\n");    
     sleep(2);
   }  
#else
     fflush(stdout);  
     setvbuf(stdout,NULL,_IONBF,0);  
     printf("test stdout\n");  
     int save_fd = dup(STDOUT_FILENO); // 保存标准输出 文件描述符 注:这里一定要用 dup 复制一个文件描述符. 不要用 = 就像是Winodws下的句柄.  
     int fd = open("test1.txt",(O_RDWR | O_CREAT), 0644);  
     dup2(fd,STDOUT_FILENO); // 用我们新打开的文件描述符替换掉 标准输出  
     printf("test file\n");  
       
     //再恢复回来标准输出. 两种方式  
     //方法1 有保存 标准输出的情况  
     //dup2(save_fd,STDOUT_FILENO);  
       
     //方法2 没有保存 标准输出的情况  
      int ttyfd = open("/dev/tty",(O_RDWR), 0644);  
     dup2(ttyfd,STDOUT_FILENO);  
     printf("test tty\n");  
#endif
   return 0;
}

