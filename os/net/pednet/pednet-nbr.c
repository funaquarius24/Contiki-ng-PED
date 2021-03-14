/*****************************************************
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 ******************************************************/

#include "contiki.h"


#include "net/ipv6/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uipbuf.h"

#include "net/pednet/pednet.h"
#include "net/pednet/pednet-conf.h"
#include "net/pednet/pednet-const.h"
#include "net/pednet/pednet-types.h"
#include "net/pednet/pednet-timers.h"
#include "net/pednet/pednet-routing.h"
#include "net/pednet/pednet-er.h"

#include "net/packetbuf.h"
#include "lib/random.h"
#include "../sys/node-id.h"


#include "sys/log.h"
#define LOG_MODULE "PED"
#define LOG_LEVEL LOG_LEVEL_PED

//NBR_TABLE_GLOBAL(ped_nbr_t, ped_neighbors);


uip_ds6_nbr_t *
ped_update_nbr_table(uip_ipaddr_t *addr, nbr_table_reason_t reason, void *data)
{
  uip_ds6_nbr_t *nbr;

  if(PED_LEAF){
      return NULL;
  }

  if((nbr = uip_ds6_nbr_lookup(addr)) == NULL) {
    if((nbr = uip_ds6_nbr_add(addr, (uip_lladdr_t *)
                              packetbuf_addr(PACKETBUF_ADDR_SENDER),
                              0, NBR_REACHABLE, reason, data)) == NULL) {
      LOG_ERR("could not add neighbor to cache ");
      LOG_ERR_6ADDR(addr);
      LOG_ERR_(", ");
      LOG_ERR_LLADDR(packetbuf_addr(PACKETBUF_ADDR_SENDER));
      LOG_ERR_("\n");
    }
  }

  return nbr;
}

void
ped_nbr_set_defrt(uip_ipaddr_t *ipaddr)
{
    uip_ds6_defrt_clear();
    uip_ds6_defrt_add(ipaddr, 0);

}