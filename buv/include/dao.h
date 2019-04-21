/* 
 * Copyright (c) 2018 nsyszr.io. All rights reserved.
 */

#ifndef BUV_DAO_H_
#define BUV_DAO_H_

#include <sqlite3.h>

#include "brand.h"
#include "vote.h"
#include "user_vote.h"

typedef struct dao_config dao_config_t;

int dao_alloc(dao_config_t **, const char *);
void dao_free(dao_config_t *);
int dao_db_open(dao_config_t *);
int dao_db_close(dao_config_t *);
/*int dao_create_vpn_client(dao_config_t *, const char *, const char *, 
    const char *, const char *, const char *);
int dao_vpn_client_find_by_cn(dao_config_t *, const char *, 
    struct vpn_client *);
int dao_vpn_client_network_find_by_client_id(dao_config_t *, int, vector_t *);*/

#endif /* BUV_DAO_H_ */