#ifndef _YK_NET_H_
#define _YK_NET_H_
#include <features.h>
__BEGIN_DECLS
/*
 * inet:tcp
 */
#define YK_TCP_SYN       0x0001
#define YK_TCP_ACK       0x0002
#define YK_TCP_PSH       0x0004
#define YK_TCP_RST       0x0008
#define YK_TCP_URG       0x0010
#define YK_TCP_FIN       0x0020

#include <netinet/in.h>

struct tcp_datagram {
	int   len;           /* length of the tcp datagram */
	char  *data;         /* point to tcp datagram */
	char  *cache;        /* include psedu header + tcp datagram */
};

extern struct tcp_datagram *yk_tcp_make_datagram(
                         u_int32_t saddr,
                         u_int32_t daddr,
                         u_int16_t sport,
                         u_int16_t dport,
                         u_int32_t seq,
                         u_int32_t ack_seq,
                         int flags,
                         u_int16_t window,
                         u_int16_t urg_ptr,
                         const char *opts,
                         size_t optlen,
                         const char *data,
                         size_t datalen);
extern void                 yk_tcp_free_datagram(struct tcp_datagram *tdgram);
extern struct tcp_datagram *yk_tcp_fill_datagram(const char *pkt);
extern int                  yk_tcp_verify_datagram(struct tcp_datagram *dgram);
extern int                  yk_tcp_send_syn(int sock,
					const char *srcip, 
					const char *dstip,
			                u_int16_t sport,
					u_int16_t dport);


extern int                  yk_ip_send(const char *msg, size_t len, const char *dstip);


/*
 * Local route table management
 */
#include <net/route.h>
#define CENTOS  1
#define FEDORA  2
extern int                  yk_route_get(int ostype, struct rtentry **rt);
extern void                 yk_route_free(struct rtentry **rt, int count);


/*
 * Operations on neetwork interface card 
 */
char *yk_if_getaddr(const char *ifname);





__END_DECLS
#endif /* _YK_NET_H_ */
