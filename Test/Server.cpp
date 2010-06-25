#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

#define debug printf

int main()
{
    int fd = socket(PF_INET,SOCK_STREAM, 0);
    struct sockaddr addr = {AF_INET, {2101>>8, 2101&0xff, 127,0, 0, 1}};
    int ret = connect(fd, &addr, sizeof(addr));

    write(fd, 
    "SOURCE password/mnt\r\nSource Agent: NTRIP 1.0 precision-gps\r\n\r\n", 62);
 
    char buf[128];
    int actual = read(fd, buf, sizeof(buf)-1);
    buf[actual] = '\0';
    debug("Server: got %s\n", buf);

    for (;;) {
        debug("Server: sending 'Howdy'\n");
        for (int i=0; i<200; i++)
            write(fd, "Howdy", 5);
        sleep(1);
    }

    close(fd);
}
