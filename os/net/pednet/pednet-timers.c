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
 */

/**
 * \addtogroup ped
 * @{
 *
 * \file
 *         RPL timer management.
 *
 * \author Joakim Eriksson <joakime@sics.se>, Nicolas Tsiftes <nvt@sics.se>,
 * Simon Duquennoy <simon.duquennoy@inria.fr>
 */

#include "contiki.h"

#include "net/ipv6/uip-sr.h"
#include "net/link-stats.h"
#include "lib/random.h"
#include "sys/ctimer.h"

#include "net/pednet/pednet-types.h"
#include "net/pednet/pednet-routing.h"
#include "net/pednet/pednet-er.h"
#include "net/pednet/pednet-r.h"
#include "net/pednet/pednet-leaf.h"

#include "uip-ds6-nbr.h"
#include "nbr-table.h"



/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "PED"
#define LOG_LEVEL LOG_LEVEL_PED

#define PERIODIC_DELAY_SECONDS     60
#define DEFAULT_DELAY              30

/* change this to the number of seconds for eap and rap to be sent */
#define DEFAULT_EAP_DELAY          DEFAULT_DELAY 
#define DEFAULT_RAP_DELAY          DEFAULT_DELAY

/*
 Continue sending node new and router new with this until acknowledge message is received
 */
#define DEFAULT_NODE_NEW_DELAY     5
#define DEFAULT_ROUTER_NEW_DELAY   5 


#define PERIODIC_DELAY             ((PERIODIC_DELAY_SECONDS) * CLOCK_SECOND)
#define R_INIT_DELAY               (5 * CLOCK_SECOND) /* Number of seconds * clock_second */
#define ER_INIT_DELAY              (10 * CLOCK_SECOND) /* Number of seconds * clock_second */
#define N_MAX_DELAY                200 /* This is going to be in milliseconds */

/*
* Node new and router new messages will be sent this number of seconds in order to inform the upstream
* that they are alive
*/
#define NODE_NEW_DELAY             (20 * CLOCK_SECOND) /* Number of seconds * clock_second */
#define ROUTER_NEW_DELAY           (20 * CLOCK_SECOND) /* Number of seconds * clock_second */

#define ADDR_CHANGE_PERIOD          (240 * CLOCK_SECOND) // periodic addr change in seconds

void handle_node_new_timer(void *ptr);
void handle_router_new_timer(void *ptr);
void handle_router_new_ack_timer(void *ptr);
void handle_node_new_ack_timer(void *ptr);
void handle_eap_timer(void *ptr);
void handle_rap_timer(void *ptr);
void handle_pac_timer(void *ptr);


static void handle_periodic_timer(void *ptr);
static struct ctimer periodic_timer; /* Not part of a DAG because used for general state maintenance */


static struct ctimer ER_init_timer = {};
static struct ctimer R_init_timer = {};
static struct ctimer N_init_timer = {};

pednet_leaf_instance_t curr_pednet_leaf_instance;
pednet_r_instance_t curr_pednet_r_instance;
pednet_er_instance_t curr_pednet_er_instance;



void
ped_timers_schedule_periodic_node_new(void)
{
  LOG_DBG("SCHEDULE Node New \n");
  if(ctimer_expired(&curr_pednet_leaf_instance.node_new_timer)) {
    uint8_t delay = curr_pednet_leaf_instance.node_new_delay;
    clock_time_t expiration_time = delay * CLOCK_SECOND / 2 + (random_rand() % (delay * CLOCK_SECOND));
    ctimer_set(&curr_pednet_leaf_instance.node_new_timer, expiration_time, handle_node_new_timer, NULL);
  }
}

void
ped_timers_schedule_periodic_router_new(void)
{
  LOG_DBG("SCHEDULE Router New \n");
  if(ctimer_expired(&curr_pednet_r_instance.router_new_timer)) {
    uint8_t delay = curr_pednet_r_instance.router_new_delay;
    clock_time_t expiration_time = delay * CLOCK_SECOND / 2 + (random_rand() % (delay * CLOCK_SECOND));
    ctimer_set(&curr_pednet_r_instance.router_new_timer, expiration_time, handle_router_new_timer, NULL);
  }
}

void
ped_timers_schedule_periodic_eap(){
  LOG_DBG("SCHEDULE eap \n");
  if(ctimer_expired(&curr_pednet_er_instance.ER_addr_config_timer)) {
    uint8_t delay = curr_pednet_er_instance.eap_delay;
    clock_time_t expiration_time = delay * CLOCK_SECOND / 2 + (random_rand() % (delay * CLOCK_SECOND));
    ctimer_set(&curr_pednet_er_instance.ER_addr_config_timer, expiration_time, handle_eap_timer, NULL);
  }

}

void
ped_timers_schedule_pac(){
  LOG_DBG("SCHEDULE periodic addr change \n");
  if(ctimer_expired(&curr_pednet_er_instance.periodic_addr_change_timer)) {
    clock_time_t expiration_time = ADDR_CHANGE_PERIOD / 2 + (random_rand() % (ADDR_CHANGE_PERIOD / 2));
    ctimer_set(&curr_pednet_er_instance.periodic_addr_change_timer, expiration_time, handle_pac_timer, NULL);
  }
}

void
ped_timers_schedule_periodic_rap(){
  LOG_DBG("SCHEDULE rap \n");
  if(ctimer_expired(&curr_pednet_r_instance.R_addr_config_timer)) {
    uint8_t delay = curr_pednet_r_instance.rap_delay;
    clock_time_t expiration_time = delay * CLOCK_SECOND / 2 + (random_rand() % (delay * CLOCK_SECOND));
    ctimer_set(&curr_pednet_r_instance.R_addr_config_timer, expiration_time, handle_rap_timer, NULL);
  }
}

/*---------------------------------------------------------------------------*/
void
handle_node_new_timer(void *ptr)
{
  LOG_DBG("REACHED .....Node New.......\n");
  ped_node_new_output(NULL);
  ped_timers_schedule_periodic_node_new();
}

void
handle_router_new_timer(void *ptr)
{
  LOG_DBG("REACHED .....Router New.......\n");
  ped_router_new_output(NULL);
  ped_timers_schedule_periodic_router_new();

}

void
handle_router_new_ack_timer(void *ptr)
{
  LOG_DBG("REACHED .....Router New.......\n");
  //ped_router_new_output(NULL);
}

void
handle_eap_timer(void *ptr)
{
  LOG_DBG("REACHED .....eap.......\n");
  ped_er_addr_param_output(NULL);
  ped_timers_schedule_periodic_eap();

}

void
handle_pac_timer(void *ptr){ // handle periodic addr change
  LOG_INFO("PAC: PERIODIC ADDR CHANGE\n");
  generate_cser();
  ped_timers_schedule_periodic_eap();
  ped_timers_schedule_pac();
}

void
handle_rap_timer(void *ptr)
{
  LOG_DBG("REACHED .....rap.......\n");
  ped_r_addr_param_output(NULL);
  ped_timers_schedule_periodic_rap();
}

void
handle_periodic_timer(void *ptr)
{
  LOG_DBG("PERIODIC TIMER \n");

  //ctimer_reset(&periodic_timer);
  //handle_router_new_ack_timer(NULL);
  //handle_node_new_ack_timer(NULL);
  //handle_eap_timer(NULL);
  //handle_rap_timer(NULL);
}

void 
handle_er_timer()
{
  LOG_DBG("handle_Er_timer \n");
  if(!curr_pednet_er_instance.initialized){
    generate_cser();

    ped_er_addr_param_output(NULL);
    curr_pednet_er_instance.initialized = 1;
    ped_timers_schedule_periodic_eap();
    LOG_DBG("GENERATED CSER \n");
  }

  // LOG_INFO("PRINTING ER NBR \n"); 
  // uip_ds6_nbr_t *nbr = uip_ds6_nbr_head();
  // while(nbr != NULL) {
  //     // do something with the neighbor here
  //     // its IP address is nbr->ipaddr
  //     // ...
  //     nbr = uip_ds6_nbr_next(nbr);
  //     LOG_INFO_6ADDR(&nbr->ipaddr);
  //     LOG_INFO("\n");
      
  // }
  // ctimer_set(&ER_init_timer, 60 * CLOCK_SECOND, handle_er_timer, NULL);
  
}

void 
handle_r_timer()
{
  LOG_DBG("handle_r_timer \n");
  if (!curr_pednet_r_instance.initialized)
  {
    generate_csr();
    ped_r_addr_param_output(NULL);
    curr_pednet_r_instance.initialized = 1;
    ped_timers_schedule_periodic_rap();
  }
  else
  {
    ped_r_addr_param_output(NULL);
  }
  
}

void 
handle_n_timer()
{
  LOG_DBG("N INIT \n");
  if(!curr_pednet_leaf_instance.initialized){
    ped_node_new_output(NULL);
    curr_pednet_leaf_instance.initialized = 1;
  }
  ped_timers_schedule_periodic_node_new();
}

void node_new_delay_change(uint8_t reduce_time)
{
  if(reduce_time == 0){
    curr_pednet_leaf_instance.node_new_delay = NODE_NEW_DELAY / CLOCK_SECOND;
  }else
  {
    curr_pednet_leaf_instance.node_new_delay = DEFAULT_NODE_NEW_DELAY;
  }
  
}

void router_new_delay_change(uint8_t reduce_time)
{
  if(reduce_time == 0){
    curr_pednet_r_instance.router_new_delay = ROUTER_NEW_DELAY / CLOCK_SECOND;
  }else
  {
    curr_pednet_r_instance.router_new_delay = DEFAULT_ROUTER_NEW_DELAY;
  }
  
}

void
ped_timers_init(void)
{
  LOG_INFO("INIT TIMERS \n");

  ctimer_set(&periodic_timer, PERIODIC_DELAY, handle_periodic_timer, NULL);
  
  
  if(ped_node_is_er()){
    ctimer_set(&ER_init_timer, ER_INIT_DELAY, handle_er_timer, NULL);
    curr_pednet_er_instance.eap_delay = DEFAULT_EAP_DELAY;

    ped_timers_schedule_pac();
  }
  else if (ped_node_is_r())
  {
    
    ctimer_set(&R_init_timer, R_INIT_DELAY, handle_r_timer, NULL);
    curr_pednet_r_instance.rap_delay = DEFAULT_RAP_DELAY;
    curr_pednet_r_instance.router_new_delay = DEFAULT_ROUTER_NEW_DELAY;
    ped_timers_schedule_periodic_router_new();
  }
  else
  {
    
    uint8_t max_delay = (N_MAX_DELAY / 2) + random_rand() % N_MAX_DELAY;
    ctimer_set(&N_init_timer, max_delay, handle_n_timer, NULL);
    curr_pednet_leaf_instance.node_new_delay = DEFAULT_NODE_NEW_DELAY;
    ped_timers_schedule_periodic_node_new();
  }
  

  
  


}

/** @}*/