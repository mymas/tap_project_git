#include <stdio.h>
#include <stdlib.h>
 
int main(int argc, const char * argv[]){

//	system("pwd");
//	system("ls -al");


	system("openvpn --mktun --dev tap --user shohei");
	system("ip link set tap0 up");

	return 0;
}
