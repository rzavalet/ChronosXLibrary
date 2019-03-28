#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "chronos.h"
#include "include/chronos_transactions.h"
#include "include/chronos_packets.h"
#include "include/chronos_environment.h"
#include "include/chronos_cache.h"

const char *chronos_user_transaction_str[] = {
  "CHRONOS_USER_TXN_VIEW_STOCK",
  "CHRONOS_USER_TXN_VIEW_PORTFOLIO",
  "CHRONOS_USER_TXN_PURCHASE",
  "CHRONOS_USER_TXN_SALE"
};

const char *chronos_system_transaction_str[] = {
  "CHRONOS_SYS_TXN_UPDATE_STOCK"
};

static int
chronosPackUpdateStock(const char                 *symbol, 
                       float                       price,
                       chronosUpdateStockInfo_t   *updateStockInfoP)
{
  int rc = CHRONOS_SUCCESS;

  if (updateStockInfoP == NULL) {
    chronos_error("Invalid argument");
    goto failXit;
  }

  if (symbol == NULL) {
    chronos_error("NULL symbol provided");
    goto failXit;
  }

  strncpy(updateStockInfoP->symbol, symbol, sizeof(updateStockInfoP->symbol));
  updateStockInfoP->price = price;

  chronos_debug(CHRONOS_DEBUG_LEVEL_MAX, 
                "Packed: [%s, %.2f]", 
                updateStockInfoP->symbol, updateStockInfoP->price);

  goto cleanup;

failXit:
  rc = CHRONOS_FAIL;

cleanup:
  return rc;
}

static int
chronosPackPurchase(const char *accountId,
                    int          symbolId, 
                    const char *symbol, 
                    float        price,
                    int          amount,
                    chronosPurchaseInfo_t *purchaseInfoP)
{
  int rc = CHRONOS_SUCCESS;

  if (purchaseInfoP == NULL) {
    chronos_error("Invalid argument");
    goto failXit;
  }

  strncpy(purchaseInfoP->accountId, 
          accountId,
          sizeof(purchaseInfoP->accountId));

  purchaseInfoP->symbolId = symbolId;
  strncpy(purchaseInfoP->symbol, 
          symbol,
          sizeof(purchaseInfoP->symbol));

  purchaseInfoP->price = price;
  purchaseInfoP->amount = amount;

  chronos_debug(CHRONOS_DEBUG_LEVEL_MAX,
               "Packed: [%s, %d, %s, %.2f, %d]",
               purchaseInfoP->accountId, purchaseInfoP->symbolId, purchaseInfoP->symbol,
               purchaseInfoP->price, purchaseInfoP->amount);
  goto cleanup;

failXit:
  rc = CHRONOS_FAIL;

cleanup:
  return rc;
}

static int
chronosPackSellStock(const char *accountId,
                     int          symbolId, 
                     const char *symbol, 
                     float        price,
                     int          amount,
                     chronosSellInfo_t *sellInfoP)
{
  int rc = CHRONOS_SUCCESS;

  if (sellInfoP == NULL) {
    chronos_error("Invalid argument");
    goto failXit;
  }

  strncpy(sellInfoP->accountId, 
          accountId,
          sizeof(sellInfoP->accountId));

  sellInfoP->symbolId = symbolId;
  strncpy(sellInfoP->symbol, 
          symbol,
          sizeof(sellInfoP->symbol));

  sellInfoP->price = price;
  sellInfoP->amount = amount;

  goto cleanup;

failXit:
  rc = CHRONOS_FAIL;

cleanup:
  return rc;
}

static int
chronosPackViewPortfolio(const char *accountId,
                         chronosViewPortfolioInfo_t *portfolioInfoP)
{
  int rc = CHRONOS_SUCCESS;

  if (portfolioInfoP == NULL) {
    chronos_error("Invalid argument");
    goto failXit;
  }

  strncpy(portfolioInfoP->accountId, 
          accountId,
          sizeof(portfolioInfoP->accountId));

  goto cleanup;

failXit:
  rc = CHRONOS_FAIL;

cleanup:
  return rc;
}

static int
chronosPackViewStock(int symbolId, 
                     const char *symbol, 
                     chronosSymbol_t *symbolInfoP)
{
  int rc = CHRONOS_SUCCESS;

  if (symbolInfoP == NULL) {
    chronos_error("Invalid argument");
    goto failXit;
  }

  symbolInfoP->symbolId = symbolId;
  strncpy(symbolInfoP->symbol, 
          symbol,
          sizeof(symbolInfoP->symbol));

  goto cleanup;

failXit:
  rc = CHRONOS_FAIL;

cleanup:
  return rc;
}

CHRONOS_REQUEST_H
chronosRequestCreateForClient(int user_idx,
                              CHRONOS_CLIENT_CACHE_H  clientCacheH,
                              CHRONOS_ENV_H envH)
{
  int i;
  int rc = CHRONOS_SUCCESS;
  int num_data_items = 0;
  int random_user_idx = 0;
  int random_symbol_idx = 0;
  int random_symbol;
  int random_amount;
  float random_price;
  const char *symbol;
  const char *user;
  chronosRequestPacket_t *reqPacketP = NULL;
  CHRONOS_CACHE_H chronosCacheH = NULL;

  if (envH == NULL || clientCacheH == NULL) {
    chronos_error("Invalid argument");
    goto failXit;
  }

  chronosCacheH = chronosEnvCacheGet(envH);
  if (chronosCacheH == NULL) {
    chronos_error("Invalid cache handle");
    goto failXit;
  }

  reqPacketP = malloc(sizeof(chronosRequestPacket_t));
  if (reqPacketP == NULL) {
    chronos_error("Could not allocate request structure");
    goto failXit;
  }

  memset(reqPacketP, 0, sizeof(*reqPacketP));
  CHRONOS_REQUEST_MAGIC_SET(reqPacketP);

  // Get user details
  user = chronosClientCacheUserGet(user_idx, clientCacheH);
  num_data_items = chronosClientCacheNumSymbolFromUserGet(user_idx, clientCacheH);

  reqPacketP->txn_type = CHRONOS_USER_TXN_PURCHASE;
  reqPacketP->numItems = (num_data_items > CHRONOS_MAX_DATA_ITEMS_PER_XACT ? CHRONOS_MAX_DATA_ITEMS_PER_XACT : num_data_items);

  for (i=0; i<num_data_items; i++) {

    // Now get the symbol
    random_symbol = chronosClientCacheSymbolIdFromUserGet(user_idx, i, clientCacheH);
    symbol = chronosClientCacheSymbolFromUserGet(user_idx, i, clientCacheH);

    random_amount = 100;
    // Allow a high price
    //random_price = chronosClientCacheSymbolPriceFromUserGet(random_user_idx, random_symbol_idx, clientCacheH) + 10;
    random_price = 2000;

    rc = chronosPackPurchase(user,
                             random_symbol, symbol,
                             random_price, random_amount,
                             &(reqPacketP->request_data.purchaseInfo[i]));
    if (rc != CHRONOS_SUCCESS) {
      chronos_error("Could not pack purchase request");
      goto failXit;
    }
  }

  goto cleanup;

failXit:
  if (reqPacketP != NULL) {
    free(reqPacketP);
    reqPacketP = NULL;
  }

cleanup:
  return (void *) reqPacketP;
}

CHRONOS_REQUEST_H
chronosRequestCreate(unsigned int num_data_items,
                     chronosUserTransaction_t txnType, 
                     CHRONOS_CLIENT_CACHE_H  clientCacheH,
                     CHRONOS_ENV_H envH)
{
  int i;
  int random_num_data_items = CHRONOS_MIN_DATA_ITEMS_PER_XACT + rand() % (1 + CHRONOS_MAX_DATA_ITEMS_PER_XACT - CHRONOS_MIN_DATA_ITEMS_PER_XACT);
  int rc = CHRONOS_SUCCESS;
  int random_user_idx = 0;
  int random_symbol_idx = 0;
  int random_symbol;
  int random_amount;
  float random_price;
  const char *symbol;
  const char *user;
  chronosRequestPacket_t *reqPacketP = NULL;
  CHRONOS_CACHE_H chronosCacheH = NULL;

  if (envH == NULL || clientCacheH == NULL) {
    chronos_error("Invalid argument");
    goto failXit;
  }

  chronosCacheH = chronosEnvCacheGet(envH);
  if (chronosCacheH == NULL) {
    chronos_error("Invalid cache handle");
    goto failXit;
  }

  if (num_data_items > 0) {
    random_num_data_items = num_data_items;
  }

  if (random_num_data_items > CHRONOS_MAX_DATA_ITEMS_PER_XACT) {
    random_num_data_items = CHRONOS_MAX_DATA_ITEMS_PER_XACT;
  }

  reqPacketP = malloc(sizeof(chronosRequestPacket_t));
  if (reqPacketP == NULL) {
    chronos_error("Could not allocate request structure");
    goto failXit;
  }

  memset(reqPacketP, 0, sizeof(*reqPacketP));
  CHRONOS_REQUEST_MAGIC_SET(reqPacketP);
  reqPacketP->txn_type = txnType;
  reqPacketP->numItems = random_num_data_items;

  switch (txnType) {
    case CHRONOS_USER_TXN_VIEW_STOCK:
      random_user_idx = rand() % chronosClientCacheNumPortfoliosGet(clientCacheH);
      for (i=0; i<random_num_data_items; i++) {
        // Choose a random symbol for this user
        random_symbol_idx = rand() % chronosClientCacheNumSymbolFromUserGet(random_user_idx, clientCacheH);

        // Now get the symbol
        random_symbol = chronosClientCacheSymbolIdFromUserGet(random_user_idx, random_symbol_idx, clientCacheH);
        symbol = chronosClientCacheSymbolFromUserGet(random_user_idx, random_symbol_idx, clientCacheH);
        rc = chronosPackViewStock(random_symbol, 
                                   symbol,
                                   &(reqPacketP->request_data.symbolInfo[i]));
        if (rc != CHRONOS_SUCCESS) {
          chronos_error("Could not pack view stock request");
          goto failXit;
        }
      }

      break;

    case CHRONOS_USER_TXN_VIEW_PORTFOLIO:
      for (i=0; i<random_num_data_items; i++) {
        random_user_idx = rand() % chronosClientCacheNumPortfoliosGet(clientCacheH);
        user = chronosClientCacheUserGet(random_user_idx, clientCacheH);
        rc = chronosPackViewPortfolio(user,
                                     &(reqPacketP->request_data.portfolioInfo[i]));
        if (rc != CHRONOS_SUCCESS) {
          chronos_error("Could not pack view portfolio request");
          goto failXit;
        }
      }
      break;

    case CHRONOS_USER_TXN_PURCHASE:
      for (i=0; i<random_num_data_items; i++) {
        // Choose a random user
        random_user_idx = rand() % chronosClientCacheNumPortfoliosGet(clientCacheH);

        // Get user details
        user = chronosClientCacheUserGet(random_user_idx, clientCacheH);

        // Choose a random symbol for this user
        random_symbol_idx = rand() % chronosClientCacheNumSymbolFromUserGet(random_user_idx, clientCacheH);

        // Now get the symbol
        random_symbol = chronosClientCacheSymbolIdFromUserGet(random_user_idx, random_symbol_idx, clientCacheH);
        symbol = chronosClientCacheSymbolFromUserGet(random_user_idx, random_symbol_idx, clientCacheH);

        random_amount = 10;
        // Allow a high price
        //random_price = chronosClientCacheSymbolPriceFromUserGet(random_user_idx, random_symbol_idx, clientCacheH) + 10;
        random_price = 2000;

        rc = chronosPackPurchase(user,
                                 random_symbol, symbol,
                                 random_price, random_amount,
                                 &(reqPacketP->request_data.purchaseInfo[i]));
        if (rc != CHRONOS_SUCCESS) {
          chronos_error("Could not pack purchase request");
          goto failXit;
        }
      }
      break;

    case CHRONOS_USER_TXN_SALE:
      for (i=0; i<random_num_data_items; i++) {
        // Choose a random user
        random_user_idx = rand() % chronosClientCacheNumPortfoliosGet(clientCacheH);

        // Get user details
        user = chronosClientCacheUserGet(random_user_idx, clientCacheH);

        // Choose a random symbol for this user
        random_symbol_idx = rand() % chronosClientCacheNumSymbolFromUserGet(random_user_idx, clientCacheH);

        // Now get the symbol
        random_symbol = chronosClientCacheSymbolIdFromUserGet(random_user_idx, random_symbol_idx, clientCacheH);
        symbol = chronosClientCacheSymbolFromUserGet(random_user_idx, random_symbol_idx, clientCacheH);

        random_amount = 5;
        // Allow a low price
        //random_price = chronosClientCacheSymbolPriceFromUserGet(random_user_idx, random_symbol_idx, clientCacheH) - 10;
        random_price = 0;

        rc = chronosPackSellStock(user,
                                  random_symbol, symbol,
                                  random_price, random_amount,
                                  &(reqPacketP->request_data.sellInfo[i]));
        if (rc != CHRONOS_SUCCESS) {
          chronos_error("Could not pack sell request");
          goto failXit;
        }
      }
      break;

    case CHRONOS_SYS_TXN_UPDATE_STOCK:
      for (i=0; i<random_num_data_items; i++) {
        // Choose a random symbol
        random_symbol = rand() % chronosCacheNumSymbolsGet(chronosCacheH);

        // Now get the symbol
        symbol = chronosCacheSymbolGet(random_symbol, chronosCacheH);
        random_price = 1000;

        rc = chronosPackUpdateStock(symbol, random_price, 
                                    &(reqPacketP->request_data.updateInfo[i]));

        if (rc != CHRONOS_SUCCESS) {
          chronos_error("Could not pack update request");
          goto failXit;
        }
      }
      break;

    default:
      assert("Invalid transaction type" == 0);
  }

  goto cleanup;

failXit:
  if (reqPacketP != NULL) {
    free(reqPacketP);
    reqPacketP = NULL;
  }

cleanup:
  return (void *) reqPacketP;
}

int
chronosRequestFree(CHRONOS_REQUEST_H requestH)
{
  chronosRequestPacket_t *requestP = NULL;

  if (requestH == NULL) {
    chronos_error("Invalid handle");
    goto failXit;
  }

  requestP = (chronosRequestPacket_t *) requestH;
  memset(requestP, 0, sizeof(*requestP));
  free(requestP);

  return CHRONOS_SUCCESS;

failXit:
  return CHRONOS_FAIL; 
}

chronosUserTransaction_t
chronosRequestTypeGet(CHRONOS_REQUEST_H requestH)
{
  chronosRequestPacket_t *requestP = NULL;

  if (requestH == NULL) {
    chronos_error("Invalid handle");
    goto failXit;
  }

  requestP = (chronosRequestPacket_t *) requestH;
  return requestP->txn_type;

failXit:
  return CHRONOS_USER_TXN_INVAL;
}

size_t
chronosRequestSizeGet(CHRONOS_REQUEST_H requestH)
{
  chronosRequestPacket_t *requestP = NULL;

  if (requestH == NULL) {
    chronos_error("Invalid handle");
    goto failXit;
  }

  requestP = (chronosRequestPacket_t *) requestH;
  return sizeof(*requestP);

failXit:
  return -1;
}

CHRONOS_RESPONSE_H
chronosResponseAlloc()
{
  chronosResponsePacket_t *resPacketP = NULL;

  resPacketP = malloc(sizeof(chronosResponsePacket_t));
  if (resPacketP == NULL) {
    chronos_error("Could not allocate response structure");
    goto failXit;
  }

  memset(resPacketP, 0, sizeof(*resPacketP));
  goto cleanup;

failXit:
  if (resPacketP != NULL) {
    free(resPacketP);
    resPacketP = NULL;
  }

cleanup:
  return (void *) resPacketP;
}

int
chronosResponseFree(CHRONOS_RESPONSE_H responseH)
{
  chronosResponsePacket_t *responseP = NULL;

  if (responseH == NULL) {
    chronos_error("Invalid handle");
    goto failXit;
  }

  responseP = (chronosResponsePacket_t *) responseH;
  memset(responseP, 0, sizeof(*responseP));
  free(responseP);

  return CHRONOS_SUCCESS;

failXit:
  return CHRONOS_FAIL; 
}

size_t
chronosResponseSizeGet(CHRONOS_RESPONSE_H responseH)
{
  chronosResponsePacket_t *responseP = NULL;

  if (responseH == NULL) {
    chronos_error("Invalid handle");
    goto failXit;
  }

  responseP = (chronosResponsePacket_t *) responseH;
  return sizeof(*responseP);

failXit:
  return -1;
}

chronosUserTransaction_t
chronosResponseTypeGet(CHRONOS_RESPONSE_H responseH)
{
  chronosResponsePacket_t *responseP = NULL;

  if (responseH == NULL) {
    chronos_error("Invalid handle");
    goto failXit;
  }

  responseP = (chronosResponsePacket_t *) responseH;
  return responseP->txn_type;

failXit:
  return CHRONOS_USER_TXN_INVAL;
}

int
chronosResponseResultGet(CHRONOS_RESPONSE_H responseH)
{
  chronosResponsePacket_t *responseP = NULL;

  if (responseH == NULL) {
    chronos_error("Invalid handle");
    goto failXit;
  }

  responseP = (chronosResponsePacket_t *) responseH;
  return responseP->rc;

failXit:
  return -1;
}

