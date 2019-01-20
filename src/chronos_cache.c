#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <benchmark.h>
#include "chronos.h"
#include "include/chronos_cache.h"

#define CHRONOS_CLIENT_NUM_STOCKS     (300)
#define CHRONOS_CLIENT_NUM_USERS      (50)
#define MAXLINE   1024

/*
 * Information for each symbol a user is interested in.
 */
typedef struct chronosClientStockInfo_t
{
  int     symbolId;
  const char    *symbol;
  int     random_amount;
  float   random_price;
} chronosClientStockInfo_t;

/*
 * A user's portfolio information.
 * A user is interested in k stock symbols.
 */
typedef struct chronosClientPortfolios_t 
{
  int         userId;

  /* Which user is this? */
  const char *user;

  /* How many symbols the user is interested in */
  int   numSymbols;

  /* Information for each of the k stocks managed by a user */
  chronosClientStockInfo_t  stockInfoArr[100];
} chronosClientPortfolios_t;

#define CHRONOS_CLIENT_CACHE_MAGIC   (0xDEAD)
#define CHRONOS_CLIENT_CACHE_MAGIC_CHECK(cacheP)    assert((cacheP)->magic == CHRONOS_CLIENT_CACHE_MAGIC)
#define CHRONOS_CLIENT_CACHE_MAGIC_SET(cacheP)      (cacheP)->magic = CHRONOS_CLIENT_CACHE_MAGIC

typedef struct chronosClientCache_t 
{
  int                     magic;
  int                     numPortfolios;

  /*List of portfolios handled by this client thread:
   * we have one entry in the array per each managed user
   */
  chronosClientPortfolios_t portfoliosArr[100];
} chronosClientCache_t;

#define CHRONOS_CACHE_MAGIC   (0xBEEF)
#define CHRONOS_CACHE_MAGIC_CHECK(cacheP)    assert((cacheP)->magic == CHRONOS_CACHE_MAGIC)
#define CHRONOS_CACHE_MAGIC_SET(cacheP)      (cacheP)->magic = CHRONOS_CACHE_MAGIC
typedef struct chronosCache_t {
  int                 magic;
  int                 numStocks;
  char                **stocksListP;

  int                 numUsers;
  char                users[CHRONOS_CLIENT_NUM_USERS][256];
} chronosCache_t;


#define MIN(a,b)        (a < b ? a : b)
#define MAX(a,b)        (a > b ? a : b)


int
chronosCacheNumSymbolsGet(CHRONOS_CACHE_H chronosCacheH)
{
  chronosCache_t *cacheP= NULL;

  if (chronosCacheH == NULL) {
    return 0;
  }

  cacheP = (chronosCache_t *) chronosCacheH;
  CHRONOS_CACHE_MAGIC_CHECK(cacheP);

  return cacheP->numStocks;
}

const char *
chronosCacheSymbolGet(int symbolNum,
                      CHRONOS_CACHE_H chronosCacheH)
{
  chronosCache_t *cacheP= NULL;

  if (chronosCacheH == NULL) {
    chronos_error("Invalid handle");
    return NULL;
  }

  cacheP = (chronosCache_t *) chronosCacheH;
  CHRONOS_CACHE_MAGIC_CHECK(cacheP);

  if (symbolNum < 0 || symbolNum > cacheP->numStocks) {
    return NULL;
  }

  return cacheP->stocksListP[symbolNum];
}

int
chronosCacheNumUsersGet(CHRONOS_CACHE_H chronosCacheH)
{
  chronosCache_t *cacheP= NULL;

  if (chronosCacheH == NULL) {
    return 0;
  }

  cacheP = (chronosCache_t *) chronosCacheH;
  CHRONOS_CACHE_MAGIC_CHECK(cacheP);

  return cacheP->numUsers;
}

const char *
chronosCacheUserGet(int userNum,
                    CHRONOS_CACHE_H chronosCacheH)
{
  chronosCache_t *cacheP= NULL;

  if (chronosCacheH == NULL) {
    chronos_error("Invalid handle");
    return NULL;
  }

  cacheP = (chronosCache_t *) chronosCacheH;
  CHRONOS_CACHE_MAGIC_CHECK(cacheP);

  if (userNum < 0 || userNum > cacheP->numUsers) {
    return NULL;
  }

  return cacheP->users[userNum];
}

/* This client process will handle n users.
 * So, for each user, we need to create its portfolio.
 */
static int
createPortfolios(int numClient, int numClients, chronosClientCache_t *clientCacheP, CHRONOS_CACHE_H chronosCacheH)
{
  int i, j;
  int numSymbols = 0;
  int numUsers =  0;
  int numPortfolios = 0;
  int symbolsPerUser = 0;
  int random_symbol;
  int random_user;
  int random_amount;
  float random_price;

  if (chronosCacheH == NULL || clientCacheP == NULL) {
    chronos_error("Invalid cache pointer");
    goto failXit;
  }

  numSymbols = chronosCacheNumSymbolsGet(chronosCacheH);
  numUsers = chronosCacheNumUsersGet(chronosCacheH);

  numPortfolios = MAX(MIN(numUsers / numClients, 100), 10);
  symbolsPerUser = MAX(MIN(numSymbols / numUsers, 100), 10);

  fprintf(stderr, "DEBUG: numSymbols: %d, numUsers: %d, numPortfolios: %d, symbolsPerClient: %d\n",
                  numSymbols,
                  numUsers,
                  numPortfolios,
                  symbolsPerUser);

  clientCacheP->numPortfolios = numPortfolios;

  for (i=0; i<numPortfolios; i++) {
    /* TODO: does it matter which client we choose? */
    random_user = (i + (numPortfolios * (numClient -1))) % numUsers;

    clientCacheP->portfoliosArr[i].userId = random_user;
    clientCacheP->portfoliosArr[i].user = chronosCacheUserGet(random_user, chronosCacheH);
    clientCacheP->portfoliosArr[i].numSymbols = symbolsPerUser;

    for (j=0; j<symbolsPerUser; j++) {
      random_symbol = rand() % chronosCacheNumSymbolsGet(chronosCacheH);
      random_amount = rand() % 100;
      random_price = 500.0;

      clientCacheP->portfoliosArr[i].stockInfoArr[j].symbolId = random_symbol;
      clientCacheP->portfoliosArr[i].stockInfoArr[j].symbol = chronosCacheSymbolGet(random_symbol, chronosCacheH);
      clientCacheP->portfoliosArr[i].stockInfoArr[j].random_amount = random_amount;
      clientCacheP->portfoliosArr[i].stockInfoArr[j].random_price = random_price;
      fprintf(stderr, "DEBUG: Client %d Handling user: %s symbol: %s\n",
                      numClient,
                      clientCacheP->portfoliosArr[i].user,
                      clientCacheP->portfoliosArr[i].stockInfoArr[j].symbol);
    }
  }

  return CHRONOS_SUCCESS;

failXit:
  return CHRONOS_FAIL;
}

void *
chronosClientCacheAlloc(int numClient, int numClients, CHRONOS_CACHE_H chronosCacheH)
{
  chronosClientCache_t *clientCacheP = NULL;
  int rc = CHRONOS_SUCCESS;

  if (chronosCacheH == NULL) {
    chronos_error("Invalid handle");
    goto failXit;
  }

  clientCacheP = malloc(sizeof(chronosClientCache_t));
  if (clientCacheP == NULL) {
    chronos_error("Could not allocate cache structure");
    goto failXit;
  }

  memset(clientCacheP, 0, sizeof(*clientCacheP));

  rc = createPortfolios(numClient, numClients, clientCacheP, chronosCacheH);
  if (rc != CHRONOS_SUCCESS) {
    chronos_error("Failed to create porfolios cache");
    goto failXit;
  }

  CHRONOS_CLIENT_CACHE_MAGIC_SET(clientCacheP);

  goto cleanup;

failXit:
  if (clientCacheP != NULL) {
    free(clientCacheP);
    clientCacheP = NULL;
  }
  
cleanup:
  return  (CHRONOS_CLIENT_CACHE_H) clientCacheP;
}

int
chronosClientCacheFree(CHRONOS_CLIENT_CACHE_H chronosClientCacheH)
{
  int rc = CHRONOS_SUCCESS;
  chronosClientCache_t *cacheP = NULL;

  if (chronosClientCacheH == NULL) {
    chronos_error("Invalid handle");
    goto failXit;
  }

  cacheP = (chronosClientCache_t *) chronosClientCacheH;
  CHRONOS_CLIENT_CACHE_MAGIC_CHECK(cacheP);

  memset(cacheP, 0, sizeof(*cacheP));

  goto cleanup;

failXit:
  rc = CHRONOS_FAIL;

cleanup:
  return rc;
}

int
chronosClientCacheNumPortfoliosGet(CHRONOS_CLIENT_CACHE_H clientCacheH)
{
  chronosClientCache_t *clientCacheP = NULL;

  if (clientCacheH == NULL) {
    return 0;
  }

  clientCacheP = (chronosClientCache_t *) clientCacheH;
  CHRONOS_CLIENT_CACHE_MAGIC_CHECK(clientCacheP);

  return clientCacheP->numPortfolios;
}

int
chronosClientCacheUserIdGet(int numUser, CHRONOS_CLIENT_CACHE_H clientCacheH)
{
  chronosClientCache_t *clientCacheP = NULL;

  if (clientCacheH == NULL) {
    return 0;
  }

  clientCacheP = (chronosClientCache_t *) clientCacheH;
  CHRONOS_CLIENT_CACHE_MAGIC_CHECK(clientCacheP);
  assert(0 <= numUser && numUser < clientCacheP->numPortfolios);

  return clientCacheP->portfoliosArr[numUser].userId;
}

const char *
chronosClientCacheUserGet(int numUser, CHRONOS_CLIENT_CACHE_H clientCacheH)
{
  chronosClientCache_t *clientCacheP = NULL;

  if (clientCacheH == NULL) {
    return 0;
  }

  clientCacheP = (chronosClientCache_t *) clientCacheH;
  CHRONOS_CLIENT_CACHE_MAGIC_CHECK(clientCacheP);
  assert(0 <= numUser && numUser < clientCacheP->numPortfolios);

  return clientCacheP->portfoliosArr[numUser].user;
}

int
chronosClientCacheNumSymbolFromUserGet(int numUser, CHRONOS_CLIENT_CACHE_H clientCacheH)
{
  chronosClientCache_t *clientCacheP = NULL;

  if (clientCacheH == NULL) {
    return 0;
  }

  clientCacheP = (chronosClientCache_t *) clientCacheH;
  CHRONOS_CLIENT_CACHE_MAGIC_CHECK(clientCacheP);
  assert(0 <= numUser && numUser < clientCacheP->numPortfolios);

  return clientCacheP->portfoliosArr[numUser].numSymbols;
}

int
chronosClientCacheSymbolIdFromUserGet(int numUser, int numSymbol, CHRONOS_CLIENT_CACHE_H clientCacheH)
{
  chronosClientCache_t *clientCacheP = NULL;

  if (clientCacheH == NULL) {
    return 0;
  }

  clientCacheP = (chronosClientCache_t *) clientCacheH;
  CHRONOS_CLIENT_CACHE_MAGIC_CHECK(clientCacheP);
  assert(0 <= numUser && numUser < clientCacheP->numPortfolios);
  assert(0 <= numSymbol && numSymbol < clientCacheP->portfoliosArr[numUser].numSymbols);

  return clientCacheP->portfoliosArr[numUser].stockInfoArr[numSymbol].symbolId;
}

const char *
chronosClientCacheSymbolFromUserGet(int numUser, int numSymbol, CHRONOS_CLIENT_CACHE_H clientCacheH)
{
  chronosClientCache_t *clientCacheP = NULL;

  if (clientCacheH == NULL) {
    return 0;
  }

  clientCacheP = (chronosClientCache_t *) clientCacheH;
  CHRONOS_CLIENT_CACHE_MAGIC_CHECK(clientCacheP);
  assert(0 <= numUser && numUser < clientCacheP->numPortfolios);
  assert(0 <= numSymbol && numSymbol < clientCacheP->portfoliosArr[numUser].numSymbols);

  return clientCacheP->portfoliosArr[numUser].stockInfoArr[numSymbol].symbol;
}

float
chronosClientCacheSymbolPriceFromUserGet(int numUser, int numSymbol, CHRONOS_CLIENT_CACHE_H clientCacheH)
{
  chronosClientCache_t *clientCacheP = NULL;

  if (clientCacheH == NULL) {
    return 0;
  }

  clientCacheP = (chronosClientCache_t *) clientCacheH;
  CHRONOS_CLIENT_CACHE_MAGIC_CHECK(clientCacheP);
  assert(0 <= numUser && numUser < clientCacheP->numPortfolios);
  assert(0 <= numSymbol && numSymbol < clientCacheP->portfoliosArr[numUser].numSymbols);

  return clientCacheP->portfoliosArr[numUser].stockInfoArr[numSymbol].random_price;
}

static int
stockListFree(chronosCache_t *cacheP) 
{
  int rc = CHRONOS_SUCCESS;
  int i;

  if (cacheP == NULL) {
    chronos_error("Invalid argument");
    goto failXit;
  }

  if (cacheP->stocksListP != NULL) {
    for (i=0; i<CHRONOS_CLIENT_NUM_STOCKS; i++) {
      if (cacheP->stocksListP[i] != NULL) {
        free(cacheP->stocksListP[i]);
        cacheP->stocksListP[i] = NULL;
      }
    }

    free(cacheP->stocksListP);
    cacheP->stocksListP = NULL;
  }

  goto cleanup;

failXit:
  rc = CHRONOS_FAIL;

cleanup:
  return rc;
}

CHRONOS_CACHE_H
chronosCacheAlloc(const char *homedir, 
                  const char *datafilesdir)
{
  int i;
  chronosCache_t *cacheP = NULL;
  int rc = CHRONOS_SUCCESS;

  cacheP = malloc(sizeof(chronosCache_t));
  if (cacheP == NULL) {
    chronos_error("Could not allocate cache structure");
    goto failXit;
  }

  memset(cacheP, 0, sizeof(*cacheP));

  rc = benchmark_stock_list_from_file_get(homedir, datafilesdir, CHRONOS_CLIENT_NUM_STOCKS, &(cacheP->stocksListP));
  if (rc != CHRONOS_SUCCESS) {
    chronos_error("Could not initialize cache structure");
    goto failXit;
  }

  assert(cacheP->stocksListP != NULL);

  cacheP->numStocks = CHRONOS_CLIENT_NUM_STOCKS;

  cacheP->numUsers = CHRONOS_CLIENT_NUM_USERS;
  for (i=0; i<CHRONOS_CLIENT_NUM_USERS; i++) {
    snprintf(cacheP->users[i], sizeof(cacheP->users[i]),
             "%d", i + 1);
  }

  CHRONOS_CACHE_MAGIC_SET(cacheP);

  goto cleanup;

failXit:
  if (cacheP != NULL) {
    free(cacheP);
    cacheP = NULL;
  }
  
cleanup:
  return  (CHRONOS_CACHE_H) cacheP;
}

int
chronosCacheFree(CHRONOS_CACHE_H chronosCacheH)
{
  int rc = CHRONOS_SUCCESS;
  chronosCache_t *cacheP = NULL;

  if (chronosCacheH == NULL) {
    chronos_error("Invalid handle");
    goto failXit;
  }

  cacheP = (chronosCache_t *) chronosCacheH;
  CHRONOS_CACHE_MAGIC_CHECK(cacheP);

  rc = stockListFree(cacheP);
  if (rc != CHRONOS_SUCCESS) {
    chronos_error("Could not free cached items");
    goto failXit;
  }

  memset(cacheP, 0, sizeof(*cacheP));

  goto cleanup;

failXit:
  rc = CHRONOS_FAIL;

cleanup:
  return rc;
}

