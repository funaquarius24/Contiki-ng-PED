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
#include "net/pednet/pednet-er.h"
#include "net/pednet/pednet-routing.h"
#include "net/pednet/pednet-in-out.h"


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
pednet_er_instance_t curr_pednet_er_instance;
er_addr_param_t er_addr_param_structure;
//uip_ds6_addr_t *lladdr;

static void router_new_input(void);
static void node_failed_input(void);
static void node_mobile_input(void);
static void node_unreachable_input(void);
static void router_alive_input(void);

PED_HANDLER(router_new_handler, PED_MESSAGES, PED_CODE_ROUTER_NEW, router_new_input);
PED_HANDLER(node_failed_handler, PED_MESSAGES, PED_FAIL_NODE_FAILED, node_failed_input);
PED_HANDLER(node_mobile_handler, PED_MESSAGES, PED_FAIL_NODE_MOBILE, node_mobile_input);
PED_HANDLER(node_unreachable_handler, PED_FAIL_DETECT, PED_FAIL_NODE_UNREACHABLE, node_unreachable_input);
PED_HANDLER(router_alive_handler, PED_FAIL_DETECT, PED_FAIL_ROUTER_ALIVE, router_alive_input);
PED_HANDLER(router_unreachable_handler, PED_FAIL_DETECT, PED_FAIL_ROUTER_UNREACHABLE, router_unreachable_input);



void 
ped_er_init(void){
  LOG_INFO("ER INIT REACHED");
  uip_create_unspecified(&er_addr_param_structure.prefix);

  ped_timers_init();
  //curr_pednet_instance = NULL;
  ped_register_input_handler(&router_new_handler);
  ped_register_input_handler(&node_failed_handler);
  ped_register_input_handler(&node_mobile_handler);
  ped_register_input_handler(&node_unreachable_handler);
  ped_register_input_handler(&router_alive_handler);
  ped_register_input_handler(&router_unreachable_handler);

    
}

void ped_er_addr_param_output(uip_ipaddr_t *addr){
  LOG_DBG("Er addr param output. \n");
  uip_ds6_addr_t *lladdr = uip_ds6_get_link_local(-1);
  /* if(lladdr == NULL){
    LOG_INFO("NUUULLLLLLLLLLLL \n");
    return;
  } */
  uip_ipaddr_t* local_addr = &lladdr->ipaddr;
  
  unsigned char *buffer;

  PED_PACKET_BUF -> type = PED_MESSAGES;
  PED_PACKET_BUF -> icode = PED_CODE_ER_ADDR_PARAM;
  
  memset(PED_PACKET_BUF -> reserved, 0, sizeof(PED_PACKET_BUF -> reserved));

  uip_create_linklocal_allrouters_mcast(&PED_PACKET_BUF -> destipaddr);
  PED_PACKET_BUF -> srcipaddr = *local_addr;

  er_addr_param_structure.cser = curr_pednet_instance.cser;

  if(uip_is_addr_unspecified(&curr_pednet_instance.prefix)){
    uip_create_unspecified(&er_addr_param_structure.prefix);    
  }else
  {
    er_addr_param_structure.prefix = curr_pednet_instance.prefix;
  }
  

  // LOG_INFO_6ADDR(&er_addr_param_structure.prefix);
  
  PED_PACKET_BUF -> srcipaddr = *local_addr;
  
  buffer = PED_PACKET_PAYLOAD;

  memcpy(buffer, &er_addr_param_structure, sizeof(er_addr_param_t));
  ped_send(addr, PED_MESSAGES, PED_CODE_ER_ADDR_PARAM, sizeof(er_addr_param_t));
}


void ped_router_new_ack_output(uip_ipaddr_t *addr)
{
  LOG_DBG("Router new ack output \n");
  uip_ds6_addr_t *lladdr = uip_ds6_get_link_local(-1);
  uip_ipaddr_t* local_addr = &lladdr->ipaddr;
  
  unsigned char *buffer;

  PED_PACKET_BUF -> type = PED_MESSAGES;
  PED_PACKET_BUF -> icode = PED_CODE_ROUTER_NEW_ACK;
  
  memset(PED_PACKET_BUF -> reserved, 0, sizeof(PED_PACKET_BUF -> reserved));
  
  PED_PACKET_BUF -> destipaddr = *addr;
  PED_PACKET_BUF -> srcipaddr = *local_addr;

  buffer = PED_PACKET_PAYLOAD;

  buffer[0] = buffer[1] = 0;
  ped_send(addr, PED_MESSAGES, PED_CODE_ROUTER_NEW_ACK, 2);
}

static void 
router_new_input()
{
  LOG_DBG("Router new input \n");
  uint8_t *buffer = PED_PACKET_PAYLOAD;
  uint16_t tmp = 0;

  memcpy(&tmp, buffer, sizeof(uint16_t));

  if(!curr_pednet_er_instance.initialized){
    // LOG_INFO("Data: %u", tmp);
    if(tmp > curr_pednet_instance.max_node_num){
      LOG_DBG("TMP: %u  max_num: %u \n", tmp, curr_pednet_instance.max_node_num);
      curr_pednet_instance.max_node_num = tmp;
    }
    LOG_DBG("TMP: %u  max_num: %u \n", tmp, curr_pednet_instance.max_node_num);
    
    
  }
  else
  {
    if(tmp > curr_pednet_instance.max_node_num){
      LOG_DBG("TMP: %u  max_num: %u \n", tmp, curr_pednet_instance.max_node_num);
      curr_pednet_instance.max_node_num = tmp;
      generate_cser();
      ped_timers_schedule_periodic_eap();
    }
    LOG_DBG("Initialized ER already \n");
  }

  // ped_router_new_ack_output(&PED_PACKET_BUF->srcipaddr);
}
void
node_failed_input(){
  LOG_DBG("node_failed_input \n");
}
void
node_mobile_input(){
  LOG_DBG("node_failed_input \n");
}
void
node_unreachable_input(){
  LOG_DBG("node_unreachable_input \n");
}
void 
router_alive_input(){
  LOG_DBG("router_alive_input \n");
}


/*---------------------------------------------------------------------------*/
void
ped_er_set_prefix(uip_ipaddr_t *prefix, uip_ipaddr_t *iid)
{
  static uint8_t initialized = 0;

  if(!initialized) {
    ped_set_global_address(prefix, iid);
    initialized = 1;
  }
}
/*---------------------------------------------------------------------------*/
int
ped_er_start(void)
{

  return 0;

  
}


const struct pednet_driver pednet_er_routing_driver = {
    "PEDNET_ER",
    ped_er_init,
    ped_er_start,
    ped_node_is_er,
    ped_node_is_r,
    ped_node_is_leaf,
    ped_er_set_prefix,
    ped_get_er_ipaddr,
    ped_leave_network,
    ped_node_has_joined,
    ped_node_is_reachable,
    ped_link_callback,
    neighbor_state_changed,
    drop_route,

};
