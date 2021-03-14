/*******************************************
 * 
 * 
 * 
 * 
 * **********************************************/ 

#ifndef PEDNET_ROUTING_H
#define PEDNET_ROUTING_H

#include "contiki.h"
#include "net/ipv6/uip.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-sr.h"
#include "net/linkaddr.h"

#include "net/pednet/pednet-types.h"


#define PED_HANDLER(name, type, code, func) \
  static pednet_input_handler_t name = { NULL, type, code, func }

//extern uip_ds6_addr_t *lladdr;

void test_send();

void
ped_send(const uip_ipaddr_t *dest, int type, int code, int payload_len);

void
ped_set_global_address(uip_ipaddr_t *prefix, uip_ipaddr_t *iid);

void 
generate_cser();

void 
generate_csr();

// static 
int ped_node_is_er(void);

//static 
int
ped_node_is_r(void);

//static 
int
ped_node_is_leaf(void);

uint8_t
ped_input(uint8_t type, uint8_t icode);


void
ped_register_input_handler(pednet_input_handler_t *handler);



const uip_ipaddr_t *
ped_get_global_address(void);
/*---------------------------------------------------------------------------*/
void
ped_link_callback(const linkaddr_t *addr, int status, int numtx);
/*---------------------------------------------------------------------------*/
int
ped_node_has_joined(void);
/*---------------------------------------------------------------------------*/
int
ped_node_is_reachable(void);

int 
ped_get_er_ipaddr(uip_ipaddr_t *addr);

void 
ped_leave_network(void);

void
neighbor_state_changed(uip_ds6_nbr_t *nbr);

void
drop_route(uip_ds6_route_t *route);

void update_random_num();

uint8_t ped_addr_in_class(const linkaddr_t *linkaddr);


extern pednet_instance_t curr_pednet_instance;
extern pednet_leaf_instance_t curr_pednet_leaf_instance;
extern pednet_er_instance_t curr_pednet_er_instance;
extern pednet_r_instance_t curr_pednet_r_instance;
extern r_addr_param_t r_addr_param_structure;
extern er_addr_param_t er_addr_param_structure;
extern uint8_t ped_rand_num;
extern uip_ipaddr_t ped_node_ipaddr;
extern uip_ipaddr_t ped_node_global_addr;
extern uint16_t ped_curr_node_id;
extern uint16_t ped_curr_router_id;


struct pednet_driver
{
    char *name;
    void (* init)(void);
    int (* root_start)(void);
    int (* node_is_ER)(void);
    int (* node_is_R)(void);
    int (* node_is_leaf)(void);
    void (* set_prefix)(uip_ipaddr_t *prefix, uip_ipaddr_t *iid);
    int (* get_er_ipaddr)(uip_ipaddr_t *ipaddr);
    void (* leave_network)(void);
    int (* node_has_joined)(void);
    int (* node_is_reachable)(void);
    void (* link_callback)(const linkaddr_t *addr, int status, int numtx);
    void (* neighbor_state_changed)(uip_ds6_nbr_t *nbr);
    void (* drop_route)(uip_ds6_route_t *route);
    uint8_t (* addr_in_class)(const linkaddr_t *linkaddr);

};




#endif
