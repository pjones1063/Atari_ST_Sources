#if  !defined( __IN__ )
#define __IN__

/* well-defined IP protocols */
#define IPPROTO_IP  0
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
/* not supported: */
#define IPPROTO_ICMP  1
#define IPPROTO_RAW 255
#define IPPROTO_MAX IPPROTO_RAW

#define IS_INET_PROTO(p) \
/* Socket address, internet style */
typedef struct
{
	int						sin_family;
	int						sin_port;
	unsigned long	sin_addr;
	char					sin_zero[8];
}sockaddr_in;

/* options for use with [s|g]etsockopt' call at the IPPROTO_IP level */
#endif