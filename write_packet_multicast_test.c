#include<stdio.h>
#include<string.h>


int main(){


	char multicast_macadd[6] = "33:33:";
	char multicast_temp[17] = "33:33:0:0:0:fb";

	char multicast_temp2[17] = "fe:ff0:0:0:fb";

	char temp[6] = "";
       	strncpy(temp, multicast_temp, 6);



	if(strcmp(temp, multicast_macadd) == 0){
	

	printf("temp %s multicast_macadd %s \n", temp, multicast_macadd);

	}


       	strncpy(temp, multicast_temp2, 6);

	if(strcmp(temp, multicast_macadd) == 0){
	

	printf("temp %s multicast_macadd %s \n", temp, multicast_macadd);

	}else{
	
	printf("temp %s multicast_macadd %s \n", temp, multicast_macadd);
	}





	return 0;

}
