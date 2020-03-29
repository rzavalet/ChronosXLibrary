#ifndef _CHRONOS_CACHE_H_
#define _CHRONOS_CACHE_H_

#define CHRONOS_CLIENT_MAX_PORTFOLIOS_PER_CLIENT  (100)
#define CHRONOS_CLIENT_MAX_SYMBOLS_PER_PORTFOLIO  (100)

typedef void *CHRONOS_CACHE_H;
typedef void *CHRONOS_CLIENT_CACHE_H;

CHRONOS_CACHE_H
chronosCacheAlloc(const char *homedir, 
                  const char *datafilesdir);

int
chronosCacheFree(CHRONOS_CACHE_H chronosCacheH);

int
chronosCacheNumSymbolsGet(CHRONOS_CACHE_H chronosCacheH);

const char *
chronosCacheSymbolGet(int symbolNum,
                      CHRONOS_CACHE_H chronosCacheH);

int
chronosCacheNumUsersGet(CHRONOS_CACHE_H chronosCacheH);

const char *
chronosCacheUserGet(int userNum,
                    CHRONOS_CACHE_H chronosCacheH);

CHRONOS_CLIENT_CACHE_H
chronosClientCacheAlloc(int numClient,
                        int numClients,
                        CHRONOS_CACHE_H chronosCacheH);

int
chronosClientCacheFree(CHRONOS_CLIENT_CACHE_H chronosClientCacheH);

int
chronosClientCacheNumPortfoliosGet(CHRONOS_CLIENT_CACHE_H  clientCacheH);

int
chronosClientCacheUserIdGet(int numUser,
                            CHRONOS_CLIENT_CACHE_H  clientCacheH);

const char *
chronosClientCacheUserGet(int numUser,
                          CHRONOS_CLIENT_CACHE_H  clientCacheH);

int
chronosClientCacheNumSymbolFromUserGet(int numUser,
                                       CHRONOS_CLIENT_CACHE_H  clientCacheH);

int
chronosClientCacheSymbolIdFromUserGet(int numUser,
                                      int numSymbol,
                                      CHRONOS_CLIENT_CACHE_H  clientCacheH);

const char *
chronosClientCacheSymbolFromUserGet(int numUser,
                                    int numSymbol,
                                    CHRONOS_CLIENT_CACHE_H  clientCacheH);

float
chronosClientCacheSymbolPriceFromUserGet(int numUser,
                                         int numSymbol,
                                         CHRONOS_CLIENT_CACHE_H  clientCacheH);

#endif
