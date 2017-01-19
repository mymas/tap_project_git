#ifndef MAC_LIST_H
#define MAC_LIST_H

struct mac_list{

	char mac_add[18];
	struct mac_list *mac_list_next;

};

//extern struct mac_list *list_top;
//extern struct mac_list p;
void mac_list_init();
struct mac_list * mac_list_next(struct mac_list * pos);
struct mac_list * mac_list_top(void);
void mac_list_add(char item[18]);
int mac_list_count(void);
struct mac_list * mac_list_check(char item[18]);

#endif
