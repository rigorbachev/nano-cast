#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int urand(int min, int max);
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

    // Generate a sequence of integers
    static const int BufSize = 4192;
    int j=0;  int random = urand(1, BufSize);
    for (unsigned char i = 0; ; i++) {

        // Fill in a random size buffer
        unsigned char buf[BufSize];
        if (j >= random) {
            write(fd, buf, j*sizeof(buf[0]));
            j = 0;

            // Wait a random time before proceeding
            usleep( urand(1, 1000000) );
            random = urand(1, BufSize);
        }
 
        buf[j++] = i;
    }

    close(fd);
}



double drand()
{
    return (double)random() / ((unsigned long)RAND_MAX+1);
}


int urand(int min, int max)
{
    return min +  (int) ( drand()*(max - min + 1) );
}
