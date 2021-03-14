/*
 * Copyright (c) 2017, RISE SICS.
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
 * \file
 *         NullNet, a minimal network layer.
 * \author
 *         Simon Duquennoy <simon.duquennoy@ri.se>
 *
 */

/**
 * \addtogroup pednet
 * @{
 */

#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/pednet/pednet.h"

#include "net/pednet/pednet-conf.h"
#include "net/pednet/pednet-const.h"
#include "net/pednet/pednet-types.h"
#include "net/pednet/pednet-timers.h"
#include "net/pednet/pednet-routing.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "PED"
#define LOG_LEVEL LOG_LEVEL_INFO

uint8_t *pednet_buf;
uint16_t pednet_len;

static pednet_input_callback current_callback = NULL;
/*--------------------------------------------------------------------*/
static void
init(void)
{
  LOG_INFO("pedNet: init\n");
  LOG_INFO("pedNet: name %s\n", PEDNET_ROUTING.name);
  current_callback = NULL;
  //ped_timers_init();
  PEDNET_ROUTING.init();
}
/*--------------------------------------------------------------------*/
static void
input(void)
{
  //uint8_t *buffer;
  uint8_t *packetbuf_ptr = packetbuf_dataptr();
  uint16_t len = packetbuf_datalen();

  if(len > PED_HDR_LEN){
    memcpy(PED_PACKET_BUF, packetbuf_ptr, PED_HDR_LEN);
    
    // LOG_INFO("rece!!!!! OK!!!... \n");
    // LOG_INFO_6ADDR(&PED_PACKET_BUF -> destipaddr);
    // LOG_INFO("\n");

    //buffer = PED_PACKET_BUF;
    if((PED_PACKET_BUF->type == PED_MESSAGES || PED_PACKET_BUF->type == PED_FAIL_DETECT)){
      LOG_DBG("pedNet: received %u bytes from ", packetbuf_datalen());
      LOG_DBG_LLADDR(packetbuf_addr(PACKETBUF_ADDR_SENDER));
      LOG_DBG("\n");

      memcpy(PED_PACKET_PAYLOAD, packetbuf_ptr + PED_HDR_LEN, len - PED_HDR_LEN);

      //ped_input(PED_PACKET_BUF->type, PED_PACKET_BUF->icode);
      if(ped_input(PED_PACKET_BUF->type, PED_PACKET_BUF->icode) == PED_INPUT_ERROR){
        LOG_ERR("INPUT ERROR %u \n", PED_PACKET_BUF->icode);
      }

      if(current_callback != NULL){
        current_callback(packetbuf_dataptr(), packetbuf_datalen(),
        packetbuf_addr(PACKETBUF_ADDR_SENDER), packetbuf_addr(PACKETBUF_ADDR_RECEIVER));
      }
      
    }

  }
  else if (len == PED_HDR_LEN)
  {
    LOG_ERR("No data only header... Drop. \n");
  }
  
  else{
    LOG_ERR("Pkt length < hdr length... Drop. \n");
  }
  
}

/*--------------------------------------------------------------------*/
/** \name Input/output functions common to all compression schemes
 * @{                                                                 */
/*--------------------------------------------------------------------*/
/**
 * Callback function for the MAC packet sent callback
 */
static void
packet_sent(void *ptr, int status, int transmissions)
{
  LOG_DBG("pedNet: packet sent \n");
  //link_stats_packet_sent(dest, status, transmissions);
}
/*--------------------------------------------------------------------*/

void
pednet_set_input_callback(pednet_input_callback callback)
{
 current_callback = callback;
}
/*--------------------------------------------------------------------*/
static uint8_t
output(const linkaddr_t *dest)
{
  packetbuf_clear();
  packetbuf_copyfrom(pednet_buf, pednet_len);
  if(dest != NULL) {
    packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, dest);
  } else {
    packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, &linkaddr_null);
  }
  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_node_addr);
  LOG_DBG("pedNet: sending %u bytes to ", packetbuf_datalen());
  LOG_DBG_LLADDR(packetbuf_addr(PACKETBUF_ADDR_RECEIVER));
  LOG_DBG("\n");
  NETSTACK_MAC.send(&packet_sent, NULL);
  return 1;
}



/*--------------------------------------------------------------------*/
const struct network_driver pednet_driver = {
  "PEDNET",
  init,
  input,
  output
};
/*--------------------------------------------------------------------*/
/** @} */
