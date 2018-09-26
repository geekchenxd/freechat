#include "typing.h"
#include "search.h"
#include "gui.h"
#include "fttp.h"

extern int state;
extern int g_bottom_line;
extern pthread_t *global_display_thread;
static struct user_list *current;

bool user_is_current(struct user_list *user)
{
	return (user == current);
}

void remove_current_select(struct client *p)
{
	if (!current) {
		wprintw(p->gui.input, "freechat>> No user has been selected.");
		wrefresh(p->gui.input);
		usleep(500000);
		werase(p->gui.input);
		wrefresh(p->gui.input);
		return;
	}

	wprintw(p->gui.input, "freechat>> Unselect %s\n", current->user->name);
	wrefresh(p->gui.input);
	current = NULL;
	usleep(500000);
	werase(p->gui.single_line);
    wbkgd(p->gui.single_line, COLOR_PAIR(4));
	wrefresh(p->gui.single_line);
	werase(p->gui.input);
	wrefresh(p->gui.input);
}

void update_current_select(struct client *p)
{
	struct user_list *tmp = NULL;
	char name[MAXNAMESIZE] = {0x0};
	WINDOW *display = NULL;

	if (!p)
		return;
	display = p->gui.input;

	wprintw(display, "freechat>>(select user:)");
	wrefresh(display);

	wscanw(display, " %[^\n]s", name);
	while (strlen(name) > 200) {
		werase(display);
		wprintw(display, "freechat>> Message cannot more than 200 characters.\n");
		wprintw(display, "freechat>> (select user:)");
		wrefresh(display);
		wscanw(display, " %[^\n]s", name);
	}

	tmp = user_list_find(client.user, name);
	if (!tmp) {
		wprintw(display, 
				"the user '%s' is not in current list,^A to show all list\n",
				name);
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}

	current = tmp;
	werase(p->gui.single_line);
    wbkgd(p->gui.single_line, COLOR_PAIR(3));
	wprintw(p->gui.single_line, "freechat>> chating with %s\n", current->user->name);
	wrefresh(p->gui.single_line);
	werase(display);
}

void send_file(struct client *p)
{
	struct user_list *tmp = NULL;
	char name[MAXNAMESIZE] = {0x0};
	WINDOW *display = NULL;

	if (!p)
		return;

	display = p->gui.input;

	tmp = current;
	if (tmp == NULL) {
		werase(display);
		wprintw(display, "freechat>> Please select contact first!\n");
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}


	wprintw(display, "freechat>>(file path:)");
	wrefresh(display);

	wscanw(display, " %[^\n]s", name);
	while (strlen(name) > 200) {
		werase(display);
		wprintw(display, "freechat>> Message cannot more than 200 characters.\n");
		wprintw(display, "freechat>> (file path:)");
		wrefresh(display);
		wscanw(display, " %[^\n]s", name);
	}

	werase(display);
	wprintw(display, "freechat>> Sending file %s\n", name);
	wrefresh(display);
	usleep(500000);
	werase(display);
}

void search_text(struct client *p)
{
	char text[MAXNAMESIZE] = {0x0};
	WINDOW *display = NULL;

	if (!p)
		return;
	display = p->gui.input;

	wprintw(display, "freechat>>(search text:)");
	wrefresh(display);

	wscanw(display, " %[^\n]s", text);
	while (strlen(text) > 200) {
		werase(display);
		wprintw(display, "freechat>> Message cannot more than 200 characters.\n");
		wprintw(display, "freechat>> (search text:)");
		wrefresh(display);
		wscanw(display, " %[^\n]s", text);
	}

	if (strlen(text) == 0) {
		werase(display);
		return;
	}

	if (search(text, p->gui.display)) {
		wprintw(display, "the text '%s' you are searching not fount in current reccord.\n");
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}
	werase(display);
	wrefresh(display);
}

/*
 * display all the online user or the message disabled user
 * flag is true indicate all of the online user and flag is
 * false indicate message disabled user.
 */
void display_online_user(struct client *client, bool flag)
{
	if (!client)
		return;
	int idx = 1;
	uint8_t show[MAXNAMESIZE + 20] = {0x0};
	char *notes = NULL;

	struct user_list *tmp = client->user;

	draw_new(client->gui.display, "freechat>> Friends List:");
	while (tmp) {
		if (idx == 1)
			notes = "(myself)";
		else
			notes = (tmp->user->sex == 2) ? "(room)" : "(person)";
		if (flag) {
			sprintf((char *)&show[0], "#%d. %s%s", idx++, tmp->user->name, notes);
			draw_new(client->gui.display, (char *)&show[0]);
			memset(&show[0], 0x0, sizeof(show));
		} else {
			if (tmp->enable == flag) {
				sprintf((char *)&show[0], "#%d. %s%s", idx++, tmp->user->name, notes);
				draw_new(client->gui.display, (char *)&show[0]);
				memset(&show[0], 0x0, sizeof(show));
			}
		}
		tmp = tmp->next;
	}
}

bool port_is_valid(uint16_t port)
{
	return ((port > 1024) && (port < 65535));
}

bool ip_is_valid(char *ip)
{
	int a,b,c,d;
	bool status = false;
	char temp[16] = {0x0};

	if (!ip)
		return status;

	if (sscanf(ip, "%d.%d.%d.%d ", &a, &b, &c, &d) == 4
			&& a >= 0 && a <= 255 && b >= 0 && b <= 255
			&& c >= 0 && c <= 255 && d >= 0 && d <= 255) {
		sprintf(temp, "%d.%d.%d.%d", a, b, c, d);
		if (strcmp(temp, ip) == 0) {
			status = true;
		}
	}

	return status;
}

bool birthday_is_valid(char *birthday)
{
	int year, mon, day;
	bool status = false;
	char temp[12] = {0x0};

	if (!birthday)
		return status;

	if (sscanf(birthday, "%d-%d-%d", &year, &mon, &day) == 3
			&& year >= 1980 && year <= 2020 && mon >= 1 && mon <= 12
			&& day >= 1 && day <= 31) {
		sprintf(temp, "%04d-%02d-%02d", year, mon, day);
		if (strcmp(temp, birthday) == 0) {
			status = true;
		}
	}

	return status;
}

void connect_server(struct client *p)
{
	char ip[16] = {0};
	uint16_t port = 0;
	WINDOW *display = NULL;

	if (!p)
		return;
	display = p->gui.input;

	/*get server ip address*/
	wprintw(display, "freechat>>[server ip(null is default):]");
	wrefresh(display);

	wscanw(display, " %[^\n]s", &ip[0]);
	if ((strlen(ip) > 16) || (!(ip_is_valid(&ip[0])) && strlen(ip) > 0)) {
		werase(display);
		wprintw(display, "freechat>> Invalid IP address.\n");
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}
	werase(display);
	
	/*get server port*/
	wprintw(display, "freechat>>[server port(null is default):]");
	wrefresh(display);

	wscanw(display, "%u", &port);
	if (!(port_is_valid(port)) && port != 0) {
		werase(display);
		wprintw(display, "freechat>> Invalid port.\n");
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}
	werase(display);

	/*
	 * if not input, set the default server ip and port
	 */
	if (strlen(ip) == 0)
		memcpy(ip, p->info.serverip, 16);
	if (port == 0)
		port = p->info.serverport;

	/*
	 * here connect to the server.if connect successfully,
	 * we should add the server to current user list.
	 */
	/*debug*/
	draw_new(p->gui.display, ip);
	char tmp[12] = {0};
	sprintf(tmp, "%u", port);
	draw_new(p->gui.display, tmp);
}

/*
 * display user details
 */
void contact_info(struct client *p)
{
	struct user_list *tmp = NULL;
	char name[MAXNAMESIZE] = {0x0};
	WINDOW *display = NULL;
	char *type[6] = {0};

	if (!p)
		return;
	display = p->gui.input;

	wprintw(display, "freechat>>(select user:)");
	wrefresh(display);

	wscanw(display, " %[^\n]s", name);
	if (strlen(name) > MAXNAMESIZE) {
		werase(display);
		wprintw(display, "freechat>> Invalid name.\n");
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}

	tmp = user_list_find(p->user, name);
	if (!tmp) {
		wprintw(display, 
				"the user '%s' is not in current list,^A to show all list\n",
				name);
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}
	werase(display);

	/* display the user info on display */
	display = p->gui.display;

	if (tmp->user->sex == 0)
		memcpy(type, "boy", strlen("boy"));
	else if (tmp->user->sex == 1)
		memcpy(type, "girl", strlen("girl"));
	else
		memcpy(type, "room", strlen("room"));

	wprintw(display, "\nfreechat>> User Info:\n");
	wprintw(display, "freechat>> name:%s\n", tmp->user->name);
	wprintw(display, "freechat>> sex:%s\n", type);
	wprintw(display, "freechat>> birthday:%s\n", tmp->user->birthday);
	wprintw(display, "freechat>> signature:%s\n\n", tmp->user->signature);
	wrefresh(display);
}

void edit_myself(struct client *p)
{
}

void refresh_list(struct client *p)
{
	/*1.delete all user except myself and chat room*/
	/*2.send user request to broad cast*/
}

void save_log(struct client *p)
{
}

void set_msg_enable_flag(struct client *p, bool flag)
{
	struct user_list *tmp = NULL;
	char name[MAXNAMESIZE] = {0x0};
	WINDOW *display = NULL;

	if (!p)
		return;
	display = p->gui.input;

	wprintw(display, "freechat>>(select user:)");
	wrefresh(display);

	wscanw(display, " %[^\n]s", name);
	if (strlen(name) > MAXNAMESIZE) {
		werase(display);
		wprintw(display, "freechat>> Invalid name.\n");
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}

	tmp = user_list_find(p->user, name);
	if (!tmp) {
		wprintw(display, 
				"the user '%s' is not in current list,^A to show all list\n",
				name);
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}
	werase(display);

	/*clear the message enable flag*/
	tmp->enable = flag;
}

void disabled_list(struct client *p)
{
	display_online_user(p, false);
}

void set_server(struct client *p)
{
	char ip[16] = {0};
	uint16_t port = 0;
	WINDOW *display = NULL;

	if (!p)
		return;
	display = p->gui.input;

	/*get server ip address*/
	wprintw(display, "freechat>>[server ip(null is default):]");
	wrefresh(display);

	wscanw(display, " %[^\n]s", &ip[0]);
	if ((strlen(ip) > 16) || !(ip_is_valid(&ip[0]))) {
		werase(display);
		wprintw(display, "freechat>> Invalid IP address.\n");
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}
	werase(display);
	
	/*get server port*/
	wprintw(display, "freechat>>[server port(null is default):]");
	wrefresh(display);

	wscanw(display, "%u", &port);
	if (!(port_is_valid(port))) {
		werase(display);
		wprintw(display, "freechat>> Invalid port.\n");
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}
	werase(display);


	/*
	 * here set the server ip and port to configure file.
	 */
	/*debug*/
	draw_new(p->gui.display, ip);
	char tmp[12] = {0};
	sprintf(tmp, "%u", port);
	draw_new(p->gui.display, tmp);
}

/*
 * This function handles the advanced options.
 */
void advanced_options(char *cmd, struct client *p)
{
	if (!cmd || !p)
		return;
	/*
	 * connect to server
	 * show a contact's detail info
	 * edit my info
	 * fresh contact list
	 * save history log
	 * disable contact's message
	 * show disabled list
	 * enable contact
	 * set server info
	 */
	if (strcasecmp(cmd, ":server") == 0) {
		werase(p->gui.input);
		connect_server(p);
	} else if (strcasecmp(cmd, ":info") == 0) {
		werase(p->gui.input);
		contact_info(p);
	} else if (strcasecmp(cmd, ":edit") == 0) {
		werase(p->gui.input);
		edit_myself(p);
	} else if (strcasecmp(cmd, ":refresh") == 0) {
		werase(p->gui.input);
		refresh_list(p);
	} else if (strcasecmp(cmd, ":log") == 0) {
		werase(p->gui.input);
		save_log(p);
	} else if (strcasecmp(cmd, ":disable") == 0) {
		werase(p->gui.input);
		set_msg_enable_flag(p, false);
	} else if (strcasecmp(cmd, ":list") == 0) {
		werase(p->gui.input);
		disabled_list(p);
	} else if (strcasecmp(cmd, ":enable") == 0) {
		werase(p->gui.input);
		set_msg_enable_flag(p, true);
	} else if (strcasecmp(cmd, ":setserver") == 0) {
		werase(p->gui.input);
		set_server(p);
	} else {
		draw_new(p->gui.input, "freechat>> Invalid command!\n");
		wrefresh(p->gui.input);
		usleep(500000);
		werase(p->gui.input);
	}
}

void* typing_func(void *arg) 
{
	int cmd = 0;
	struct client *p = (struct client *)arg;
    char message_buffer[LENGHT_MESSAGE];
    char message_buffer_2[LENGHT_MESSAGE];

	while (state == 0) {
        /*clear the message buffer*/
        strcpy(message_buffer, "");
        strcpy(message_buffer_2, "");

		cmd = wgetch(p->gui.input);
		if (cmd == '\n')
			continue;
		switch (cmd) {
		case 1:		/*^A show all online contact*/
			display_online_user(p, true);
			werase(p->gui.input);
			continue;
		case 4:		/*^D down page */
			draw_old_line(p->gui.display, 2, (int)get_display_height() - 1);
			werase(p->gui.input);
			continue;
		case 5:		/*^E*/
			state = 1;
			werase(p->gui.input);
			continue;
		case 6:		/*^F send file*/
			werase(p->gui.input);
			send_file(p);
			continue;
		case 7:		/*^G*/
			werase(p->gui.input);
			show_base_info(p->gui.display);
			continue;
		case 15:	/*^O unselect*/
			werase(p->gui.input);
			remove_current_select(p);
			continue;
		case 18:	/*^R	up one page*/
			werase(p->gui.input);
			draw_old_line(p->gui.display, 1, (int)get_display_height() - 1);
			continue;
		case 20:	/*^T find text*/
			werase(p->gui.input);
			search_text(p);
			continue;
		case 23:		/*^W latest line*/
			werase(p->gui.input);
			draw_new(p->gui.display, NULL);
			continue;
		case 25:	/*^Y select a contact*/
			werase(p->gui.input);
			update_current_select(p);
			continue;
		default:
			message_buffer[0] = (char)cmd;
			wscanw(p->gui.input, " %[^\n]s", &message_buffer[1]);
			if (strlen(message_buffer) > 200) {
				werase(p->gui.input);
				draw_new(p->gui.input, "freechat>> Message cannot more than 200 characters.");
				wrefresh(p->gui.input);
				usleep(500000);
				werase(p->gui.input);
				continue;
			}
			if (message_buffer[0] == ':') {
				advanced_options(&message_buffer[0], p);
				continue;
			}

			if (!current) {
				wprintw(p->gui.input, "%s\n", "freechat>> select a user to send or input correct cmd");
				wrefresh(p->gui.input);
				usleep(500000);
				werase(p->gui.input);
				continue;
			}
			break;
		}

		/*if the type(sex) is not room*/
		if(send_text_udp((uint8_t *)message_buffer, strlen(message_buffer), 
					current->user->id, p->user->user->id) <= 0) {
			wprintw(p->gui.input, "%s\n", "freechat>> Send failed");
			wrefresh(p->gui.input);
			usleep(500000);
			werase(p->gui.input);
			continue;
		}
        //Draw_new line to display message
        strcpy(message_buffer_2, "Me>> ");
        strcat(message_buffer_2, message_buffer);
        draw_new(p->gui.display, message_buffer_2);
		werase(p->gui.input);
	}

    pthread_cancel(*global_display_thread);
	return NULL;
}

