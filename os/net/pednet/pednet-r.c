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
#include "net/pednet/pednet-r.h"
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
pednet_r_instance_t curr_pednet_r_instance;
er_addr_param_t er_addr_param_structure;
uip_lladdr_t ped_node_linkaddr;
uip_ipaddr_t ped_node_global_addr;

//uip_ds6_addr_t *lladdr;

static void node_new_input(void);
static void router_new_ack_input(void);
static void er_addr_param_input(void);
static void node_alive_input(void);
static void node_unreachable_input(void);
static void node_mobile_input(void);
static void router_unreachable_input(void);

void
change_r_node_address();

PED_HANDLER(node_new_handler, PED_MESSAGES, PED_CODE_NODE_NEW, node_new_input);
PED_HANDLER(router_new_ack_handler, PED_MESSAGES, PED_CODE_ROUTER_NEW_ACK, router_new_ack_input);
PED_HANDLER(er_addr_param_handler, PED_MESSAGES, PED_CODE_ER_ADDR_PARAM, er_addr_param_input);
PED_HANDLER(node_alive_handler, PED_FAIL_DETECT, PED_FAIL_NODE_ALIVE, node_alive_input);
PED_HANDLER(node_unreachable_handler, PED_FAIL_DETECT, PED_FAIL_NODE_UNREACHABLE, node_unreachable_input);
PED_HANDLER(node_mobile_handler, PED_FAIL_DETECT, PED_FAIL_NODE_MOBILE, node_mobile_input);
PED_HANDLER(router_unreachable_handler, PED_FAIL_DETECT, PED_FAIL_ROUTER_UNREACHABLE, router_unreachable_input);



void 
ped_r_init(void){
  LOG_INFO("R INIT REACHED \n");

  ped_timers_init();
  //curr_pednet_instance = NULL;
  ped_register_input_handler(&node_new_handler);
  ped_register_input_handler(&router_new_ack_handler);
  ped_register_input_handler(&er_addr_param_handler);
  ped_register_input_handler(&node_alive_handler);
  ped_register_input_handler(&node_unreachable_handler);
  ped_register_input_handler(&node_mobile_handler);
  ped_register_input_handler(&router_unreachable_handler);

    
}

void
ped_r_addr_param_output(uip_ipaddr_t *addr){
  LOG_DBG("ped_r_addr_param_output ID: %u \n", node_id);

  uip_ds6_addr_t *lladdr = uip_ds6_get_link_local(-1);
  uip_ipaddr_t* local_addr = &lladdr->ipaddr;
  
  unsigned char *buffer;

  PED_PACKET_BUF -> type = PED_MESSAGES;
  PED_PACKET_BUF -> icode = PED_CODE_ROUTER_ADDR_PARAM;

  PED_PACKET_BUF -> srcipaddr = *local_addr;
    
  r_addr_param_structure.cser = curr_pednet_instance.cser;
  r_addr_param_structure.csr = curr_pednet_instance.csr;
  r_addr_param_structure.router_uid = node_id;

  if(uip_is_addr_unspecified(&curr_pednet_instance.prefix)){
    uip_create_unspecified(&r_addr_param_structure.prefix);
    
  }
  else
  {
    r_addr_param_structure.prefix = curr_pednet_instance.prefix;
    LOG_DBG("prfix to send: ");
    LOG_INFO_6ADDR(&r_addr_param_structure.prefix);
    LOG_INFO("\n");
  }
  
  
  buffer = PED_PACKET_PAYLOAD;
  memcpy(buffer, &r_addr_param_structure, sizeof(r_addr_param_t));
  ped_send(addr, PED_MESSAGES, PED_CODE_ROUTER_ADDR_PARAM, sizeof(r_addr_param_t));
  
}

void
ped_process_eap(uip_ipaddr_t *from)
{
  
}
/*---------------------------------------------------------------------------*/
void
ped_process_node_new(uip_ipaddr_t *from, int is_multicast)
{
  if(is_multicast) {
    //rpl_timers_dio_reset("Multicast DIS");
  } else {
    /* Add neighbor to cache and reply to the unicast DIS with a unicast DIO*/
    // if(rpl_icmp6_update_nbr_table(from, NBR_TABLE_REASON_RPL_DIS, NULL) != NULL) {
    //   LOG_INFO("unicast DIS, reply to sender\n");
    //   //rpl_icmp6_dio_output(from);
    // }
  }
}

void
ped_router_new_output(uip_ipaddr_t *addr){
  LOG_DBG("Router new output id: %u \n", node_id);
  uip_ds6_addr_t *lladdr = uip_ds6_get_link_local(-1);
  /* if(lladdr == NULL){
    LOG_INFO("NUUULLLLLLLLLLLL \n");
    return;
  } */
  uip_ipaddr_t* local_addr = &lladdr->ipaddr;
  
  unsigned char *buffer;

  PED_PACKET_BUF -> type = PED_MESSAGES;
  PED_PACKET_BUF -> icode = PED_CODE_ROUTER_NEW;
  
  memset(PED_PACKET_BUF -> reserved, 0, sizeof(PED_PACKET_BUF -> reserved));
  
  uip_create_linklocal_allrouters_mcast(&PED_PACKET_BUF -> destipaddr);
  PED_PACKET_BUF -> srcipaddr = *local_addr;
  
  buffer = PED_PACKET_PAYLOAD;
  buffer[0] = node_id;
  ped_send(addr, PED_MESSAGES, PED_CODE_ROUTER_NEW, 2);
}

void 
ped_node_new_ack_output(uip_ipaddr_t *addr){
  LOG_DBG("Node new ack output \n");
  uip_ds6_addr_t *lladdr = uip_ds6_get_link_local(-1);
  uip_ipaddr_t* local_addr = &lladdr->ipaddr;
  
  unsigned char *buffer;

  PED_PACKET_BUF -> type = PED_MESSAGES;
  PED_PACKET_BUF -> icode = PED_CODE_NODE_NEW_ACK;
  
  memset(PED_PACKET_BUF -> reserved, 0, sizeof(PED_PACKET_BUF -> reserved));
  
  PED_PACKET_BUF -> destipaddr = *addr;
  PED_PACKET_BUF -> srcipaddr = *local_addr;

  buffer = PED_PACKET_PAYLOAD;
  buffer[0] = buffer[1] = 0;
  ped_send(addr, PED_MESSAGES, PED_CODE_NODE_NEW_ACK, 2);
}

void 
router_new_ack_input()
{
  LOG_DBG("Router new ack received \n");
  router_new_delay_change(0);
}

void
node_alive_input(){}

static void
node_new_input(){
  uint8_t *buffer = PED_PACKET_PAYLOAD;
  uint16_t test = 0;

  memcpy(&test, buffer, sizeof(uint16_t));

  LOG_DBG("Node new received %u\n", test);

  if(!curr_pednet_r_instance.initialized){
    
    if(test > curr_pednet_instance.max_node_num){
      curr_pednet_instance.max_node_num = test;
    }
        
  }
  else
  {
    if(test > (curr_pednet_instance.max_node_num)){
      curr_pednet_instance.max_node_num = test;
      update_random_num();
      generate_csr();
      change_r_node_address();
    }
    ped_node_new_ack_output(&PED_PACKET_BUF->srcipaddr);
        
  }

  ped_node_new_ack_output(&PED_PACKET_BUF->srcipaddr);
  
  
}

uint8_t
ped_list_loop(uip_ds6_element_t *list, uint8_t size,
                  uint16_t elementsize, uip_ipaddr_t *ipaddr,
                  uint8_t ipaddrlen, uip_ds6_element_t **out_element)
{
  uip_ds6_element_t *element;

  if(list == NULL || ipaddr == NULL || out_element == NULL) {
    return NOSPACE;
  }

  *out_element = NULL;

  for(element = list;
      element <
      (uip_ds6_element_t *)((uint8_t *)list + (size * elementsize));
      element = (uip_ds6_element_t *)((uint8_t *)element + elementsize)) {
    if(element->isused) {
      if(uip_ipaddr_prefixcmp(&element->ipaddr, ipaddr, ipaddrlen)) {
        *out_element = element;
        LOG_INFO_6ADDR(&element->ipaddr);
        LOG_INFO(",,");
        return FOUND;
      }
      LOG_INFO_6ADDR(&element->ipaddr);
      LOG_INFO(",,");
    } else {
      LOG_INFO_6ADDR(&element->ipaddr);
      LOG_INFO(",,");
      *out_element = element;
    }
  }

  return *out_element != NULL ? FREESPACE : NOSPACE;
}

void
change_r_node_address(){
  uip_ipaddr_t last_ipaddr = ped_node_ipaddr;
  
  ped_curr_router_id = node_id + curr_pednet_instance.cser * ped_rand_num;
  
  // LOG_INFO("id: %u, cser: %u, rand: %u \n", node_id, curr_pednet_instance.cser, ped_rand_num);
  
  uint16_t dummy_node_id = curr_pednet_instance.csr - 1;
  ped_curr_node_id = dummy_node_id + curr_pednet_instance.csr * ped_rand_num;
  
  memcpy(&ped_node_ipaddr, &uip_ds6_get_link_local(-1)->ipaddr, 8);

  uip_ds6_addr_t *test_ds6 = (uip_ds6_get_global(-1));
  uip_ipaddr_t ipaddr;
  uip_create_unspecified(&ipaddr);

  LOG_INFO("Showing global: ");
  if(ped_list_loop
     ((uip_ds6_element_t *)uip_ds6_if.addr_list, UIP_DS6_ADDR_NB,
      sizeof(uip_ds6_addr_t), &ipaddr, 128,
      (uip_ds6_element_t **)&test_ds6) == FOUND) {
    
    
    LOG_INFO_6ADDR(&test_ds6->ipaddr);
  }
  LOG_INFO("\n");


  

  ped_node_ipaddr.u16[6] = UIP_HTONS(ped_curr_router_id);
  ped_node_ipaddr.u16[7] = UIP_HTONS(ped_curr_node_id);
  uip_ds6_set_lladdr_from_iid(&ped_node_linkaddr, &ped_node_ipaddr);

  //LOG_INFO("CSER %u, csr: %u, r_id: %u, node_id: %u \n", cser, csr, ped_router_id, ped_node_id);
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
  LOG_INFO("PAC: Address changed! Cser: %u\n", curr_pednet_instance.cser);
}



void
er_addr_param_input(){
  LOG_DBG("er_addr_param_input... \n");
  uint8_t *buffer;
  buffer = PED_PACKET_PAYLOAD;

  memcpy(&er_addr_param_structure, buffer, sizeof(er_addr_param_t));

  LOG_DBG("PREFIX: ");
  LOG_DBG_6ADDR(&er_addr_param_structure.prefix);
  LOG_DBG("\n");
  
  if(!curr_pednet_r_instance.configured){
    // LOG_INFO("!configured \n");

    curr_pednet_instance.cser = er_addr_param_structure.cser;
    curr_pednet_instance.prefix = er_addr_param_structure.prefix;

    ped_node_ipaddr = uip_ds6_get_link_local(-1)->ipaddr;

    LOG_INFO("INIT NODEIPADDR: ");
    LOG_INFO_6ADDR(&ped_node_ipaddr);
    LOG_INFO("\n");

    ped_curr_node_id = node_id;
    update_random_num();
    change_r_node_address();

    curr_pednet_r_instance.configured = 1;
    
  }else{
    
    if(!(curr_pednet_instance.cser == er_addr_param_structure.cser)){
      update_random_num();

      ped_node_ipaddr = uip_ds6_get_link_local(-1)->ipaddr;
      LOG_INFO("INIT NODEIPADDR: ");
      LOG_INFO_6ADDR(&ped_node_ipaddr);
      LOG_INFO("\n");

      LOG_DBG("curr: %u struct: %u \n", curr_pednet_instance.cser , er_addr_param_structure.cser);

      curr_pednet_instance.cser = er_addr_param_structure.cser;
      curr_pednet_instance.prefix = er_addr_param_structure.prefix;

      
      change_r_node_address();
      ped_timers_schedule_periodic_rap();
    
    }
    
  }
}

void
node_mobile_input(){

}
void
node_unreachable_input(){}
void
router_unreachable_input(){}

/*---------------------------------------------------------------------------*/
void
ped_r_set_prefix(uip_ipaddr_t *prefix, uip_ipaddr_t *iid)
{
  static uint8_t initialized = 0;

  if(!initialized) {
    ped_set_global_address(prefix, iid);
    initialized = 1;
  }
}
/*---------------------------------------------------------------------------*/
int
ped_r_start(void)
{
  //struct uip_ds6_addr *root_if;
  //int i;
  //uint8_t state;
  //uip_ipaddr_t *ipaddr = NULL;

  //ped_r_set_prefix(NULL, NULL);

  return 0;

  
}

const struct pednet_driver pednet_r_routing_driver = {
    "PEDNET_R",
    ped_r_init,
    ped_r_start,
    ped_node_is_er,
    ped_node_is_r,
    ped_node_is_leaf,
    ped_r_set_prefix,
    ped_get_er_ipaddr,
    ped_leave_network,
    ped_node_has_joined,
    ped_node_is_reachable,
    ped_link_callback,
    neighbor_state_changed,
    drop_route,
    ped_addr_in_class,

};
