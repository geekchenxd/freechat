#include "config.h"

#if 0
int main(int argc, char **argv)
{
  config_t cfg;
  config_setting_t *setting;

  config_init(&cfg);

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, "init.cfg"))
  {
    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }


  /* Get the server configure information. */
  setting = config_lookup(&cfg, "server_info");
  if(setting != NULL)
  {
	  char *ip;
	  int serverport;
	  int local_port;
	  int port_local;
      if((config_setting_lookup_string(setting, "serverip", &ip)
           && config_setting_lookup_int(setting, "serverport", &serverport)
           && config_setting_lookup_int(setting, "local_port", &local_port)
           && config_setting_lookup_int(setting, "port_local", &port_local))) {
		  printf("IP is %s\nserverport is %d\nlocal_port is %d\nport_local is %d\n",
				  ip, serverport, local_port, port_local);
	  }
  }

  config_destroy(&cfg);
  return(EXIT_SUCCESS);
}
#endif

int parser_user_self(config_t *cfg)
{
	config_setting_t *setting;
	char *nickname = NULL;
	char *per_signature = NULL;
	struct user_list *self = NULL;
	uint8_t type;

	if (!cfg)
		return -1;
	setting = config_lookup(cfg, "client_info");
	if(setting != NULL) {
	  if((config_setting_lookup_int(setting, "user_type", &type)
		   && config_setting_lookup_string(setting, "nickname", &nickname)
		   && config_setting_lookup_string(setting, "per_signature", &per_signature))) {
		  self = user_list_init_user(nickname, "127.0.0.1", 0, 0);
		  if (!self)
			  return -1;
		  self->is_room = type;
		  memcpy(self->per_signature, per_signature, sizeof(self->per_signature));

		  user_list_add(&client.user, self);
#if 0
		  printf("is_room =  %d\n", client.user->is_room);
		  printf("nickname is %s\n", client.user->nickname);
		  printf("per_signature is %s\n", client.user->per_signature);
#endif
	  } else {
		  return -2;
	  }
	} else {
		return -1;
	}

	return 0;
}

int parser_server_info(config_t *cfg, struct info *info)
{
	config_setting_t *setting;

	setting = config_lookup(cfg, "server_info");
	if(setting != NULL) {
	  char *ip = NULL;
	  int serverport;
	  int local_port;
	  int port_local;
	  if((config_setting_lookup_string(setting, "serverip", &ip)
		   && config_setting_lookup_int(setting, "serverport", &serverport)
		   && config_setting_lookup_int(setting, "local_port", &local_port)
		   && config_setting_lookup_int(setting, "port_local", &port_local))) {
		  info->serverport = serverport;
		  info->local_port = local_port;
		  info->port_local = port_local;
		  memcpy(info->serverip, ip, sizeof(info->serverip));
#if 0
		  printf("serverport is %d\n", info->serverport);
		  printf("local_port is %d\n", info->local_port);
		  printf("port_local is %d\n", info->port_local);
		  printf("serverip is %s\n", info->serverip);
#endif
	  } else {
		  return -2;
	  }
	} else {
		return -1;
	}

	return 0;
}

int config_parser(const char *path, struct info *info)
{
	config_t cfg;

	config_init(&cfg);

	/* Read the file. If there is an error, report it and exit. */
	if (!config_read_file(&cfg, path)) {
		fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
				config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		return -1;
	}

	if (parser_server_info(&cfg, info)) {
		config_destroy(&cfg);
		return -2;
	}

	if (parser_user_self(&cfg)) {
		config_destroy(&cfg);
		return -3;
	}

	config_destroy(&cfg);

	return 0;
}

