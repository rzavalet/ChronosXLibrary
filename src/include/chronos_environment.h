#ifndef _CHRONOS_ENVIRONMENT_H_
#define _CHRONOS_ENVIRONMENT_H_

#include "chronos_cache.h"

typedef void *CHRONOS_ENV_H;

CHRONOS_ENV_H
chronosEnvAlloc(const char *homedir, 
                const char *datafilesdir);

int
chronosEnvFree(CHRONOS_ENV_H envH);

int
chronosEnvCheck(CHRONOS_ENV_H envH);

CHRONOS_CACHE_H
chronosEnvCacheGet(CHRONOS_ENV_H envH);
#endif

