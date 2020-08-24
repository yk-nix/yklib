
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#include "yk_lib.h"
#include "yk_net.h"
#include "yk_checksum.h"

struct pseudohdr {
	u_int32_t   source;
	u_int32_t   dest;
	u_int8_t    padding;
	u_int8_t    ptcl;
	u_int16_t   len;
}__attribute__((aligned(4)));

/*
 * Create a tcp datagram.
 * Return the pointer point to the created datagram, which 
 * must be free by the caller; return NULL if something wrong.
 */
struct tcp_datagram *yk_tcp_make_datagram(
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
			 size_t datalen) {
	struct tcp_datagram *dgram = NULL;
	int tcphdr_len = 0;
	int total_len = 0;
	char *cache    = NULL;
	char *cur      = NULL;
	struct pseudohdr *phdr = NULL;
	struct tcphdr    *hdr  = NULL;
	dgram = malloc(sizeof(struct tcp_datagram));
	if(dgram == NULL)
		goto err0;
	tcphdr_len = roundup(sizeof(struct tcphdr) + optlen, 4);
	total_len = roundup(tcphdr_len + datalen + sizeof(struct pseudohdr), 4);
	cache = malloc(total_len);
	if(cache == NULL)
		goto err1;
	memset(cache, 0, total_len);
	phdr = (struct pseudohdr *)cache;
	phdr->source = saddr;
	phdr->dest   = daddr;
	phdr->ptcl   = IPPROTO_TCP;
	phdr->len    = htons(tcphdr_len + datalen);
	hdr = (struct tcphdr *)(cache + sizeof(struct pseudohdr));
	hdr->source  = htons(sport);
	hdr->dest    = htons(dport);
	hdr->seq     = htonl(seq);
	hdr->ack_seq = htonl(ack_seq);
	hdr->window  = htons(window);
	hdr->doff    = tcphdr_len / 4;
	hdr->fin     = (YK_TCP_FIN & flags) ? 1 : 0;
	hdr->syn     = (YK_TCP_SYN & flags) ? 1 : 0;
	hdr->rst     = (YK_TCP_RST & flags) ? 1 : 0;
	hdr->psh     = (YK_TCP_PSH & flags) ? 1 : 0;
	hdr->ack     = (YK_TCP_ACK & flags) ? 1 : 0;
	hdr->urg     = (YK_TCP_URG & flags) ? 1 : 0;
	cur = cache + sizeof(struct pseudohdr) + sizeof(struct tcphdr);
	memcpy(cur, opts, optlen);
	cur += roundup(optlen, 4);
	memcpy(cur, data, datalen);
	hdr->check   = yk_checksum16(cache, total_len);
	dgram->len   = tcphdr_len + datalen;
	dgram->cache = cache;
	dgram->data  = (char*)hdr;
	return dgram;
err1:
	if(dgram)
		free(dgram);
err0:
	return NULL;
}

void yk_tcp_free_datagram(struct tcp_datagram *tdgram) {
	if(tdgram) {
		if(tdgram->cache)
			free(tdgram->cache);
		free(tdgram);
	}
}

/*
 * Get tcp datagram from IP packet
 */
struct tcp_datagram *yk_tcp_fill_datagram(const char *pkt) {
	struct iphdr *iphdr = NULL;
	struct tcp_datagram *dgram = NULL;
	struct pseudohdr *phdr = NULL;
	char *cache = NULL;
	int total_len = 0;
	int tcp_len = 0;
	if(pkt)
		goto err0;
	dgram = malloc(sizeof(struct tcp_datagram));
	if(dgram == NULL)
		goto err0;
	iphdr = (struct iphdr *)pkt;
	tcp_len = ntohs(iphdr->tot_len) - iphdr->ihl * 4;
	total_len = roundup(tcp_len + sizeof(struct pseudohdr), 4);
	cache = malloc(total_len);
	if(cache == NULL)
		goto err1;
	phdr = (struct pseudohdr *)cache;
	phdr->source = iphdr->saddr;
	phdr->dest   = iphdr->daddr;
	phdr->ptcl   = IPPROTO_TCP;
	phdr->len    = htons(tcp_len);
	memcpy(cache + sizeof(struct pseudohdr), pkt + iphdr->ihl * 4, tcp_len);
	dgram->cache = cache;
	dgram->data = cache + sizeof(struct pseudohdr);
	dgram->len  = tcp_len;
	return dgram;
err1:
	if(dgram)
		free(dgram);
err0:
	return NULL;
}


/*
 * Verify tcp checksum
 */
int yk_tcp_verify_datagram(struct tcp_datagram *dgram) {
	if(dgram == NULL)
		return 0;
	return !yk_checksum16(dgram->cache, dgram->len + sizeof(struct pseudohdr));
}

/*
 * Send syn request via PACKET socket.
 * Return how many bytes sent successfully, otherewise return -1.
 */
int yk_tcp_send_syn(int sock, const char *srcip, const char *dstip, 
		u_int16_t sport, u_int16_t dport) {
	int bytes = 0;
	struct tcp_datagram *tcp = NULL;
	tcp = yk_tcp_make_datagram(inet_addr(srcip),           /* source address */
				inet_addr(dstip),              /* destination address */
				sport,                         /* source port */
				dport,                         /* destination port */
				(u_int32_t)rand(),             /* sequence number */
				0,                             /* ACK sequence number */
				YK_TCP_SYN,                    /* SYN request */
				8192,                          /* window size */
				0,                             /* urgent pointer */
				NULL,                          /* optional data */
				0,                             /* length of optional data */
				NULL,                          /* data */
				0);                            /* length of data */
						
	if(tcp == NULL)
		return -1;
	bytes = yk_ip_send((void*)tcp->data, tcp->len, dstip);
	yk_tcp_free_datagram(tcp);
	return bytes;
}


