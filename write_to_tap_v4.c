//miyama 
#include "write_to_tap.h"
#define SIZE_ETHERNET 14

//#define print_check
#define mac_check

char destination[20] = "192.168.100.10"; 
int numrcv;
//int tap_fd, option;
int option;
int flags = IFF_TAP;
char if_name[IFNAMSIZ] = "";

/* sockaddr_in構造体 */
struct sockaddr_in cl_addr;

//mac list init 
struct mac_list *list_top;

//prptotype
int tun_alloc(char *dev, int flags);

struct mac_list{

	char mac_add[18];
	int tap_fd;
	struct mac_list *mac_list_next;

};



//init
void mac_list_init(){

	while(list_top != NULL){
		struct mac_list *delete_pos = list_top;
		list_top = list_top->mac_list_next;
		free(delete_pos);
	}

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

//count
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

#ifdef mac_check
//add
void mac_list_add(char item[18], int flag){

	char cmd_str[256];
	char tap_name[5];

	if(flag == 0){

		printf("dest mac %s\n",item);

	}else{

		printf("src mac  %s\n",item);


	}

	if(list_top == MAC_LIST_TAIL){

		list_top = (struct mac_list *)malloc(sizeof(struct mac_list));
		strcpy(list_top->mac_add, item);
		list_top->mac_list_next = MAC_LIST_TAIL;

		system("openvpn --mktun --dev tap --user shohei");

		//sprintf
		sprintf(cmd_str, "ip link set tap%d up", mac_list_count()-1);
		sprintf(tap_name, "tap%d", mac_list_count()-1);

		if ( (list_top->tap_fd = tun_alloc(tap_name, flags | IFF_NO_PI)) < 0 ) {
			printf("error:tun_alloc\n");
			exit(1);
		}



	}else{

		struct mac_list *p = list_top;
		while(p->mac_list_next != MAC_LIST_TAIL){
			p = p->mac_list_next;
		}

		p->mac_list_next = (struct mac_list *)malloc(sizeof(struct mac_list));
		strcpy(p->mac_list_next->mac_add, item);
		p->mac_list_next->mac_list_next = MAC_LIST_TAIL;


		//active taps
		system("openvpn --mktun --dev tap --user shohei");

		//sprintf
		sprintf(cmd_str, "ip link set tap%d up", mac_list_count()-1);

		sprintf(tap_name, "tap%d", mac_list_count()-1);

		if ( (p->mac_list_next->tap_fd = tun_alloc(tap_name, flags | IFF_NO_PI)) < 0 ) {
			printf("error:tun_alloc\n");
			exit(1);
		}
	}
}
#else

void mac_list_add(char item[18]){

	char cmd_str[256];
	char tap_name[5];
	if(list_top == MAC_LIST_TAIL){

		list_top = (struct mac_list *)malloc(sizeof(struct mac_list));
		strcpy(list_top->mac_add, item);
		list_top->mac_list_next = MAC_LIST_TAIL;

	}else{

		struct mac_list *p = list_top;
		while(p->mac_list_next != MAC_LIST_TAIL){
			p = p->mac_list_next;
		}

		p->mac_list_next = (struct mac_list *)malloc(sizeof(struct mac_list));
		strcpy(p->mac_list_next->mac_add, item);
		p->mac_list_next->mac_list_next = MAC_LIST_TAIL;


		//active taps
		//	cmd_str = "openvpn –mktun –dev tap –user username";

		//	strcpy(cmd_str, "openvpn –mktun –dev tap –user username");

//		printf("mac address %s\n", item);
//		system("openvpn --mktun --dev tap --user shohei");

		//sprintf
//		sprintf(cmd_str, "ip link set tap%d up", mac_list_count()-1);
//
//		sprintf(tap_name, "tap%d", mac_list_count()-1);

//		if ( (p->tap_fd = tun_alloc(tap_name, flags | IFF_NO_PI)) < 0 ) {
//			printf("error:tun_alloc\n");
//			exit(1);
//		}
//

	}



}
#endif

//check
struct mac_list *mac_list_check(char item[18], int flag){


	if(list_top == MAC_LIST_TAIL){

#ifdef mac_check
		if(flag == 0){

		mac_list_add(item, 0);
		return list_top;
		
		}else{
		mac_list_add(item,1);
		return list_top;
		}

#else

		mac_list_add(item);
		return list_top;

#endif
	}else{
		struct mac_list *p;
		p = list_top;
		while(p!= MAC_LIST_TAIL){

			if(strcmp(p->mac_add, item) == 0){

				return p;
			}
			p = mac_list_next(p);
		}

#ifdef mac_check
		if(flag == 0){

		mac_list_add(item, 0);
		return p;
		
		}else{
		mac_list_add(item, 1);
		return p;
		}
#else
		mac_list_add(item);
		return p;

#endif

	}

}

char * convmac_tostr(u_char *hwaddr,char *mac,size_t size){
	snprintf(mac,size,"%02x:%02x:%02x:%02x:%02x:%02x",
			hwaddr[0],hwaddr[1],hwaddr[2],
			hwaddr[3],hwaddr[4],hwaddr[5]);

	return mac;
}

void start_pktfunc( u_char *user,                  // pcap_loop関数の第4引数
		const struct pcap_pkthdr *h , // 受信したPacketの補足情報
		const u_char *packet               // 受信したpacketへのポインタ
		){

	char dmac[18] = {0};
	char smac[18] = {0};

	struct mac_list *p;
	struct ether_header *eth_hdr = (struct ether_header *)packet;

	int tmpsize, i;  
	unsigned int size[P_MAX_NUM];
	//int tap_fd, option;
	char buffer[P_MAX_SIZE];

	const struct ip *ip_addr;	
	ip_addr = (struct ip *)(packet+SIZE_ETHERNET);

	// size[]の初期化
	for(i=0;i<P_MAX_NUM;i++){
		size[i] = 0;
	}


	p = mac_list_check(convmac_tostr(eth_hdr->ether_dhost,dmac,sizeof(dmac)),0);
//	write(p->tap_fd, packet, ip_addr->ip_len);

	p = mac_list_check(convmac_tostr(eth_hdr->ether_shost,smac,sizeof(smac)), 1);
//	write(p->tap_fd, packet, ip_addr->ip_len);

	//if( strcmp(convmac_tostr(eth_hdr->ether_dhost,dmac,sizeof(dmac)),filter_mac)==0 || strcmp(convmac_tostr(eth_hdr->ether_shost,smac,sizeof(smac)),filter_mac)==0){
	//printf("dest mac %s\n",convmac_tostr(eth_hdr->ether_dhost,dmac,sizeof(dmac)));
	//printf("src mac %s\n",convmac_tostr(eth_hdr->ether_shost,smac,sizeof(smac)));
	

	//printf("Packet size %d byte \n",ip_addr->ip_len);
	//}


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


int main(int argc, char *argv[]){
	//int tap_fd, option;
	//int flags = IFF_TAP;
//	char if_name[IFNAMSIZ] = "";
	unsigned int size[P_MAX_NUM];
	unsigned int seq, allsize;
	int mode = 1;
	int rc;
	char buffer[P_MAX_SIZE];
	char hash[41];
	char recv_data[P_MAX_SIZE+sizeof(unsigned int)*(P_MAX_NUM+1)];
	int count = 0;


	pcap_t *pd = NULL;
	char ebuf[PCAP_ERRBUF_SIZE];

	struct mac_list p;
	mac_list_init();

//	if(argc < 2){
//		printf("tap name\n");
//		exit(1);
//	}

//	strncpy(if_name, argv[1], IFNAMSIZ-1);


	/* initialize tun/tap interface */
//	if ( (tap_fd = tun_alloc(if_name, flags | IFF_NO_PI)) < 0 ) {
//		printf("error:tun_alloc\n");
//		exit(1);
//	}


	if( (pd = pcap_open_live( "br0" ,             // インターフェイス名
					DPCP_RCV_MAXSIZE ,  // 最大受信サイズ(最初の68byteまで受信する)
					DPCP_PROMSCS_MODE , // 自分宛以外のパケットも処理の対象にする
					DPCP_RCV_TIMEOUT ,  // タイムアウト時間(ミリ秒)
					ebuf )) == NULL ){
		// error
		//			exit(-1);
	}

	if( pcap_loop( pd ,
				DPCP_NOLIMIT_LOOP , // エラーが発生するまで取得を続ける
				start_pktfunc,      // パケット受信した時のCallBack関数
				NULL                   //  CallBack関数へ渡す引数
		     ) < 0 ){
		// error
		exit(-1);
	}

	//	pcap_close(pd);
	//printf("writing data:sleep 60 seconds\n");
	//sleep(30);
	//
	return 0;
}
