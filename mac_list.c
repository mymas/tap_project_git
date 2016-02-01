#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//typedef int LIST_ITEM;
#define MAC_LIST_TAIL NULL
//#define check_mac_list



struct mac_list{

	char mac_add[18];
	struct mac_list *mac_list_next;

};

struct mac_list *list_top;


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

//add
void mac_list_add(char item[18]){


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

	}

}

//(item)

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


#ifdef check_mac_list
int main(int argc, const char * argv[]){

	//	system("pwd");
	//	system("ls -al");


	struct mac_list p;

	mac_list_init();
	mac_list_add("XX.XX.XX.A1");
	mac_list_add("XX.XX.XX.A2");
	mac_list_add("XX.XX.XX.A3");
	mac_list_add("XX.XX.XX.A4");


	printf("count %d\n", mac_list_count());

	return 0;
}
#endif




