#include "typing.h"

extern int state;
extern int g_bottom_line;
extern pthread_t *global_display_thread;

void* typing_func(void *arg) 
{

    char message_buffer[LENGHT_MESSAGE];
    char message_buffer_2[LENGHT_MESSAGE];
    char confirm_file[LENGHT_MESSAGE];
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
            draw_new(p->gui.display, "system>> Message cannot more than 200 characters.");
            wscanw(p->gui.input, " %[^\n]s", message_buffer);
        }

        //Draw_new line to display message
        strcpy(message_buffer_2, "you>> ");
        strcat(message_buffer_2, message_buffer);
        draw_new(p->gui.display, message_buffer_2);


		switch (message_buffer[0]) {
		case 5:		/*^E*/
			state = 1;
			break;
		case 7:		/*^G*/
			draw_new(p->gui.display, "system>> ### THIS IS HELP! ###");
			draw_new(p->gui.display, "system>> \":q!\" to exit program.");
			draw_new(p->gui.display, "system>> \"/talkto [nickname]\" to choose contact.");
			draw_new(p->gui.display, "system>> \"/untalk\" to remove contact that we are talking.");
			draw_new(p->gui.display, "system>> \"/upload [file]\" to upload file to client that you are talking.");
			draw_new(p->gui.display, "system>> \"/watline\" to show number of latest line");
			draw_new(p->gui.display, "system>> \"/up [amount of line]\" to scroll screen up n lines.");
			draw_new(p->gui.display, "system>> \"/down [amount of line]\" to scroll screen down n lines.");
			draw_new(p->gui.display, "system>> \"/find [word]\" to find number of line that word was display.");
			draw_new(p->gui.display, "system>> \"/contact\" to show all user on server.");
			break;
		default:
			break;
		}
        //Check exit command
        if (strcmp(message_buffer, ":q!") == 0) {
            //set state to stop all function
            state = 1;
        }
        else if (message_buffer[0] == '/') {

            if (split_strcmp(0, 6, "/upload", 0, 6, message_buffer)){

                split_str(8, strlen(message_buffer), message_buffer, filename);
                sprintf(message_buffer, "3system>> Sending file to you: %s", filename);
                //send_data(message_buffer);

                sleep(1);

                draw_new(p->gui.display, "system>> Uploading...");

                fp = fopen(filename, "r");
                while( ( ch = fgetc(fp) ) != EOF ){

                    sprintf(message_buffer, "4%c", ch);

                    if(send_data(message_buffer) == 0)
                        draw_new(p->gui.display, "system>> Send failed");

                }
                fclose(fp);

                sleep(1);

                strcpy(message_buffer, "5");
                send_data(message_buffer);
                draw_new(p->gui.display, "system>> Done!");

            }
            else if (split_strcmp(0, 2, "/up", 0, 2, message_buffer)){

                split_str(4, strlen(message_buffer), message_buffer, message_buffer_2);
                buffer_int = atoi(message_buffer_2);
                draw_old_line(p->gui.display, 1, buffer_int);

            }
            else if (split_strcmp(0, 4, "/down", 0, 4, message_buffer)){

                split_str(6, strlen(message_buffer), message_buffer, message_buffer_2);
                buffer_int = atoi(message_buffer_2);
                draw_old_line(p->gui.display, 2, buffer_int);

            }
            else if (split_strcmp(0, 4, "/help", 0, 4, message_buffer)){

                draw_new(p->gui.display, "system>> ### THIS IS HELP! ###");
                draw_new(p->gui.display, "system>> \":q!\" to exit program.");
                draw_new(p->gui.display, "system>> \"/talkto [nickname]\" to choose contact.");
                draw_new(p->gui.display, "system>> \"/untalk\" to remove contact that we are talking.");
                draw_new(p->gui.display, "system>> \"/upload [file]\" to upload file to client that you are talking.");
                draw_new(p->gui.display, "system>> \"/watline\" to show number of latest line");
                draw_new(p->gui.display, "system>> \"/up [amount of line]\" to scroll screen up n lines.");
                draw_new(p->gui.display, "system>> \"/down [amount of line]\" to scroll screen down n lines.");
                draw_new(p->gui.display, "system>> \"/find [word]\" to find number of line that word was display.");
                draw_new(p->gui.display, "system>> \"/contact\" to show all user on server.");

            }
            else if (split_strcmp(0, 4, "/find", 0, 4, message_buffer)){

                split_str(6, strlen(message_buffer) - 1, message_buffer, message_buffer_2);
                search(message_buffer_2, p->gui.display);

            }
            else if (split_strcmp(0, 7, "/watline", 0, 7, message_buffer)){

                //bottom_line come from buffer_screen.h
                sprintf(message_buffer, "system>> v This is lines number %d. v", g_bottom_line);
                draw_new(p->gui.display, message_buffer);

            }
            else if (
                    split_strcmp(0, 6, "/talkto", 0, 6, message_buffer) ||
                    split_strcmp(0, 6, "/untalk", 0, 6, message_buffer) ||
                    split_strcmp(0, 7, "/contact", 0, 7, message_buffer)) {

                sprintf(message_buffer_2, "0%s", message_buffer);
                send_data(message_buffer_2);
            }
            else {

                draw_new(p->gui.display, "system>> Command not found.");

            }
        }
        else {

			wprintw(p->gui.display, "%d\n", message_buffer[0]);
			wrefresh(p->gui.display);
            //Set protocal to send packet
            sprintf(message_buffer_2, "0%s", message_buffer);
            if(send_data(message_buffer_2) == 0)
                draw_new(p->gui.input, "system>> Send failed");

        }

		sleep(1);
        werase(p->gui.input);

    }

    pthread_cancel(*global_display_thread);
    return 0;

}

