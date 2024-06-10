#include <rt_sys.h>
#include "usart.h"
#ifdef __cplusplus
extern "C" {
#endif
 
extern int  sendchar(int ch);  
extern int  getkey(void);      
 
int fputc(int ch, FILEHANDLE *f) {
  return (sendchar(ch));
}
 
int fgetc(FILEHANDLE *f) {
  return (sendchar(getkey()));
}
 
FILEHANDLE _sys_open(const char * name, int openmode)
{
    return 1;  
}
int _sys_close(FILEHANDLE fh)
{
    return 0; //return success
}
int _sys_write(FILEHANDLE fh, const unsigned char * buf,
               unsigned len, int mode)
{
    return 0;   
}
int _sys_read(FILEHANDLE fh, unsigned char * buf,
              unsigned len, int mode)
{
    return 0;       
}
void _ttywrch(int ch)
{ 
}
int _sys_istty(FILEHANDLE fh)
{
    return 1; // no interactive device present
}
int _sys_seek(FILEHANDLE fh, long pos)
{
    return -1; // error
}
int _sys_ensure(FILEHANDLE fh)
{
    return 0; // success
}
long _sys_flen(FILEHANDLE fh)
{
    return 0;
}
int _sys_tmpnam(char * name, int sig, unsigned maxlen)
{
    return 0; // fail, not supported
}
void _sys_exit(int returncode)
{
    while(1) {};
}

/* 重定向输出流,可以使用cout了 */
int sendchar(int ch)
{
	if (ch == '\n') 
	{
    HAL_UART_Transmit(&huart1, (void *)"\r", 1,1000);
  }
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);
}
 
int getkey(void)
{
  uint8_t ch = 0;;
  HAL_UART_Receive(&huart1, &ch, 1,1000);
  return ch;
}

#ifdef __cplusplus
}
#endif
