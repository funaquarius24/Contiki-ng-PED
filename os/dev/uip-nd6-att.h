/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
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
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \addtogroup uip
 * @{
 */

/**
 * \file
 *    Header file for IPv6 Neighbor discovery (RFC 4861)
 * \author Julien Abeille <jabeille@cisco.com>
 * \author Mathilde Durvy <mdurvy@cisco.com>
 */

#ifndef UIP_ND6_ATT_H_
#define UIP_ND6_ATT_H_

#include "net/ipv6/uip.h"
#include "sys/stimer.h"
/**
 *  \name General
 * @{
 */
/** \brief HOP LIMIT to be used when sending ND messages (255) */
#define UIP_ND6_HOP_LIMIT               255
/** \brief INFINITE lifetime */
#define UIP_ND6_INFINITE_LIFETIME       0xFFFFFFFF
/** @} */

/** \name RFC 4861 Host constant */
/** @{ */
/** \brief Maximum router solicitation delay */
#ifndef UIP_CONF_ND6_MAX_RTR_SOLICITATION_DELAY
#define UIP_ND6_MAX_RTR_SOLICITATION_DELAY 1
#else
#define UIP_ND6_MAX_RTR_SOLICITATION_DELAY UIP_CONF_ND6_MAX_RTR_SOLICITATION_DELAY
#endif
/** \brief Router solicitation interval */
#ifndef UIP_CONF_ND6_RTR_SOLICITATION_INTERVAL
#define UIP_ND6_RTR_SOLICITATION_INTERVAL  4
#else
#define UIP_ND6_RTR_SOLICITATION_INTERVAL  UIP_CONF_ND6_RTR_SOLICITATION_INTERVAL
#endif
/** \brief Maximum router solicitations */
#ifndef UIP_CONF_ND6_MAX_RTR_SOLICITATIONS
#define UIP_ND6_MAX_RTR_SOLICITATIONS      3
#else
#define UIP_ND6_MAX_RTR_SOLICITATIONS      UIP_CONF_ND6_MAX_RTR_SOLICITATIONS
#endif
/** @} */

/** \name RFC 4861 Router constants */
/** @{ */
#ifndef UIP_CONF_ND6_SEND_RA
#define UIP_ND6_SEND_RA                     1   /* enable/disable RA sending */
#else
#define UIP_ND6_SEND_RA UIP_CONF_ND6_SEND_RA
#endif
#ifndef UIP_CONF_ND6_SEND_NS
#define UIP_ND6_SEND_NS                     1   /* enable/disable NS sending */
#else
#define UIP_ND6_SEND_NS UIP_CONF_ND6_SEND_NS
#endif
#ifndef UIP_CONF_ND6_SEND_NA
#define UIP_ND6_SEND_NA                     1   /* enable/disable NA sending */
#else
#define UIP_ND6_SEND_NA UIP_CONF_ND6_SEND_NA
#endif
#ifndef UIP_CONF_ND6_AUTOFILL_NBR_CACHE
/* Neighbor not found in cache? Derive its link-layer address from it's
link-local IPv6, assuming it used autoconfiguration. This is not
standard-compliant but this is a convenient way to keep the
neighbor cache out of the way in cases ND is not used.
Note that this is not standard-compliant (RFC 4861), as neighbors will
be added regardless of their reachability and liveness.  */
#define UIP_ND6_AUTOFILL_NBR_CACHE          0
#else
#define UIP_ND6_AUTOFILL_NBR_CACHE UIP_CONF_ND6_AUTOFILL_NBR_CACHE
#endif
#ifndef UIP_CONF_ND6_MAX_RA_INTERVAL
#define UIP_ND6_MAX_RA_INTERVAL             600
#else
#define UIP_ND6_MAX_RA_INTERVAL             UIP_CONF_ND6_MAX_RA_INTERVAL
#endif
#ifndef UIP_CONF_ND6_MIN_RA_INTERVAL
#define UIP_ND6_MIN_RA_INTERVAL             (UIP_ND6_MAX_RA_INTERVAL / 3)
#else
#define UIP_ND6_MIN_RA_INTERVAL             UIP_CONF_ND6_MIN_RA_INTERVAL
#endif
#define UIP_ND6_M_FLAG                      0
#define UIP_ND6_O_FLAG                      0
#ifndef UIP_CONF_ROUTER_LIFETIME
#define UIP_ND6_ROUTER_LIFETIME             3 * UIP_ND6_MAX_RA_INTERVAL
#else
#define UIP_ND6_ROUTER_LIFETIME             UIP_CONF_ROUTER_LIFETIME
#endif

#define UIP_ND6_MAX_INITIAL_RA_INTERVAL     16  /*seconds*/
#define UIP_ND6_MAX_INITIAL_RAS             3   /*transmissions*/
#ifndef UIP_CONF_ND6_MIN_DELAY_BETWEEN_RAS
#define UIP_ND6_MIN_DELAY_BETWEEN_RAS       3   /*seconds*/
#else
#define UIP_ND6_MIN_DELAY_BETWEEN_RAS       UIP_CONF_ND6_MIN_DELAY_BETWEEN_RAS
#endif
//#define UIP_ND6_MAX_RA_DELAY_TIME           0.5 /*seconds*/
#define UIP_ND6_MAX_RA_DELAY_TIME_MS        500 /*milli seconds*/
/** @} */

#ifndef UIP_CONF_ND6_DEF_MAXDADNS
/** \brief Do not try DAD when using EUI-64 as allowed by draft-ietf-6lowpan-nd-15 section 8.2 */
#if UIP_CONF_LL_802154
#define UIP_ND6_DEF_MAXDADNS 0
#else /* UIP_CONF_LL_802154 */
#define UIP_ND6_DEF_MAXDADNS UIP_ND6_SEND_NS
#endif /* UIP_CONF_LL_802154 */
#else /* UIP_CONF_ND6_DEF_MAXDADNS */
#define UIP_ND6_DEF_MAXDADNS UIP_CONF_ND6_DEF_MAXDADNS
#endif /* UIP_CONF_ND6_DEF_MAXDADNS */

/** \name RFC 4861 Node constant */
#define UIP_ND6_MAX_MULTICAST_SOLICIT  3

#ifdef UIP_CONF_ND6_MAX_UNICAST_SOLICIT
#define UIP_ND6_MAX_UNICAST_SOLICIT    UIP_CONF_ND6_MAX_UNICAST_SOLICIT
#else /* UIP_CONF_ND6_MAX_UNICAST_SOLICIT */
#define UIP_ND6_MAX_UNICAST_SOLICIT    3
#endif /* UIP_CONF_ND6_MAX_UNICAST_SOLICIT */

#ifdef UIP_CONF_ND6_REACHABLE_TIME
#define UIP_ND6_REACHABLE_TIME         UIP_CONF_ND6_REACHABLE_TIME
#else
#define UIP_ND6_REACHABLE_TIME         60000
#endif

#ifdef UIP_CONF_ND6_RETRANS_TIMER
#define UIP_ND6_RETRANS_TIMER          UIP_CONF_ND6_RETRANS_TIMER
#else
#define UIP_ND6_RETRANS_TIMER          1000
#endif

#define UIP_ND6_DELAY_FIRST_PROBE_TIME 5
#define UIP_ND6_MIN_RANDOM_FACTOR(x)   (x / 2)
#define UIP_ND6_MAX_RANDOM_FACTOR(x)   ((x) + (x) / 2)
/** @} */


/** \name RFC 6106 RA DNS Options Constants  */
/** @{ */
#ifndef UIP_CONF_ND6_RA_RDNSS
#define UIP_ND6_RA_RDNSS                0
#else
#define UIP_ND6_RA_RDNSS                UIP_CONF_ND6_RA_RDNSS
#endif

#ifndef UIP_CONF_ND6_RA_DNSSL
#define UIP_ND6_RA_DNSSL                0
#else
#error Not implemented
#define UIP_ND6_RA_DNSSL                UIP_CONF_ND6_RA_DNSSL
#endif
/** @} */


/** \name ND6 option types */
/** @{ */
#define UIP_ND6_OPT_SLLAO               1
#define UIP_ND6_OPT_TLLAO               2
#define UIP_ND6_OPT_PREFIX_INFO         3
#define UIP_ND6_OPT_REDIRECTED_HDR      4
#define UIP_ND6_OPT_MTU                 5
#define UIP_ND6_OPT_RDNSS               25
#define UIP_ND6_OPT_DNSSL               31
/** @} */

/** \name ND6 option types */
/** @{ */
#define UIP_ND6_OPT_TYPE_OFFSET         0
#define UIP_ND6_OPT_LEN_OFFSET          1
#define UIP_ND6_OPT_DATA_OFFSET         2

/** \name ND6 message length (excluding options) */
/** @{ */
#define UIP_ND6_NA_LEN                  20
#define UIP_ND6_NS_LEN                  20
#define UIP_ND6_RA_LEN                  12
#define UIP_ND6_RS_LEN                  4
/** @} */


/** \name ND6 option length in bytes */
/** @{ */
#define UIP_ND6_OPT_HDR_LEN            2
#define UIP_ND6_OPT_PREFIX_INFO_LEN    32
#define UIP_ND6_OPT_MTU_LEN            8
#define UIP_ND6_OPT_RDNSS_LEN          1
#define UIP_ND6_OPT_DNSSL_LEN          1


/* Length of TLLAO and SLLAO options, it is L2 dependant */
#if UIP_CONF_LL_802154
/* If the interface is 802.15.4. For now we use only long addresses */
#define UIP_ND6_OPT_SHORT_LLAO_LEN     8
#define UIP_ND6_OPT_LONG_LLAO_LEN      16
/** \brief length of a ND6 LLAO option for 802.15.4 */
#define UIP_ND6_OPT_LLAO_LEN UIP_ND6_OPT_LONG_LLAO_LEN
#else /*UIP_CONF_LL_802154*/
#if UIP_CONF_LL_80211
/* If the interface is 802.11 */
/** \brief length of a ND6 LLAO option for 802.11 */
#define UIP_ND6_OPT_LLAO_LEN           8
#else /*UIP_CONF_LL_80211*/
/** \brief length of a ND6 LLAO option for default L2 type (e.g. Ethernet) */
#define UIP_ND6_OPT_LLAO_LEN           8
#endif /*UIP_CONF_LL_80211*/
#endif /*UIP_CONF_LL_802154*/
/** @} */


/** \name Neighbor Advertisement flags masks */
/** @{ */
#define UIP_ND6_NA_FLAG_ROUTER          0x80
#define UIP_ND6_NA_FLAG_SOLICITED       0x40
#define UIP_ND6_NA_FLAG_OVERRIDE        0x20
#define UIP_ND6_RA_FLAG_ONLINK          0x80
#define UIP_ND6_RA_FLAG_AUTONOMOUS      0x40
/** @} */

struct attack_target_info{
  uip_ipaddr_t ipaddr;
  linkaddr_t lladdr;
} ;
typedef struct attack_target_info attack_target_t;
extern attack_target_t attack_target;

/**
 * \name ND message structures
 * @{
 */


void
uip_nd6_ns_output(uip_ipaddr_t *src, uip_ipaddr_t *dest, uip_ipaddr_t *tgt);


/**
 * \brief send a Router Advertisement
 *
 * Only for router, for periodic as well as sollicited RA
 */
void uip_nd6_ra_output(uip_ipaddr_t *dest);

/**
 * \brief Send a Router Solicitation
 *
 * src is chosen through the uip_netif_select_src function. If src is
 * unspecified  (i.e. we do not have a preferred address yet), then we do not
 * put a SLLAO option (MUST NOT in RFC 4861). Otherwise we do.
 *
 * RS message format,
 * possible option is SLLAO, MUST NOT be included if source = unspecified
 * SHOULD be included otherwise
 */
void uip_nd6_rs_output(void);

/**
 * \brief Initialise the uIP ND core
 */
void uip_nd6_att_init(void);
/** @} */

void change_attack_node_address(void);
void print_attk_tgt_addr(void);
attack_target_t get_attack_target(void);


void
uip_appserver_addr_get(uip_ipaddr_t *ipaddr);
/*--------------------------------------*/
/******* ANNEX - message formats ********/
/*--------------------------------------*/

/*
 * RS format. possible option is SLLAO
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                            Reserved                           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Options ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-
 *
 *
 * RA format. possible options: prefix information, MTU, SLLAO
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    | Cur Hop Limit |M|O|  Reserved |       Router Lifetime         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                         Reachable Time                        |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                          Retrans Timer                        |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Options ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-
 *
 *
 * NS format: options should be SLLAO
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                           Reserved                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                       Target Address                          +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Options ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-
 *
 *
 * NA message format. possible options is TLLAO
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |R|S|O|                     Reserved                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                       Target Address                          +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Options ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-
 *
 *
 * Redirect message format. Possible options are TLLAO and Redirected header
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                           Reserved                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                       Target Address                          +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                     Destination Address                       +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Options ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-
 *
 *
 * SLLAO/TLLAO option:
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |    Length     |    Link-Layer Address ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 * Prefix information option
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |    Length     | Prefix Length |L|A| Reserved1 |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                         Valid Lifetime                        |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                       Preferred Lifetime                      |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                           Reserved2                           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                            Prefix                             +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 * MTU option
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |    Length     |           Reserved            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                              MTU                              |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 * Redirected header option
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |    Length     |            Reserved           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                           Reserved                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    ~                       IP header + data                        ~
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */
#endif /* UIP_ND6_H_ */

/** @} */
