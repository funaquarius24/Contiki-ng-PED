#ifndef PEDNET_NBR_H
#define PEDNET_NBR_H

#include "net/ipv6/uip-ds6.h"

NBR_TABLE_DECLARE(ped_neighbors);

uip_ds6_nbr_t *
ped_update_nbr_table(uip_ipaddr_t *addr, nbr_table_reason_t reason, void *data);

void
ped_nbr_set_defrt(uip_ipaddr_t *ipaddr);






#endif