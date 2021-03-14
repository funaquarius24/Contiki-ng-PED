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
 * \addtogroup rpl-lite
 * @{
 *
 * \file
 *	Public configuration and API declarations for ContikiRPL.
 * \author
 *	Joakim Eriksson <joakime@sics.se>, Nicolas Tsiftes <nvt@sics.se>,
 *  Simon DUquennoy <simon.duquennoy@inria.fr>
 *
 */

#ifndef PEDNET_CONF_H
#define PEDNET_CONF_H

#include "contiki.h"

/* #ifdef PED_LEAF_CONF
#define PED_LEAF PED_LEAF_CONF
#else
#define PED_LEAF 0
#endif

#ifdef PED_ER_CONF
#define PED_ER PED_ER_CONF
#else
#define PED_ER 0
#endif

#ifdef PED_R_CONF
#define PED_R PED_R_CONF
#else
#define PED_R 0
#endif */

#define PED_PAYLOAD_SIZE                        128
#define PED_HDR_LEN                             40
#define PED_DATA_START_POS                      PED_HDR_LEN

#ifdef PED_CONF_PREFIX_SIZE
#define PED_PREFIX_SIZE                         PED_CONF_PREFIX_SIZE
#else
#define PED_PREFIX_SIZE                         64
#endif


/* Interval of node_new transmission  */
#ifdef RPL_CONF_NODE_NEW_INTERVAL
#define RPL_NODE_NEW_INTERVAL                RPL_CONF_NODE_NEW_INTERVAL
#else
#define RPL_NODE_NEW_INTERVAL                (30 * CLOCK_SECOND)
#endif

/* Interval of router_new transmission  */
#ifdef RPL_CONF_ROUTER_NEW_INTERVAL
#define RPL_ROUTER_NEW_INTERVAL                RPL_CONF_ROUTER_NEW_INTERVAL
#else
#define RPL_ROUTER_NEW_INTERVAL                (30 * CLOCK_SECOND)
#endif

#ifdef RPL_CONF_EAP_INTERVAL_MIN
#define RPL_EAP_INTERVAL_MIN        RPL_CONF_EAP_INTERVAL_MIN
#else
#define RPL_EAP_INTERVAL_MIN        12
#endif /* RPL_CONF_EAP_INTERVAL_MIN */

#ifdef RPL_CONF_RAP_INTERVAL_MIN
#define RPL_RAP_INTERVAL_MIN        RPL_CONF_RAP_INTERVAL_MIN
#else
#define RPL_RAP_INTERVAL_MIN        12
#endif /* RPL_CONF_RAP_INTERVAL_MIN */

#ifdef RPL_CONF_EAP_INTERVAL_DOUBLINGS
#define RPL_EAP_INTERVAL_DOUBLINGS  RPL_CONF_EAP_INTERVAL_DOUBLINGS
#else
#define RPL_EAP_INTERVAL_DOUBLINGS  8
#endif /* RPL_CONF_EAP_INTERVAL_DOUBLINGS */

#ifdef RPL_CONF_RAP_INTERVAL_DOUBLINGS
#define RPL_RAP_INTERVAL_DOUBLINGS  RPL_CONF_RAP_INTERVAL_DOUBLINGS
#else
#define RPL_RAP_INTERVAL_DOUBLINGS  8
#endif /* RPL_CONF_EAP_INTERVAL_DOUBLINGS */




/** @} */

#endif /* PED_CONF_H */
