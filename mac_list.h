//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>


struct mac_list{

	char mac_add[18];
	struct mac_list *mac_list_next;

};

extern struct mac_list *list_top;
//struct mac_list *list_top;
//void mac_list_init();
//struct mac_list * mac_list_next(struct mac_list * pos){
//struct mac_list * mac_list_top(void);
void mac_list_add(char item[18]);
//int mac_list_count(void);
