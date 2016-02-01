#include <pcap.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/if_ether.h> //ethernetヘッダの定義
#include <netinet/ip.h> //ipヘッダの定義
#include <netinet/tcp.h> //tcpヘッダの定義

#include <string.h>

#include <netdb.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DPCP_RCV_MAXSIZE   68
#define DPCP_PROMSCS_MODE  1
#define DPCP_RCV_TIMEOUT   1000
#define DPCP_NOLIMIT_LOOP  -1

#define P_MAX_SIZE 12000
#define P_MAX_NUM  200


//mac_list
#define MAC_LIST_TAIL NULL
