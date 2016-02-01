//#include <stdio.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <sys/fcntl.h>
//#include <arpa/inet.h>
//#include <string.h>
//#include <stdlib.h>
//#include <assert.h>
//#include <stdint.h>
//#include <unistd.h>
//#include <inttypes.h>
//#include <net/if.h>
//#include <linux/if_tun.h>
//#include <sys/ioctl.h>
//#include <sys/stat.h>
//#include <fcntl.h>

//miyama 
#include "write_to_tap.h"
#define SIZE_ETHERNET 14

//#define P_MAX_SIZE 12000
//#define P_MAX_NUM  200

int clsocket;
char destination[20] = "192.168.100.10"; 
int numrcv;
int tap_fd, option;

/* sockaddr_in構造体 */
struct sockaddr_in cl_addr;


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
	//char filter_mac[18] ="00:16:3e:0d:58:c1";
	//char filter_mac[18] ="00:16:3e:0d:58:v1";
	char filter_mac[18] ="00:16:3e:0d:58:00";

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

	//データがからじゃないかのチェック
	//	if(tmpsize < sizeof(unsigned int)*P_MAX_NUM){
	//		for(i = tmpsize; i < sizeof(unsigned int)*P_MAX_NUM; i+=tmpsize){
	//			tmpsize = recv(clsocket, &size+i, sizeof(unsigned int)*P_MAX_NUM-i, 0);
	//		}
	//	} 

	//	for(i=0; size[i]!=0; i++){
	//		//count++;
	//		//if((write(tap_fd, buffer+tmpsize, size[i])) < 0)
	//		if((write(tap_fd, packet+tmpsize, size[i])) < 1){
	//			perror("Writing data");
	//			printf("are\n");
	//			//exit(1);
	//		}
	//
	//tmpsize += size[i];
	//	}




	if( strcmp(convmac_tostr(eth_hdr->ether_dhost,dmac,sizeof(dmac)),filter_mac)==0 || strcmp(convmac_tostr(eth_hdr->ether_dhost,smac,sizeof(smac)),filter_mac)==0){
		//printf("dest mac %s\n",convmac_tostr(eth_hdr->ether_dhost,dmac,sizeof(dmac)));
		//printf("src mac %s\n",convmac_tostr(eth_hdr->ether_shost,smac,sizeof(smac)));
		//printf("Packet size %d byte \n",ip_addr->ip_len);
		write(tap_fd, packet, ip_addr->ip_len);
		//printf("Write check %ld\n",write(tap_fd, packet, write(tap_fd, packet, ip_addr->ip_len)));
	}




}

int tun_alloc(char *dev, int flags) {

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


int main(int argc, char *argv[])
{
	//int tap_fd, option;
	int flags = IFF_TAP;
	char if_name[IFNAMSIZ] = "";
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


	if(argc < 2){
		printf("tap name\n");
		exit(1);
	}

	strncpy(if_name, argv[1], IFNAMSIZ-1);


	/* initialize tun/tap interface */
	if ( (tap_fd = tun_alloc(if_name, flags | IFF_NO_PI)) < 0 ) {
		printf("error:tun_alloc\n");
		exit(1);
	}

	//int tmpsize, i;  

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
	//}

return 0;
}

