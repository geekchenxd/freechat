#include "typing.h"
#include "c_string.h"
#include "search.h"

extern int state;
extern int g_bottom_line;
extern pthread_t *global_display_thread;
static struct user_list *current;

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

	wprintw(p->gui.input, "freechat>> Unselect %s\n", current->nickname);
	current = NULL;
	usleep(500000);
	werase(p->gui.single_line);
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
		wprintw(display, "the user '%s' is not in current list,^A to show all list\n");
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}

	current = tmp;
	werase(p->gui.single_line);
	wprintw(p->gui.single_line, "freechat>> chating with %s\n", current->nickname);
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
	char name[MAXNAMESIZE] = {0x0};
	WINDOW *display = NULL;

	if (!p)
		return;
	display = p->gui.input;

	wprintw(display, "freechat>>(search text:)");
	wrefresh(display);

	wscanw(display, " %[^\n]s", name);
	while (strlen(name) > 200) {
		werase(display);
		wprintw(display, "freechat>> Message cannot more than 200 characters.\n");
		wprintw(display, "freechat>> (search text:)");
		wrefresh(display);
		wscanw(display, " %[^\n]s", name);
	}

	if (search(name, p->gui.display)) {
		wprintw(display, "the text '%s' you are searching not fount in current reccord.\n");
		wrefresh(display);
		usleep(500000);
		werase(display);
		return;
	}
	werase(display);
	wrefresh(display);
}

void display_online_user(struct client *client)
{
	if (!client)
		return;
	int idx = 1;
	uint8_t show[MAXNAMESIZE + 4] = {0x0};
	char *notes = NULL;

	struct user_list *tmp = client->user;

	draw_new(client->gui.display, "freechat>> Friends List:");
	while (tmp) {
		if (idx == 1)
			notes = "(myself)";
		else
			notes = (tmp->is_room == 1) ? "(room)" : "(person)";
		
		sprintf(&show[0], "#%d. %s%s", idx++, tmp->nickname,notes);
		draw_new(client->gui.display, &show[0]);
		tmp = tmp->next;
	}
}

void* typing_func(void *arg) 
{

    char message_buffer[LENGHT_MESSAGE];
    char message_buffer_2[LENGHT_MESSAGE];
    //char confirm_file[LENGHT_MESSAGE];
    char filename[LENGHT_MESSAGE];
    char ch;
    int buffer_int;
    FILE *fp;
	struct client *p = (struct client *)arg;

    while (state == 0) {

        //Reset string for get new message
        strcpy(message_buffer, "");
        strcpy(message_buffer_2, "");

        wscanw(p->gui.input, " %[^\n]s", message_buffer);
        while (strlen(message_buffer) > 200) {
            werase(p->gui.input);
            draw_new(p->gui.input, "freechat>> Message cannot more than 200 characters.");
			sleep(1);
            werase(p->gui.input);
            wscanw(p->gui.input, " %[^\n]s", message_buffer);
        }

		switch (message_buffer[0]) {
		case 1:		/*^A show all online contact*/
			display_online_user(p);
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
			if (!current) {
				wprintw(p->gui.input, "%s\n", "freechat>> select a user to send or input correct cmd");
				wrefresh(p->gui.input);
				usleep(500000);
				werase(p->gui.input);
				continue;
			}
			break;
		}

		sprintf(message_buffer_2, "0%s", message_buffer);
		if(send_data(message_buffer_2) == 0) {
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
    }

    pthread_cancel(*global_display_thread);
    return 0;
}

