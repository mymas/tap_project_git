//#include <pcap.h>
//#include <stdio.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netinet/if_ether.h> //ethernetヘッダの定義
//#include <netinet/ip.h> //ipヘッダの定義
//#include <netinet/tcp.h> //tcpヘッダの定義
//#include <string.h>

//when you compling this file, you do not forget to link command "-lpcap".

#include "write_to_tap.h"

#define case4

#ifdef case4

#define DPCP_RCV_MAXSIZE   68
#define DPCP_PROMSCS_MODE  1
#define DPCP_RCV_TIMEOUT   1000
#define DPCP_NOLIMIT_LOOP  -1

char * convmac_tostr(u_char *hwaddr,char *mac,size_t size){
	snprintf(mac,size,"%02x:%02x:%02x:%02x:%02x:%02x",
			hwaddr[0],hwaddr[1],hwaddr[2],
			hwaddr[3],hwaddr[4],hwaddr[5]);

	return mac;
}


void start_pktfunc( u_char *user,                  // pcap_loop関数の第4引数
		const struct pcap_pkthdr *h , // 受信したPacketの補足情報
		const u_char *p               // 受信したpacketへのポインタ
		){
	char dmac[18] = {0};
	char smac[18] = {0};

	//このpをtapに書き込んでいけばいいのかしら
	struct ether_header *eth_hdr = (struct ether_header *)p;

	int tmpsize, i;  
	unsigned int size[P_MAX_NUM];
	int tap_fd, option;
	char buffer[P_MAX_SIZE];

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

	for(i=0; size[i]!=0; i++){
		//count++;
		if((write(tap_fd, buffer+tmpsize, size[i])) < 0){
			perror("Writing data");
			printf("are\n");
			//exit(1);
		}
		tmpsize += size[i];
	}

//	if( strcmp(convmac_tostr(eth_hdr->ether_dhost,dmac,sizeof(dmac)),"00:16:3e:0d:58:c1") == 0|| strcmp(convmac_tostr(eth_hdr->ether_dhost,smac,sizeof(smac)),"00:16:3e:0d:58:c1")==0){
		printf("---------ether header---------\n");
		printf("dest mac %s\n",convmac_tostr(eth_hdr->ether_dhost,dmac,sizeof(dmac)));
		printf("src mac %s\n",convmac_tostr(eth_hdr->ether_shost,smac,sizeof(smac)));
		printf("ether type %x\n\n",ntohs(eth_hdr->ether_type));
//	}




}



#endif


int main(int argc, char *argv[]){

#ifdef case4

	pcap_t *pd = NULL;
	char ebuf[PCAP_ERRBUF_SIZE];

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
		//			exit(-1);
	}
	pcap_close(pd);
	return 0;
#endif
}
