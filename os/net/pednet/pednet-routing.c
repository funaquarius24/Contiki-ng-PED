#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"

#include "net/pednet/pednet.h"
#include "net/pednet/pednet-conf.h"
#include "net/pednet/pednet-const.h"
#include "net/pednet/pednet-types.h"
#include "net/pednet/pednet-timers.h"
#include "net/pednet/pednet-routing.h"
//#include "net/pednet/pednet-er.h"

#include "net/ipv6/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uipbuf.h"

#include "lib/random.h"
#include "sys/node-id.h"
//#include "services/deployment/deployment.h"


/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "PED"
#define LOG_LEVEL LOG_LEVEL_PED

//static uint8_t *packetbuf_ptr;

static uint8_t ped_as_er = PED_ER;
static uint8_t ped_as_r = PED_R;
static uint8_t ped_as_leaf = PED_LEAF;
uint8_t ped_rand_num;


LIST(input_handler_list);

void ped_input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest);

/*---------------------------------------------------------------------------*/

static pednet_input_handler_t *
input_handler_lookup(uint8_t type, uint8_t icode)
{
  pednet_input_handler_t *handler = NULL;

  for(handler = list_head(input_handler_list);
      handler != NULL;
      handler = list_item_next(handler)) {
    if(handler->type == type &&
       (handler->icode == icode ||
        handler->icode == PED_HANDLER_CODE_ANY)) {
      return handler;
    }
  }

  return NULL;
}

void update_random_num()
{
  ped_rand_num = random_rand() % 255;
}

void
set_ip_from_prefix(uip_ipaddr_t *ipaddr, uip_ds6_prefix_t *prefix)
{
  memset(ipaddr, 0, sizeof(uip_ipaddr_t));
  memcpy(ipaddr, &prefix->ipaddr, (prefix->length + 7) / 8);
  uip_ds6_set_addr_iid(ipaddr, &uip_lladdr);
}

int
ped_node_is_er(void){
  
  return ped_as_er;
  
}

//static 
int
ped_node_is_r(void){
  //return PED_R ? 1 : 0;
  
  return ped_as_r;
}

//static 
int
ped_node_is_leaf(void){  
  return ped_as_leaf;
}

int
ped_get_er_ipaddr(uip_ipaddr_t *ipaddr)
{
  return 1;
}

const uip_ipaddr_t *
ped_get_global_address(void)
{
  LOG_DBG("gET GLOBAL ADDR");

  
  return NULL;
}

/*********************************************************************************/
uint8_t
ped_input(uint8_t type, uint8_t icode)
{
  pednet_input_handler_t *handler = input_handler_lookup(type, icode);
  LOG_DBG("PED input reached \n");

  if(handler == NULL) {
    return PED_INPUT_ERROR;
  }

  if(handler->handler == NULL) {
    return PED_INPUT_ERROR;
  }
  

  handler->handler();
  return PED_INPUT_MSG;
}
/*---------------------------------------------------------------------------*/

void test_send(){
  // LOG_INFO("OOOKKK..... \n");
  ped_send(NULL, PED_INPUT_MSG, PED_CODE_NODE_NEW, 0);
}

uint8_t ped_addr_in_class(const linkaddr_t *linkaddr)
{
  // linkaddr_t *addr_to_ckeck;
  // addr_to_ckeck = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  // uint8_t ped_node_id = linkaddr_node_addr.u8[LINKADDR_SIZE - 1]
  //           + (linkaddr_node_addr.u8[LINKADDR_SIZE - 2] << 8);

  return 0;
  
            
}

void
ped_send(const uip_ipaddr_t *dest, int type, int code, int payload_len) 
{
  uip_lladdr_t *linkaddr;

  if(payload_len == 0){
    LOG_DBG("No Payload. Exit \n");
    return;
  }else if(payload_len > PED_PAYLOAD_SIZE)
  {
    LOG_DBG("Too large payload. Exit \n");
  }

  if(uip_is_addr_mcast(&PED_PACKET_BUF->destipaddr)) {
    linkaddr = NULL;
  }
  else
  {
    uip_lladdr_t lladdr;
    uip_ds6_set_lladdr_from_iid(&lladdr, &PED_PACKET_BUF->destipaddr);
    linkaddr = &lladdr;
  }
  if(node_id == 1){
    uint16_t tested = 0;

    memcpy(&tested, PED_PACKET_PAYLOAD, sizeof(uint16_t));

    LOG_INFO("CHECK CSER %u", tested);
  }
  

  pednet_buf = (uint8_t *)PED_PACKET_BUF;
  pednet_len = PED_HDR_LEN + payload_len;
  

  NETSTACK_NETWORK1.output((const linkaddr_t *)linkaddr);

  
}

void 
generate_cser(){
  uint8_t max = curr_pednet_instance.max_node_num;
  LOG_DBG("MAX ROUTER: %u ", max);

  curr_pednet_instance.cser = max + (random_rand() % (255 - max));
  LOG_DBG("TIMER CSER: %u \n", curr_pednet_instance.cser);

}

void 
generate_csr(){
  uint8_t max = curr_pednet_instance.max_node_num + 2;
  LOG_DBG("MAX NODE: %u ", max);

  curr_pednet_instance.csr = max + (random_rand() % (255 - max));
  LOG_DBG("TIMER CSR: %u \n", curr_pednet_instance.csr);
  
}

/*------------------------------------------------------------------------*/
void
ped_set_global_address(uip_ipaddr_t *prefix, uip_ipaddr_t *iid)
{
  static uip_ipaddr_t root_ipaddr;
  const uip_ipaddr_t *default_prefix;
  int i;
  uint8_t state;

  default_prefix = uip_ds6_default_prefix();

  /* Assign a unique local address (RFC4193,
     http://tools.ietf.org/html/rfc4193). */
  if(prefix == NULL) {
    uip_ip6addr_copy(&root_ipaddr, default_prefix);
  } else {
    memcpy(&root_ipaddr, prefix, 8);
  }
  if(iid == NULL) {
    uip_ds6_set_addr_iid(&root_ipaddr, &uip_lladdr);
  } else {
    memcpy(((uint8_t*)&root_ipaddr) + 8, ((uint8_t*)iid) + 8, 8);
  }

  uip_ds6_addr_add(&root_ipaddr, 0, ADDR_AUTOCONF);
  curr_pednet_instance.prefix = *prefix;

  LOG_INFO("IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      LOG_INFO("-- ");
      LOG_INFO_6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      LOG_INFO_("\n");
    }
  }
}
/*------------------------------------------------------------------------*/
void
ped_link_callback(const linkaddr_t *addr, int status, int numtx)
{
  
      /* Link stats were updated, and we need to update our internal state.
      Updating from here is unsafe; postpone */
      LOG_INFO("packet sent to ");
      LOG_INFO_LLADDR(addr);
      LOG_INFO("\n");
      
    
  
}
/*---------------------------------------------------------------------------*/

int
ped_node_has_joined(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
int
ped_node_is_reachable(void)
{
  return 0;
}

void 
ped_leave_network(void){

}

//static 
void
neighbor_state_changed(uip_ds6_nbr_t *nbr)
{
  /* Nothing needs be done in non-storing mode */
}

//static 
void
drop_route(uip_ds6_route_t *route)
{
  /* Do nothing. RPL-lite only supports non-storing mode, i.e. no routes */
}

void
ped_router_unreachable_output(uip_ipaddr_t *addr)
{

}

void
ped_register_input_handler(pednet_input_handler_t *handler)
{
  list_add(input_handler_list, handler);
}
