/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
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
 * \addtogroup ped-lite
 * @{
 *
 * \file
 *	PED types and macros
 * \author
 *	Joakim Eriksson <joakime@sics.se> & Nicolas Tsiftes <nvt@sics.se>
 *  Simon Duquennoy <simon.duquennoy@inria.fr>
 *
 */

#ifndef PEDNET_TYPES_H
#define PEDNET_TYPES_H

#include "pednet-conf.h"

 /********** Macros **********/

#define PED_PACKET_BUF                             ((struct ped_packet_hdr *)ped_buf)
#define PED_PACKET_PAYLOAD                          ((unsigned char *)ped_buf + PED_HDR_LEN)

#define uip_is_addr_same(a, b)                               \
  ((((a)->u16[0]) == ((b)->u16[0])) &&                       \
   (((a)->u16[1]) == ((b)->u16[0])) &&                       \
   (((a)->u16[2]) == ((b)->u16[0])) &&                       \
   (((a)->u16[3]) == ((b)->u16[0])) &&                       \
   (((a)->u16[4]) == ((b)->u16[0])) &&                       \
   (((a)->u16[5]) == ((b)->u16[0])) &&                       \
   (((a)->u16[6]) == ((b)->u16[0])) &&                       \
   (((a)->u16[7]) == ((b)->u16[0])))


/* Multicast address: create and compare */

/** \brief Set IP address addr to the link-local, all-ped-nodes
   multicast address. */
/* #define uip_create_linklocal_pednodes_mcast(addr)	\
  uip_ip6addr((addr), 0xff02, 0, 0, 0, 0, 0, 0, 0x001a)
 */
/** \brief Is IPv6 address addr the link-local, all-PED-nodes
   multicast address? */
/* #define uip_is_addr_linklocal_pednodes_mcast(addr)	    \
  ((addr)->u8[0] == 0xff) &&				    \
  ((addr)->u8[1] == 0x02) &&				    \
  ((addr)->u16[1] == 0) &&				    \
  ((addr)->u16[2] == 0) &&				    \
  ((addr)->u16[3] == 0) &&				    \
  ((addr)->u16[4] == 0) &&				    \
  ((addr)->u16[5] == 0) &&				    \
  ((addr)->u16[6] == 0) &&				    \
  ((addr)->u8[14] == 0) &&				    \
  ((addr)->u8[15] == 0x1a))
 */

typedef struct ped_iid_addr {
  uint16_t router_id;
  uint16_t node_id;
  uint8_t icode;
} ped_iid_addr_t;


/* handle input messages */
typedef struct pednet_input_handler {
  struct pednet_input_handler *next;
  uint8_t type;
  uint8_t icode;
  void (*handler)(void);
} pednet_input_handler_t;

/* */
struct pednet_instance {
  uint8_t num_routers;
  uint8_t max_node_num;
  struct ctimer state_update;
  struct ctimer leave;
  uint8_t csr;
  uint8_t cser;
  uip_ipaddr_t prefix;
};
typedef struct pednet_instance pednet_instance_t;

struct ped_packet_hdr {
  uint8_t type, icode;
  uint8_t len[2];
  uint8_t reserved[4];
  uip_ip6addr_t srcipaddr, destipaddr;
};

typedef union {
  uint32_t u32[(PED_PAYLOAD_SIZE + 3) / 4];
  uint8_t u8[PED_PAYLOAD_SIZE];
} ped_buf_t;

ped_buf_t ped_aligned_buf;

#define ped_buf (ped_aligned_buf.u8)

struct er_addr_param
{
  uint16_t cser;
  uip_ipaddr_t prefix;
};
typedef struct er_addr_param er_addr_param_t;

struct r_addr_param
{
  uint16_t router_uid;
  uip_ipaddr_t prefix;
  uint16_t cser;
  uint16_t csr;
};
typedef struct r_addr_param r_addr_param_t;



struct pednet_er_instance {
  uint8_t initialized;
  struct ctimer ER_addr_config_timer;
  struct ctimer periodic_addr_change_timer;
  uint8_t eap_delay;
  uint8_t eap_int_min;
  uint8_t eap_counter;
  uint8_t eap_intdbl;

};
typedef struct pednet_er_instance pednet_er_instance_t;

struct pednet_r_instance {
  uint8_t initialized;
  uint8_t configured;
  struct ctimer R_addr_config_timer;
  struct ctimer Node_check_timer;
  struct ctimer router_new_timer;
  uint8_t rap_delay;
  uint8_t node_check_delay;
  uint8_t router_new_delay;
  uint8_t rap_int_min;
  uint8_t rap_counter;
  uint8_t rap_intdbl;

};
typedef struct pednet_r_instance pednet_r_instance_t;

struct pednet_leaf_instance {
  uint8_t initialized;
  uint8_t configured;
  struct ctimer leaf_timer;
  struct ctimer node_new_timer;
  uint8_t node_new_delay;
  uint16_t router_uid;
};
typedef struct pednet_leaf_instance pednet_leaf_instance_t;

struct ped_router_node_ids
{
  uint16_t router_id;
  uint16_t node_id;
};
typedef struct ped_router_node_ids ped_router_node_ids_t;

 /** @} */

#endif /* PED_TYPES_H */
