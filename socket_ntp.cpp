
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "socket.h"
#if 0
using namespace std;

#define LOGD   printf

#define MAXLINE 1024

#define JAN_1970      0x83aa7e80U

/*
 *  用于64位NTP时间戳，即小数部分为32位
 *      NTP_CONV_FRAC32(x)将x转换为NTP时间戳中的小数部分值；
 *      NTP_REVE_FRAC32(x)则相反，将NTP时间戳小数部分值x解析成具体值。
 */
#define NTP_CONV_FRAC32(x)  (uint64_t) ((x) * ((uint64_t)1<<32))    
#define NTP_REVE_FRAC32(x)  ((double) ((double) (x) / ((uint64_t)1<<32))) 


/*
 *  用于32位NTP时间戳，即小数部分为16位
 *      NTP_CONV_FRAC16(x)将x转换为NTP时间戳中的小数部分值；
 *      NTP_REVE_FRAC16(x)则相反，将NTP时间戳小数部分值x解析成具体值。
 */
#define NTP_CONV_FRAC16(x)  (uint32_t) ((x) * ((uint32_t)1<<16))    
#define NTP_REVE_FRAC16(x)  ((double)((double) (x) / ((uint32_t)1<<16)))  


/*
 *  timeval结构中tv_usec字段和NTP时间戳小数部分互转
 */
#define USEC2FRAC(x)        ((uint32_t) NTP_CONV_FRAC32( (x) / 1000000.0 )) 
#define FRAC2USEC(x)        ((uint32_t) NTP_REVE_FRAC32( (x) * 1000000.0 )) 

/*
 *  将l_fixedpt结构（NTP64位时间戳）换算成自1970-01-01所经过的秒数。
 *  该l_fixedpt结构的字段均为网络字节序。
 */
#define NTP_LFIXED2DOUBLE(x)    ((double) ( ntohl(((struct l_fixedpt *) (x))->intpart) - JAN_1970 + FRAC2USEC(ntohl(((struct l_fixedpt *) (x))->fracpart)) / 1000000.0 ))   


typedef struct s_fixedpt
{
    uint16_t intpart;
    uint16_t fracpart;
}_s_fixedpt;

typedef struct l_fixedpt
{
    uint32_t intpart;
    uint32_t fracpart;
}_l_fixedpt;

typedef struct ntphdr
{
    unsigned char ntp_li:2;
    unsigned char ntp_vn:3;
    unsigned char ntp_mode:3;

    uint8_t  ntp_strstum;
    uint8_t  ntp_poll;
    int8_t   ntp_precision;

    _s_fixedpt ntp_rtdelay;
    _s_fixedpt ntp_rtdispersion;
    uint32_t ntp_refid;

    _l_fixedpt ntp_refts;
    _l_fixedpt ntp_orits;
    _l_fixedpt ntp_recvts;
    _l_fixedpt ntp_transts;
}_ntphdr;








int main(int argc, char** argv)
{
    int fd = 0;
    int n = 0;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        LOGD("opne socket faile fd = %d\n", fd);
        return -1;
    }

    LOGD("_ntphdr len is %d\n", sizeof(_ntphdr));
    

    char sendBuf[48] = {0};
    sendBuf[0] = 0xa3;
    char recvline[MAXLINE] = {0};
    recvline[0] = 0xa3;

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(123);
    servaddr.sin_addr.s_addr = inet_addr("120.24.166.46");
    LOGD("before sendto\n");
    n = sendto(fd, sendBuf, 48, 0, (struct sockaddr*)&servaddr,sizeof(servaddr));
    LOGD("after sendto n = %d\n", n);

    memset(recvline, 0, sizeof(recvline));
    int len = 0;
    n = recvfrom(fd, recvline, MAXLINE, 0, (struct sockaddr*)&servaddr, (socklen_t*)&len);
    LOGD("n = %d\n", n);
    LOGD("after recvfrom\n");
    for (size_t i = 0; i < n; i++)
    {
       LOGD(" %x", recvline[i]);
    }


    LOGD("\n");
    
 

    struct tm *tblock;

    long tmit=ntohl((time_t)recvline[4]); 
   
    tmit -= JAN_1970;
    std::cout << "time is " << ctime(&tmit)  << std::endl;
    const time_t i=time(0);
    printf("i: %d\n",i);
    tblock = localtime(&i);
    printf("Local time is: %s",asctime(tblock));

    const time_t t = (tmit);
      tblock = localtime(&t);
       printf("net time is: %s",asctime(tblock));

    std::cout << "System time is " << (i-tmit) << " seconds off" << std::endl;

    _ntphdr *ntp = (_ntphdr*)recvline;
    time_t x = ntp->ntp_transts.intpart - JAN_1970;
    x += 8*60*60;
    tblock = localtime(&x);
    printf("6666Local time is: %s",asctime(tblock));

    LOGD("\n");
  
    
    close(fd);
    
    LOGD("hello, world\n");
    

    return 0;
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include<sys/time.h>
#include <endian.h>

#define VERSION_3           3
#define VERSION_4           4

#define MODE_CLIENT         3
#define MODE_SERVER         4


#define NTP_LI              0
#define NTP_VN              VERSION_3   
#define NTP_MODE            MODE_CLIENT
#define NTP_STRATUM         0
#define NTP_POLL            4
#define NTP_PRECISION       -6

#define NTP_HLEN            48

#define NTP_PORT            123
#define NTP_SERVER          "182.92.12.11"

#define TIMEOUT             10

#define BUFSIZE             1500

#define JAN_1970            0x83aa7e80

#define NTP_CONV_FRAC32(x)  (uint64_t) ((x) * ((uint64_t)1<<32))    
#define NTP_REVE_FRAC32(x)  ((double) ((double) (x) / ((uint64_t)1<<32)))   

#define NTP_CONV_FRAC16(x)  (uint32_t) ((x) * ((uint32_t)1<<16))    
#define NTP_REVE_FRAC16(x)  ((double)((double) (x) / ((uint32_t)1<<16)))    


#define USEC2FRAC(x)        ((uint32_t) NTP_CONV_FRAC32( (x) / 1000000.0 )) 
#define FRAC2USEC(x)        ((uint32_t) NTP_REVE_FRAC32( (x) * 1000000.0 )) 


#define NTP_LFIXED2DOUBLE(x)    ((double) ( ntohl(((struct l_fixedpt *) (x))->intpart) - JAN_1970 + FRAC2USEC(ntohl(((struct l_fixedpt *) (x))->fracpart)) / 1000000.0 ))   


struct s_fixedpt {
    uint16_t    intpart;
    uint16_t    fracpart;
};

struct l_fixedpt {
    uint32_t    intpart;
    uint32_t    fracpart;
};


struct ntphdr {
#if __BYTE_ORDER == __BID_ENDIAN
    unsigned int    ntp_li:2;
    unsigned int    ntp_vn:3;
    unsigned int    ntp_mode:3;
#endif
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int    ntp_mode:3;
    unsigned int    ntp_vn:3;
    unsigned int    ntp_li:2;
#endif
    uint8_t         ntp_stratum;
    uint8_t         ntp_poll;
    int8_t          ntp_precision;
    struct s_fixedpt    ntp_rtdelay;
    struct s_fixedpt    ntp_rtdispersion;
    uint32_t            ntp_refid;
    struct l_fixedpt    ntp_refts;
    struct l_fixedpt    ntp_orits;
    struct l_fixedpt    ntp_recvts;
    struct l_fixedpt    ntp_transts;
};


in_addr_t inet_host(const char *host)
{
    in_addr_t saddr;
    struct hostent *hostent;

    if ((saddr = inet_addr(host)) == INADDR_NONE) {
        if ((hostent = gethostbyname(host)) == NULL)
            return INADDR_NONE;

        memmove(&saddr, hostent->h_addr, hostent->h_length);
    }

    return saddr;
}


int get_ntp_packet(void *buf, size_t *size)
{
    struct ntphdr *ntp;
    struct timeval tv = {0};


    if (!size || *size<NTP_HLEN)
        return -1;

    memset(buf, 0, *size);

    ntp = (struct ntphdr *) buf;
    ntp->ntp_li = NTP_LI;
    ntp->ntp_vn = NTP_VN;
    ntp->ntp_mode = NTP_MODE;
    ntp->ntp_stratum = NTP_STRATUM;
    ntp->ntp_poll = NTP_POLL;
    ntp->ntp_precision = NTP_PRECISION;

    gettimeofday(&tv, NULL);
    ntp->ntp_transts.intpart = htonl(tv.tv_sec + JAN_1970);
    ntp->ntp_transts.fracpart = htonl(USEC2FRAC(tv.tv_usec));

    *size = NTP_HLEN;

    return 0;
}





double get_rrt(const struct ntphdr *ntp, const struct timeval *recvtv)
{
    double t1, t2, t3, t4;

    t1 = NTP_LFIXED2DOUBLE(&ntp->ntp_orits);
    t2 = NTP_LFIXED2DOUBLE(&ntp->ntp_recvts);
    t3 = NTP_LFIXED2DOUBLE(&ntp->ntp_transts);
    t4 = recvtv->tv_sec + recvtv->tv_usec / 1000000.0;

    return (t4 - t1) - (t3 - t2);
}


double get_offset(const struct ntphdr *ntp, const struct timeval *recvtv)
{
    double t1, t2, t3, t4;

    t1 = NTP_LFIXED2DOUBLE(&ntp->ntp_orits);
    t2 = NTP_LFIXED2DOUBLE(&ntp->ntp_recvts);
    t3 = NTP_LFIXED2DOUBLE(&ntp->ntp_transts);
    t4 = recvtv->tv_sec + recvtv->tv_usec / 1000000.0;

    return ((t2 - t1) + (t3 - t4)) / 2;
}

int main(int argc, char *argv[])
{
    char buf[BUFSIZE];
    size_t nbytes;
    int sockfd, maxfd1;
    struct sockaddr_in servaddr;
    fd_set readfds;
    struct timeval timeout = {0};
    struct timeval recvtv = {0};
    struct timeval tv = {0};
    double offset;

    struct ntphdr ttt;

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(NTP_PORT);
    servaddr.sin_addr.s_addr = inet_host("120.24.166.46");
    // servaddr.sin_addr.s_addr = inet_host(NTP_SERVER);
    

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        exit(-1);
    }

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) != 0) {
        perror("connect error");
        exit(-1);
    }

    nbytes = BUFSIZE;
    if (get_ntp_packet(buf, &nbytes) != 0) {
        fprintf(stderr, "construct ntp request error \n");
        exit(-1);
    }
    send(sockfd, buf, nbytes, 0);


    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    maxfd1 = sockfd + 1;

    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    if (select(maxfd1, &readfds, NULL, NULL, &timeout) > 0) {
        if (FD_ISSET(sockfd, &readfds)) {
            if ((nbytes = recv(sockfd, buf, BUFSIZE, 0)) < 0) {
                perror("recv error");
                exit(-1);
            }

            gettimeofday(&recvtv, NULL);
            offset = get_offset((struct ntphdr *) buf, &recvtv);

            gettimeofday(&tv, NULL);
            tv.tv_sec += (int) offset;
            tv.tv_usec += offset - (int) offset;

            printf("%s \n", ctime((time_t *) &tv.tv_sec));
            printf("%lu \n", tv.tv_sec);
        }
    }

    close(sockfd);

    return 0;
}
