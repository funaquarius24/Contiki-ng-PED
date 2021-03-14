#ifndef PEDNET_ER_H
#define PEDNET_ER_H

void 
ped_er_init(void);

int 
ped_er_start();

void
ped_er_addr_param_output(uip_ipaddr_t *addr);


void
ped_er_set_prefix(uip_ipaddr_t *prefix, uip_ipaddr_t *iid);








#endif