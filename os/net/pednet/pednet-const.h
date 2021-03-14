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
 *	Constants for RPL
 * \author
 *	Joakim Eriksson <joakime@sics.se> & Nicolas Tsiftes <nvt@sics.se>,
 *  Simon Duquennoy <simon.duquennoy@inria.fr>
 *
 */

#ifndef PEDNET_CONST_H
#define PEDNET_CONST_H

/********** Constants **********/

#define PED_MESSAGES                0x05
#define PED_FAIL_DETECT             0x07

/************************************************/
#define PED_INPUT_ERROR             0x01
#define PED_INPUT_MSG               0x00

/************************************************/



//////////////////////
#define PED_CODE_NODE_NEW              0x04   /* ism Node new message */
#define PED_CODE_ROUTER_NEW            0x05   /* ism Router new message */
#define PED_CODE_ER_ADDR_PARAM         0x06   /* ism ER address parameter message EAP */
#define PED_CODE_ROUTER_ADDR_PARAM     0x07   /* ism Router address parameter message RAP */
#define PED_CODE_NODE_NEW_ACK          0x08   /* ism Node new acknowledgement message */
#define PED_CODE_ROUTER_NEW_ACK        0x09   /* ism Router new acknowledgement message */
///////////////////////


/* ism PED Failure detection messages */
#define PED_FAIL_NODE_ALIVE                     0x0A
#define PED_FAIL_NODE_UNREACHABLE               0x0B
#define PED_FAIL_NODE_MOBILE                    0x0C
#define PED_FAIL_NODE_FAILED                    0x0D
#define PED_FAIL_ROUTER_ALIVE                   0x0E
#define PED_FAIL_ROUTER_UNREACHABLE             0x0F
///////////////////////////////////////////
#define PED_HANDLER_CODE_ANY                    0XFF

/* ism PED header positions */
#define PED_HDR_TYPE_POS                        0
#define PED_FAIL_NODE_NUM                       2



/****************************************************/


 /** @} */

#endif /* PED_CONST_H */
