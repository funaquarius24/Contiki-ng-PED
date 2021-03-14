#define LOG_CONF_LEVEL_IPV6                        LOG_LEVEL_DBG
//#define LOG_CONF_LEVEL_RPL                         LOG_LEVEL_INFO
// #define LOG_CONF_LEVEL_6LOWPAN                     LOG_LEVEL_INFO
//#define LOG_CONF_LEVEL_TCPIP                       LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_MAC                         LOG_LEVEL_INFO
//#define LOG_CONF_LEVEL_FRAMER                      LOG_LEVEL_INFO
//#define LOG_CONF_LEVEL_PED                         LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_App                         LOG_LEVEL_INFO

#define UIP_CONF_ND6_SEND_RA                     1   /* enable/disable RA sending */
#define UIP_CONF_ND6_SEND_NS                     1   /* enable/disable NS sending */
#define UIP_CONF_ND6_SEND_NA                     1   /* enable/disable NA sending */

#define UIP_CONF_ROUTER 0


#define PED_CONF_NODE_TYPE                         3  // attacker