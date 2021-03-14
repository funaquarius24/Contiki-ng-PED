/*****************************************************************************
 * 
 * 
 * 
 * 
 * 
 * ****************************************************************************/


#include "contiki.h"
#include "contiki-net.h"


#include "net/pednet/pednet.h"
#include "net/pednet/pednet-conf.h"
#include "net/pednet/pednet-const.h"
#include "net/pednet/pednet-types.h"
#include "net/pednet/pednet-timers.h"
#include "net/pednet/pednet-leaf.h"
#include "net/pednet/pednet-routing.h"

#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-ds6.h"

#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-sr.h"
#include "net/ipv6/uip.h"
#include "sys/node-id.h"



/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "PED"
#define LOG_LEVEL LOG_LEVEL_PED

// hhhhh
pednet_instance_t curr_pednet_instance;
pednet_leaf_instance_t curr_pednet_leaf_instance;
r_addr_param_t r_addr_param_structure;
uip_ipaddr_t ped_node_ipaddr;
uip_ipaddr_t ped_node_global_addr;
uip_lladdr_t ped_node_linkaddr;
uint16_t ped_curr_node_id;
uint16_t ped_curr_router_id;

static void r_addr_param_input(void);
static void node_new_ack_input(void);
static void node_unreachable_input(void);

PED_HANDLER(r_addr_param_handler, PED_MESSAGES, PED_CODE_ROUTER_ADDR_PARAM, r_addr_param_input);
PED_HANDLER(node_new_ack_handler, PED_MESSAGES, PED_CODE_NODE_NEW_ACK, node_new_ack_input);
PED_HANDLER(node_unreachable_handler, PED_FAIL_DETECT, PED_FAIL_NODE_UNREACHABLE, node_unreachable_input);



void 
ped_leaf_init(void){
  LOG_INFO("LEAF INIT REACHED \n");

  ped_timers_init();
  update_random_num();
  ped_curr_node_id = node_id;
  //curr_pednet_instance = NULL;
  ped_register_input_handler(&r_addr_param_handler);
  ped_register_input_handler(&node_new_ack_handler);
  ped_register_input_handler(&node_unreachable_handler);

  //uip_create_unspecified(&ped_node_ipaddr);
  
}

void
change_leaf_node_address(){/* 
  uint16_t cser = curr_pednet_instance.cser;
  uint16_t csr = curr_pednet_instance.csr; */
  LOG_DBG("change_leaf_node_address \n");
  uip_ipaddr_t last_ipaddr = ped_node_ipaddr;
  
  ped_curr_router_id = r_addr_param_structure.router_uid + r_addr_param_structure.cser * ped_rand_num;
  ped_curr_node_id = node_id + r_addr_param_structure.csr * ped_rand_num;
  
  memcpy(&ped_node_ipaddr, &uip_ds6_get_link_local(-1)->ipaddr, 8);
  
  ped_node_ipaddr.u16[6] = UIP_HTONS(ped_curr_router_id);
  ped_node_ipaddr.u16[7] = UIP_HTONS(ped_curr_node_id);
  uip_ds6_set_lladdr_from_iid(&ped_node_linkaddr, &ped_node_ipaddr);

  LOG_DBG("r_id: %u, node_id: %u \n", ped_curr_router_id, ped_curr_node_id);
  linkaddr_copy(&linkaddr_node_addr, (linkaddr_t *)&ped_node_linkaddr);

  
  if(!uip_is_addr_unspecified(&ped_node_ipaddr)){
    if(uip_ds6_addr_lookup(&last_ipaddr)){
      LOG_DBG("REMOVING LAST ADDR :");
      LOG_DBG_6ADDR(&last_ipaddr);
      LOG_DBG("\n");
      uip_ds6_addr_rm(uip_ds6_addr_lookup(&last_ipaddr));
      
    }

    if(uip_ds6_addr_lookup(&ped_node_ipaddr) == NULL) {
      LOG_DBG("adding link local address ");
      LOG_DBG_6ADDR(&ped_node_ipaddr);
      LOG_DBG("\n");
      uip_ds6_addr_add(&ped_node_ipaddr, 0, ADDR_AUTOCONF);
    }

    
    if(!(uip_is_addr_unspecified(&curr_pednet_instance.prefix) || 
            uip_is_addr_linklocal(&curr_pednet_instance.prefix)) ){
      memcpy(&ped_node_global_addr, &ped_node_ipaddr, sizeof(uip_ipaddr_t));
      
      memcpy(&ped_node_global_addr, &curr_pednet_instance.prefix, 8);

      uip_ds6_addr_t *ipaddr = uip_ds6_get_global(-1);
      if(ipaddr){
        uip_ds6_addr_rm(ipaddr);
      }
      LOG_INFO("adding global address ");
      LOG_INFO_6ADDR(&ped_node_global_addr);
      LOG_INFO_("\n");
      uip_ds6_addr_add(&ped_node_global_addr, 0, ADDR_AUTOCONF);

    }


  }
  // uip_ds6_send_rs();
  LOG_INFO("PAC: Address changed! \n");
}

void 
r_addr_param_input()
{
  LOG_DBG("r_addr_param_input \n");
  uint8_t *buffer;
  buffer = PED_PACKET_PAYLOAD;
  memcpy(&r_addr_param_structure, buffer, sizeof(r_addr_param_t));

  LOG_INFO("prefix: ");
  LOG_INFO_6ADDR(&curr_pednet_instance.prefix);
  LOG_INFO("\n");

  // LOG_INFO("ID1: %u  ID2: %u \n", r_addr_param_structure.router_uid, curr_pednet_leaf_instance.router_uid);
  if(!curr_pednet_leaf_instance.configured){
    // LOG_INFO("r_addr_param_structure.cser: %u", r_addr_param_structure.cser);
    if(r_addr_param_structure.cser > 1){
      curr_pednet_leaf_instance.configured = 1;
      curr_pednet_instance.cser = r_addr_param_structure.cser;
      curr_pednet_instance.csr = r_addr_param_structure.csr;
      curr_pednet_leaf_instance.router_uid = r_addr_param_structure.router_uid;

      ped_node_ipaddr = uip_ds6_get_link_local(-1)->ipaddr;

      LOG_INFO("INIT NODEIPADDR: ");
      LOG_INFO_6ADDR(&ped_node_ipaddr);
      LOG_INFO("\n");

      change_leaf_node_address();
      curr_pednet_leaf_instance.configured = 1;
    }
    
  }else{
    // LOG_INFO("addr_struct_uid: %u curr_uid: %u \n", r_addr_param_structure.router_uid, curr_pednet_leaf_instance.router_uid);
    if(r_addr_param_structure.router_uid == curr_pednet_leaf_instance.router_uid){
      if(!(curr_pednet_instance.cser == r_addr_param_structure.cser &&
          curr_pednet_instance.csr == r_addr_param_structure.csr) ){

        curr_pednet_instance.cser = r_addr_param_structure.cser;
        curr_pednet_instance.csr = r_addr_param_structure.csr;

        if(!(uip_is_addr_unspecified(&r_addr_param_structure.prefix) || 
            uip_is_addr_linklocal(&r_addr_param_structure.prefix)) ){
              curr_pednet_instance.prefix = r_addr_param_structure.prefix;
        }


        change_leaf_node_address();
      }
      
    }
  }
    
  

}
void
node_new_ack_input()
{
  LOG_DBG("node_new_ack_input \n");
  node_new_delay_change(0);
  
  // LOG_DBG("ADDR: ");
  // LOG_DBG_6ADDR(&PED_PACKET_BUF->destipaddr);
  // LOG_DBG("\n");

}
void
node_unreachable_input()
{

}

/*--------------------------------------------------------------------*/
void ped_node_new_output(uip_ipaddr_t *addr){
  LOG_DBG("Node new output id: %u\n", node_id);
  
  uip_ds6_addr_t *lladdr = uip_ds6_get_link_local(-1);
  uip_ipaddr_t* local_addr = &lladdr->ipaddr;
  
  unsigned char *buffer;

  PED_PACKET_BUF -> type = PED_MESSAGES;
  PED_PACKET_BUF -> icode = PED_CODE_NODE_NEW;
  
  memset(PED_PACKET_BUF -> reserved, 0, sizeof(PED_PACKET_BUF -> reserved));
  
  uip_create_linklocal_allrouters_mcast(&PED_PACKET_BUF -> destipaddr);
  PED_PACKET_BUF -> srcipaddr = *local_addr;
  
  buffer = PED_PACKET_PAYLOAD;
  buffer[0] = node_id;
  ped_send(NULL, PED_MESSAGES, PED_CODE_NODE_NEW, 2);
}

/*---------------------------------------------------------------------*/
void
ped_leaf_set_prefix(uip_ipaddr_t *prefix, uip_ipaddr_t *iid)
{
  static uint8_t initialized = 0;

  if(!initialized) {
    ped_set_global_address(prefix, iid);
    initialized = 1;
  }
}
/*---------------------------------------------------------------------------*/
int
ped_leaf_start(void)
{
  //struct uip_ds6_addr *root_if;
  //int i;
  //uint8_t state;
  //uip_ipaddr_t *ipaddr = NULL;

  //ped_leaf_set_prefix(NULL, NULL);

  return 0;

  
}



const struct pednet_driver pednet_leaf_routing_driver = {
    "PEDNET_LEAF",
    ped_leaf_init,
    ped_leaf_start,
    ped_node_is_er,
    ped_node_is_r,
    ped_node_is_leaf,
    ped_leaf_set_prefix,
    ped_get_er_ipaddr,
    ped_leave_network,
    ped_node_has_joined,
    ped_node_is_reachable,
    ped_link_callback,
    neighbor_state_changed,
    drop_route,

};
