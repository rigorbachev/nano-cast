#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#define debug printf

int main()
{
    int fd = socket(PF_INET,SOCK_STREAM, 0);
    struct sockaddr addr = {AF_INET, {9999>>8, 9999, 127,0, 0,1}};
    int ret = connect(fd, &addr, sizeof(addr));

    write(fd, 
    "GET /mnt\r\nUser Agent: NTRIP 1.0 precision-gps\r\n\r\n", 49);
 
    char buf[13] = {0};
    int actual = read(fd, buf, 12);
    debug("Client: got %d bytes\n", actual);

    for (;;){
        char buf[1024];
        int len = read(fd, buf, sizeof(buf)-1);
        if (len <= 0) {
            printf("  len=%d  errno=%d\n", len, errno);
            break;
        }
        buf[len] = '\0';
        debug("len=%d  \n", len);
    }

    close(fd);
}
