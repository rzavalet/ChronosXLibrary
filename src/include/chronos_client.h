#ifndef _CHRONOS_CLIENT_H_
#define _CHRONOS_CLIENT_H_

#include "chronos_packets.h"
#include "chronos_environment.h"

typedef void *CHRONOS_CONN_H;

CHRONOS_ENV_H
chronosClientEnvGet(CHRONOS_CONN_H connH);

CHRONOS_CONN_H
chronosConnHandleAlloc(CHRONOS_ENV_H envH);

int
chronosConnHandleFree(CHRONOS_CONN_H connH);

int
chronosClientConnect(const char *serverAddress,
                     int serverPort,
                     const char *connName,
                     CHRONOS_CONN_H connH);

int
chronosClientDisconnect(CHRONOS_CONN_H connH);

int
chronosClientSendRequest(CHRONOS_REQUEST_H    requestH,
                         CHRONOS_CONN_H connH);

int
chronosClientReceiveResponse(int *txn_rc_ret, 
                             CHRONOS_CONN_H connH, 
                             int (*isTimeToDieFp) (void));

#endif
