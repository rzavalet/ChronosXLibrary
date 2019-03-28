#ifndef _CHRONOS_PACKETS_H_
#define _CHRONOS_PACKETS_H_

#include "chronos_transactions.h"
#include "chronos_cache.h"
#include "chronos_environment.h"
#include <stdlib.h>

#define CHRONOS_REQUEST_PACKET_SIZE (100)

typedef struct chronosResponsePacket_t {
  chronosUserTransaction_t txn_type;
  int rc;
} chronosResponsePacket_t;

typedef struct chronosRequestPacket_t {
  int magic;

  chronosUserTransaction_t txn_type;

  /* A transaction can affect up to 100 symbols */
  int numItems;
  union {
    chronosViewPortfolioInfo_t portfolioInfo[CHRONOS_REQUEST_PACKET_SIZE];
    chronosSymbol_t            symbolInfo[CHRONOS_REQUEST_PACKET_SIZE];
    chronosPurchaseInfo_t      purchaseInfo[CHRONOS_REQUEST_PACKET_SIZE];
    chronosSellInfo_t          sellInfo[CHRONOS_REQUEST_PACKET_SIZE];
    chronosUpdateStockInfo_t   updateInfo[CHRONOS_REQUEST_PACKET_SIZE];
  } request_data;

} chronosRequestPacket_t;

#define CHRONOS_REQUEST_MAGIC                    (0xDEAF)
#define CHRONOS_REQUEST_MAGIC_CHECK(requestP)    assert((requestP)->magic == CHRONOS_REQUEST_MAGIC)
#define CHRONOS_REQUEST_MAGIC_SET(requestP)      (requestP)->magic = CHRONOS_REQUEST_MAGIC

typedef void *CHRONOS_REQUEST_H;
typedef void *CHRONOS_RESPONSE_H;

CHRONOS_REQUEST_H
chronosRequestCreate(unsigned int num_data_items,
                     chronosUserTransaction_t txnType, 
                     CHRONOS_CLIENT_CACHE_H  clientCacheH,
                     CHRONOS_ENV_H envH);

CHRONOS_REQUEST_H
chronosRequestCreateForClient(int user_idx,
                              CHRONOS_CLIENT_CACHE_H  clientCacheH,
                              CHRONOS_ENV_H envH);

int
chronosRequestFree(CHRONOS_REQUEST_H requestH);

chronosUserTransaction_t
chronosRequestTypeGet(CHRONOS_REQUEST_H requestH);

size_t
chronosRequestSizeGet(CHRONOS_REQUEST_H requestH);

CHRONOS_RESPONSE_H
chronosResponseAlloc();

int
chronosResponseFree(CHRONOS_RESPONSE_H responseH);

size_t
chronosResponseSizeGet(CHRONOS_REQUEST_H requestH);

chronosUserTransaction_t
chronosResponseTypeGet(CHRONOS_RESPONSE_H responseH);

int
chronosResponseResultGet(CHRONOS_RESPONSE_H responseH);
#endif
