/*
 * Simple example program to log packets received with the ulog
 * watcher of ebtables.
 *
 * usage:
 * Add the appropriate ebtables ulog rule, e.g. (0 < NLGROUP < 33):
 *   ebtables -A FORWARD --ulog-nlgroup NLGROUP
 * Start this application somewhere:
 *   test_ulog NLGROUP
 *
 * compile with make test_ulog KERNEL_INCLUDES=<path_to_kernel_include_dir>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
//#include <asm/types.h>
//#include <sys/socket.h>
//#include <sys/time.h>
//#include <time.h>
//#include <linux/netlink.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <netdb.h>
//#include <errno.h>
//#include <netinet/if_ether.h>
//#include <netinet/ether.h>
//#include <netinet/ip.h>
//#include <netinet/ip_icmp.h>
//#include "ebtables/include/ebtables_u.h"
//#include "ebtables/include/ethernetdb.h"
//#include <linux/netfilter_bridge/ebt_ulog.h>


//mym 
#include "write_to_tap.h"
//#include "mac_list.h"

struct mac_list *list_top;
struct mac_list p;

char cmd_str[256];

int flags = IFF_TAP;
#define tap_test
#define broadcast_test
//#define PRINTF
//#define tap_test
#define DISTRIBUTE_PACKET
#define SIZE_ETHERNET 14
/* <linux/if_vlan.h> doesn't hand this to userspace :-( */
#define VLAN_HLEN 4

int mac_list_count(void);
int tun_alloc(char *dev, int flags);
struct mac_list * mac_list_check(char item[18]);
void mac_list_print(void);
void delete_tap();


struct mac_list{

	char mac_add[18];
	char tap_name[10];
	int tap_fd_num;

	struct mac_list *mac_list_next;
};

//init
void mac_list_init(){

	while(list_top != NULL){
		struct mac_list *delete_pos = list_top;
		list_top = list_top->mac_list_next;
		free(delete_pos);
	}

	delete_tap();

}

//next
struct mac_list * mac_list_next(struct mac_list * pos){

	if(pos != NULL){

		return pos->mac_list_next;
	}else{

		return MAC_LIST_TAIL;
	}
}



//top

struct mac_list * mac_list_top(void){

	return list_top;

}

//add
void mac_list_add(char item[18]){


	char tap_name[5];


	if(list_top == MAC_LIST_TAIL){

		system("openvpn --mktun --dev tap --user shohei");
		sprintf(cmd_str, "ip link set tap%d up", mac_list_count());
		system(cmd_str);

		sprintf(tap_name, "tap%d", mac_list_count());
		list_top = (struct mac_list *)malloc(sizeof(struct mac_list));


		strcpy(list_top->mac_add, item);
		strcpy(list_top->tap_name, tap_name);

		if ( (list_top->tap_fd_num = tun_alloc(tap_name, flags | IFF_NO_PI)) < 0 ) {
			printf("error:tun_alloc\n");
			exit(1);
		}


		list_top->mac_list_next = MAC_LIST_TAIL;

	}else{
		struct mac_list *p_temp = list_top;
		while(p_temp->mac_list_next != MAC_LIST_TAIL){


			if( strcmp(p_temp->mac_add, item) == 0){
				//printf("already have the item %s \n", item);
				return;
			}

			p_temp = mac_list_next(p_temp);
		}


		if(strcmp(p_temp->mac_add, item) == 0 ){
			return;
		}


		system("openvpn --mktun --dev tap --user shohei");
		sprintf(cmd_str, "ip link set tap%d up", mac_list_count());
		system(cmd_str);

		sprintf(tap_name, "tap%d", mac_list_count());

		p_temp->mac_list_next = (struct mac_list *)malloc(sizeof(struct mac_list));
		strcpy(p_temp->mac_list_next->mac_add, item);
		strcpy(p_temp->mac_list_next->tap_name, tap_name);

		if ( (p_temp->mac_list_next->tap_fd_num = tun_alloc(tap_name, flags | IFF_NO_PI)) < 0 ) {
			printf("error:tun_alloc\n");
			exit(1);
		}

		p_temp->mac_list_next->mac_list_next = MAC_LIST_TAIL;

	}
	mac_list_print();

}


int mac_list_count(void){

	int cnt=0;
	struct mac_list *p;

	p = list_top;
	while(p != MAC_LIST_TAIL){
		cnt++;
		p = p->mac_list_next;
	}

	return cnt;

}


void mac_list_print(void){

	struct mac_list *p;

	p = list_top;

	while(p != MAC_LIST_TAIL){
		printf("mac add | %s --> %s | fd num %d\n", p->mac_add, p->tap_name, p->tap_fd_num);
		p = p->mac_list_next;
	}

	printf("------------------------\n");
	return ;

}


struct mac_list * mac_list_check(char item[18]){


	if(list_top == MAC_LIST_TAIL){

		return list_top;

	}else{
		struct mac_list *p;
		p = list_top;

		while(p!= MAC_LIST_TAIL){

			if( strcmp(p->mac_add, item) == 0){
				return p;
			}
			
			p = mac_list_next(p);
		}

		return NULL;
	}

}

void delete_tap(){

	int tap_num =0;

	while(tap_num <= 50){
		sprintf(cmd_str, "ip link set tap%d down",tap_num);
		tap_num++;
	}

	return;

}



int write_packet_for_multicastadd(char item[18] , ebt_ulog_packet_msg_t *msg){

	char multicast_macadd_ipv4[6] = "1:0:5e";
	char multicast_macadd_ipv6[6] = "33:33:";
	char multicast_macadd_temp[6] = "";
	struct ip *ipv4_addr_temp = (struct ip *)(msg->data+SIZE_ETHERNET);
	struct ip6_hdr *ipv6_addr_temp = (struct ip6_hdr*)(msg->data);

	strncpy(multicast_macadd_temp, item, 6);


	if( strcmp(multicast_macadd_temp, multicast_macadd_ipv4) == 0 ){

		struct mac_list *p;
		p = list_top;

		while(p != MAC_LIST_TAIL){

			write(p->tap_fd_num, msg->data, ipv4_addr_temp->ip_len);
			p = p->mac_list_next;

		}

		return 1;

	}else if(strcmp(multicast_macadd_temp, multicast_macadd_ipv6) == 0 ){

		struct mac_list *p;
		p = list_top;

		while(p != MAC_LIST_TAIL){

			write(p->tap_fd_num, msg->data, ipv6_addr_temp->ip6_ctlun.ip6_un1.ip6_un1_plen);
			p = p->mac_list_next;
		}


		return 1;
	}else{


		return 0;
	}

}




int write_packet_for_broadcastadd(ebt_ulog_packet_msg_t *msg){

	char broadcast_macadd_ipv4[2] = "ff";
	char buf_src[2];
	char buf_dst[2];
	char temp_src[2] ="";
	char temp_dst[2] ="";
	struct ip *ipv4_addr_temp = (struct ip *)(msg->data+SIZE_ETHERNET);

	sprintf(buf_src,"%x",ipv4_addr_temp->ip_src.s_addr);
	sprintf(buf_dst,"%x",ipv4_addr_temp->ip_dst.s_addr);

//	printf("ip_src %s | ip_dst %s\n", buf_src, buf_dst);

	strncpy(temp_src, buf_src,2);
//	printf("%s\n" ,temp_src);


	strncpy(temp_dst, buf_dst,2);
//	printf("%s\n" ,temp_dst);

#ifdef broadcast_test
	if( strcmp(broadcast_macadd_ipv4, temp_src) == 0 || strcmp(broadcast_macadd_ipv4, temp_dst) == 0  ){


		struct mac_list *p;
		p = list_top;

		while(p != MAC_LIST_TAIL){

			write(p->tap_fd_num, msg->data, ipv4_addr_temp->ip_len);
			p = p->mac_list_next;

		}

		return 1;

	}else{
		return 0;
	}
#endif

}


int write_packet_for_broadcast_macadd(ebt_ulog_packet_msg_t *msg){

	//	struct ip6_hdr *ipv6_addr_temp = (struct ip6_hdr*)(msg->data);

	struct ip *ipv4_addr_temp = (struct ip *)(msg->data+SIZE_ETHERNET);

	struct mac_list *p;
	p = list_top;

	while(p != MAC_LIST_TAIL){

		write(p->tap_fd_num, msg->data, ipv4_addr_temp->ip_len);
		p = p->mac_list_next;

	}

	return 1;
}





int tun_alloc(char *dev, int flags){

	struct ifreq ifr;
	int fd, err;
	char *clonedev = "/dev/net/tun";

	if( (fd = open(clonedev , O_RDWR)) < 0 ) {
		perror("Opening /dev/net/tun");
		return fd;
	}

	memset(&ifr, 0, sizeof(ifr));

	ifr.ifr_flags = flags;

	if (*dev) {
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}

	if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
		perror("ioctl(TUNSETIFF)");
		close(fd);
		return err;
	}

	strcpy(dev, ifr.ifr_name);

	return fd;
}

struct vlan_hdr {
	unsigned short TCI;
	unsigned short encap;
};

static struct sockaddr_nl sa_local =
{
	.nl_family = AF_NETLINK,
	.nl_groups = 1,
};

static struct sockaddr_nl sa_kernel =
{
	.nl_family = AF_NETLINK,
	.nl_pid = 0,
	.nl_groups = 1,
};


static const char *hookstr[NF_BR_NUMHOOKS] =
{
	[NF_BR_POST_ROUTING] "POSTROUTING",
	[NF_BR_PRE_ROUTING] "PREROUTING",
	[NF_BR_LOCAL_OUT] "OUTPUT",
	[NF_BR_LOCAL_IN] "INPUT",
	[NF_BR_BROUTING] "BROUTING",
	[NF_BR_FORWARD] "FORWARD"
};


#define DEBUG_QUEUE 0
//#define BUFLEN 212992
#define BUFLEN 131072	
static char buf[BUFLEN];
static int sfd;

/* Get the next ebt_ulog packet, talk to the kernel if necessary */
ebt_ulog_packet_msg_t *ulog_get_packet()
{
	static struct nlmsghdr *nlh = NULL;
	static int len, remain_len;
	static int pkts_per_msg = 0;
	ebt_ulog_packet_msg_t *msg;
	socklen_t addrlen = sizeof(sa_kernel);

	if (!nlh) {
recv_new:
		if (pkts_per_msg && DEBUG_QUEUE)
#ifdef PRINTF
			printf("PACKETS IN LAST MSG: %d\n", pkts_per_msg);
#endif

		pkts_per_msg = 0;
		len = recvfrom(sfd, buf, BUFLEN, 0,
				(struct sockaddr *)&sa_kernel, &addrlen);
		if (addrlen != sizeof(sa_kernel)) {
#ifdef PRINTF
			printf("addrlen %u != %u\n", addrlen,
					(uint32_t)sizeof(sa_kernel));
#endif
			exit(-1);
		}
		if (len == -1) {
			perror("recvmsg");
			if (errno == EINTR)
				goto recv_new;
			exit(-1);
		}
		nlh = (struct nlmsghdr *)buf;
		if (nlh->nlmsg_flags & MSG_TRUNC || len > BUFLEN) {
#ifdef PRINTF
			printf("Packet truncated");
			exit(-1);
#endif
		}
		if (!NLMSG_OK(nlh, BUFLEN)) {
			perror("Netlink message parse error\n");
			return NULL;
		}
	}

	msg = NLMSG_DATA(nlh);

	remain_len = (len - ((char *)nlh - buf));
	if (nlh->nlmsg_flags & NLM_F_MULTI && nlh->nlmsg_type != NLMSG_DONE)
		nlh = NLMSG_NEXT(nlh, remain_len);
	else
		nlh = NULL;

	pkts_per_msg++;
	return msg;
}

int main(int argc, char **argv){

	int i;
	//int curr_len;

#ifdef PRINTF
	int pktcnt = 0;
#endif

	int rcvbufsize = BUFLEN;
	ebt_ulog_packet_msg_t *msg;
	struct ethhdr *ehdr;
	//struct ethertypeent *etype;

	//struct protoent *prototype;
	//struct iphdr *iph;
	struct tm* ptm;
	char time_str[40], *ctmp;

#ifdef tap_test
	//tap
	//int tap_fd;
	//int flags = IFF_TAP;
	//char if_name[IFNAMSIZ] = "tap0";
	const struct ip *ip_addr;	
	char interface_dev[] ="vif1.0-emu";
	struct mac_list *temp_mac_pointer =NULL;
	int on = 0;
	//	struct mac_list p;
	mac_list_init();

#endif

	if (argc == 2) {
		i = strtoul(argv[1], &ctmp, 10);
		if (*ctmp != '\0' || i < 1 || i > 32) {
#ifdef PRINTF
			printf("Usage: %s <group number>\nWith  0 < group "
					"number < 33\n", argv[0]);
			exit(0);
#endif
		}
		sa_local.nl_groups = sa_kernel.nl_groups = 1 << (i - 1);
	}



	sa_local.nl_pid = getpid();
	sfd = socket(PF_NETLINK, SOCK_RAW, NETLINK_NFLOG);

	if (!sfd) {
		perror("socket");
		exit(-1);
	}

	if (bind(sfd, (struct sockaddr *)(&sa_local), sizeof(sa_local)) ==
			-1) {
		perror("bind");
		exit(-1);
	}

	i = setsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &rcvbufsize, sizeof(rcvbufsize));

	// ignore error message
	//i = setsockopt(sfd, SOL_SOCKET, NETLINK_NO_ENOBUFS, &on, sizeof(on));

	while (1) {
		if (!(msg = ulog_get_packet()))
			continue;

		if (msg->version != EBT_ULOG_VERSION) {
			printf("WRONG VERSION: %d INSTEAD OF %d\n",
					msg->version, EBT_ULOG_VERSION);
			continue;
		}
#ifdef PRINTF
		printf("PACKET NR: %d\n", ++pktcnt);
#endif
		//iph = NULL;
		//curr_len = ETH_HLEN;

#ifdef PRINTF
		printf("INDEV=%s\nOUTDEV=%s\nPHYSINDEV=%s\nPHYSOUTDEV=%s\n"
				"PREFIX='%s'", msg->indev, msg->outdev, msg->physindev,
				msg->physoutdev, msg->prefix);
#endif

		ptm = localtime(&msg->stamp.tv_sec);
		strftime (time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", ptm);

#ifdef PRINTF
		printf("\nARRIVAL TIME: %s\nMARK=%lu\nHOOK=%s\n", time_str, msg->mark, hookstr[msg->hook]);
#endif 



		ehdr = (struct ethhdr *)msg->data;

		//etype = getethertypebynumber(ntohs(ehdr->h_proto));

		
#ifdef DISTRIBUTE_PACKET
		//fpr multicastaddress
		if(write_packet_for_multicastadd(ether_ntoa((const struct ether_addr *)ehdr->h_dest), msg ) == 1 || write_packet_for_multicastadd(ether_ntoa((const struct ether_addr *)ehdr->h_source), msg) == 1){
		
			goto label;
		}


		if(strcmp(ether_ntoa((const struct ether_addr *)ehdr->h_dest),"ff:ff:ff:ff:ff:ff") == 0 || 
				strcmp(ether_ntoa((const struct ether_addr *)ehdr->h_source),"ff:ff:ff:ff:ff:ff" )== 0  ){

			write_packet_for_broadcast_macadd(msg);

			goto label;
		}


		if(write_packet_for_broadcastadd(msg) == 1){
		
			goto label;
		
		}

		if(strcmp(msg->physoutdev, interface_dev) == 0 ){

			mac_list_add(ether_ntoa((const struct ether_addr *)ehdr->h_dest));
			temp_mac_pointer =  mac_list_check(ether_ntoa((const struct ether_addr *)ehdr->h_dest));

			ip_addr = (struct ip *)(msg->data+SIZE_ETHERNET);
			write(temp_mac_pointer->tap_fd_num , msg->data, ip_addr->ip_len);
		}

		if(strcmp(msg->physindev, interface_dev) == 0 ){
			mac_list_add(ether_ntoa((const struct ether_addr *)ehdr->h_source));
			temp_mac_pointer =  mac_list_check(ether_ntoa((const struct ether_addr *)ehdr->h_source));

			ip_addr = (struct ip *)(msg->data+SIZE_ETHERNET);
			write(temp_mac_pointer->tap_fd_num , msg->data, ip_addr->ip_len);

		}

label:
	mac_list_print();

#endif /* DISTRIBUTE_PACKET */

#ifdef etype_print
if (!etype)

	printf("0x%x\n", ntohs(ehdr->h_proto));
	else
	printf("%s\n", etype->e_name);

	if (ehdr->h_proto == htons(ETH_P_8021Q)) {
		struct vlan_hdr *vlanh = (struct vlan_hdr *)
			(((char *)ehdr) + curr_len);

		printf("::::::::::VLAN:HEADER::::::::::\n");
		curr_len += VLAN_HLEN;

		printf("VLAN TCI=%d\n", ntohs(vlanh->TCI));
		printf("VLAN ENCAPS PROTOCOL=");
		etype = getethertypebynumber(ntohs(vlanh->encap));
		if (!etype)

			printf("0x%x\n", ntohs(vlanh->encap));

		else
			printf("%s\n", etype->e_name);
		if (ehdr->h_proto == htons(ETH_P_IP))
			iph = (struct iphdr *)(vlanh + 1);
	} else if (ehdr->h_proto == htons(ETH_P_IP))


iph = (struct iphdr *)(((char *)ehdr) + curr_len);


curr_len += sizeof(struct iphdr);
18printf(":::::::::::IP:HEADER:::::::::::\n");
printf("IP SRC ADDR=");
for (i = 0; i < 4; i++)

printf("%d%s", ((unsigned char *)&iph->saddr)[i],
		(i == 3) ? "" : ".");

printf("\nIP DEST ADDR=");

for (i = 0; i < 4; i++)

printf("%d%s", ((unsigned char *)&iph->daddr)[i],
		(i == 3) ? "" : ".");

//printf("\nIP PROTOCOL=");

if (!(prototype = getprotobynumber(iph->protocol)))

	//	printf("%d\n", iph->protocol);
	else
	//	printf("%s\n", prototype->p_name);




	curr_len += 4;

	curr_len += 4;

	//printf("ICMP_ECHO IDENTIFIER=%u\n", icmph->un.echo.id);
	//printf("ICMP_ECHO SEQ NR=%u\n", ntohs(icmph->un.echo.sequence));


#endif
	}

return 0;
}
