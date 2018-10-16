#include "config.h"

config_t *get_config_root(const char *path)
{
	config_t *cfg = NULL;

	cfg = (config_t *)malloc(sizeof(config_t));
	if (cfg == NULL)
		goto err;
	config_init(cfg);
	if (!config_read_file(cfg, path)) {
		config_destroy(cfg);
		free(cfg);
		goto err;
	}
err:
	return cfg;
}

int cofig_up_file(config_t *cfg, const char *path)
{
	return config_write_file(cfg, path);
}

int config_up_servip(config_t *cfg, char *newip)
{
	int error = 0;
	config_setting_t *setting;

	setting = config_lookup(cfg, "server_info.serverip");
	if(setting == NULL) {
		error = -1;
		goto out;
	}
	config_setting_set_string(setting, newip);

out:
	return error;
}

int config_up_servport(config_t *cfg, uint16_t newport)
{
	int error = 0;
	config_setting_t *setting;

	setting = config_lookup(cfg, "server_info.serverport");
	if(setting == NULL) {
		error = -1;
		goto out;
	}
	config_setting_set_int(setting, newport);

out:
	return error;
}

int config_up_nickname(config_t *cfg, char *name)
{
	int error = 0;
	config_setting_t *setting;

	setting = config_lookup(cfg, "client_info.nickname");
	if(setting == NULL) {
		error = -1;
		goto out;
	}
	config_setting_set_string(setting, name);

out:
	return error;
}

int config_up_user_type(config_t *cfg, int type)
{
	int error = 0;
	config_setting_t *setting;

	setting = config_lookup(cfg, "client_info.user_type");
	if(setting == NULL) {
		error = -1;
		goto out;
	}
	config_setting_set_int(setting, type);

out:
	return error;
}

int config_up_signature(config_t *cfg, char *signature)
{
	int error = 0;
	config_setting_t *setting;

	setting = config_lookup(cfg, "client_info.signature");
	if(setting == NULL) {
		error = -1;
		goto out;
	}
	config_setting_set_string(setting, signature);

out:
	return error;
}

int config_up_birthday(config_t *cfg, char *birthday)
{
	int error = 0;
	config_setting_t *setting;

	setting = config_lookup(cfg, "client_info.birthday");
	if(setting == NULL) {
		error = -1;
		goto out;
	}
	config_setting_set_string(setting, birthday);

out:
	return error;
}

int parser_user_self(config_t *cfg)
{
	config_setting_t *setting;
	const char *nickname = NULL;
	const char *per_signature = NULL;
	const char *birthday = NULL;
	const char *ifname = NULL;
	struct user_list *self = NULL;
	int type;

	if (!cfg)
		return -1;
	setting = config_lookup(cfg, "client_info");
	if(setting != NULL) {
	  if((config_setting_lookup_int(setting, "user_type", &type)
		   && config_setting_lookup_string(setting, "nickname", &nickname)
		   && config_setting_lookup_string(setting, "signature", &per_signature)
		   && config_setting_lookup_string(setting, "ifname", &ifname)
		   && config_setting_lookup_string(setting, "birthday", &birthday))) {
		  client.ifname = malloc(strlen(ifname));
		  if (client.ifname)
			  memcpy(client.ifname, ifname, strlen(ifname));

		  self = user_list_init_user((char *)nickname, 0);
		  if (!self)
			  return -1;
		  self->user->sex = type;
		  memcpy(self->user->signature, per_signature, strlen(per_signature) + 1);
		  memcpy(self->user->birthday, birthday, strlen(birthday) + 1);

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
	  const char *ip = NULL;
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

int config_parser(const char *path, struct client *client)
{
	config_t cfg;

	config_init(&cfg);

	if (!path || !client)
		return -1;

	client->cfg_path = (char *)path;
	/* Read the file. If there is an error, report it and exit. */
	if (!config_read_file(&cfg, path)) {
		fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
				config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		return -1;
	}

	if (parser_server_info(&cfg, &client->info)) {
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

int update_name_config(char *name, char *configfile)
{
	config_t *pcfg = NULL;
	int error = 0;

	pcfg = get_config_root(configfile);
	if (!pcfg)
	  return -1;
	/*return zero on successed*/
	error = config_up_nickname(pcfg, name);
	if (error)
	  return error;
	cofig_up_file(pcfg, configfile);
	config_destroy(pcfg);

	return error;
}

int update_signature_config(char *signature, char *configfile)
{
	config_t *pcfg = NULL;
	int error = 0;

	pcfg = get_config_root(configfile);
	if (!pcfg)
	  return -1;
	/*return zero on successed*/
	error = config_up_signature(pcfg, signature);
	if (error)
	  return error;
	cofig_up_file(pcfg, configfile);
	config_destroy(pcfg);

	return error;
}

int update_birthday_config(char *birthday, char *configfile)
{
	config_t *pcfg = NULL;
	int error = 0;

	pcfg = get_config_root(configfile);
	if (!pcfg)
	  return -1;
	/*return zero on successed*/
	error = config_up_birthday(pcfg, birthday);
	if (error)
	  return error;
	cofig_up_file(pcfg, configfile);
	config_destroy(pcfg);

	return error;
}

int update_sex_config(uint8_t sex, char *configfile)
{
	config_t *pcfg = NULL;
	int error = 0;

	pcfg = get_config_root(configfile);
	if (!pcfg)
	  return -1;
	/*return zero on successed*/
	error = config_up_user_type(pcfg, (int)sex);
	if (error)
	  return error;
	cofig_up_file(pcfg, configfile);
	config_destroy(pcfg);

	return error;
}

int update_serverip_config(char *ip, char *configfile)
{
	config_t *pcfg = NULL;
	int error = 0;

	pcfg = get_config_root(configfile);
	if (!pcfg)
	  return -1;
	/*return zero on successed*/
	error = config_up_servip(pcfg, ip);
	if (error)
	  return error;
	cofig_up_file(pcfg, configfile);
	config_destroy(pcfg);

	return error;
}

int update_serverport_config(uint16_t port, char *configfile)
{
	config_t *pcfg = NULL;
	int error = 0;

	pcfg = get_config_root(configfile);
	if (!pcfg)
	  return -1;
	/*return zero on successed*/
	error = config_up_servport(pcfg, port);
	if (error)
	  return error;
	cofig_up_file(pcfg, configfile);
	config_destroy(pcfg);

	return error;
}

#if 0
int main(int argc, char **argv)
{
  config_t cfg, *pcfg;
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
      if((config_setting_lookup_string(setting, "serverip", (const char **)&ip)
           && config_setting_lookup_int(setting, "serverport", &serverport)
           && config_setting_lookup_int(setting, "local_port", &local_port)
           && config_setting_lookup_int(setting, "port_local", &port_local))) {
		  printf("IP is %s\nserverport is %d\nlocal_port is %d\nport_local is %d\n",
				  ip, serverport, local_port, port_local);
	  }
  }

  config_destroy(&cfg);

  /*test for modify configure*/
  pcfg = get_config_root("init.cfg");
  /*return zero on successed*/
  printf("%d\n",config_up_servip(pcfg, "192.168.7.104"));
  printf("%d\n",config_up_nickname(pcfg, "donggebadao"));
  printf("%d\n",config_up_user_type(pcfg, 4));
  cofig_up_file(pcfg, "init.cfg");
  config_destroy(pcfg);
  return(EXIT_SUCCESS);
}
#endif
