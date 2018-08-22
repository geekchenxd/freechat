#include "display.h"


extern int state;
extern char username[LENGHT_USERNAME];

void* display_func(void *arg) 
{

    int i, j, error;
    char message_buffer[LENGHT_MESSAGE], message_buffer_2[LENGHT_MESSAGE];
    char filename[LENGHT_MESSAGE];
    char confirm_file[LENGHT_MESSAGE];
    FILE *fp;
	struct client *p = (struct client *)arg;

	if (!p)
		return NULL;

    while (state == 0) {

        if (recieve_data(LENGHT_MESSAGE, message_buffer) == 0){}
           // draw_new(global_display, "system>> recieve error");

        //draw_new(global_display, message_buffer);
        if (message_buffer[0] == '0') {
            split_str(1, strlen(message_buffer), message_buffer, message_buffer_2);
            draw_new(p->gui.display, message_buffer_2);
        }
        else if (message_buffer[0] == '1') {
            //do nothing
        }
        else if (message_buffer[0] == '2') {
            split_str(27, strlen(message_buffer) - 2, message_buffer, message_buffer_2);
            strcpy(username, message_buffer_2);
            split_str(1, strlen(message_buffer), message_buffer, message_buffer_2);
            draw_new(p->gui.display, message_buffer_2);
        }
        else if (message_buffer[0] == '3') {
            split_str(1, strlen(message_buffer), message_buffer, message_buffer_2);
            draw_new(p->gui.display, message_buffer_2);
            split_str(31, strlen(message_buffer) - 1, message_buffer, filename);

            sprintf(message_buffer_2, "downloaded/%s", filename);
            fp = fopen(message_buffer_2, "w+");
            error = 1;
            do {
                recieve_file(message_buffer);
                fprintf(fp, "%c", message_buffer[1]);
            }while(message_buffer[0] != '5');
            fclose(fp);

            draw_new(p->gui.display, "system>> Downloaded file success.");

        }

        //Reset value in message_buffer for check while loop's condition
        strcpy(message_buffer, "");
        strcpy(message_buffer_2, "");

    }

    return NULL;

}
