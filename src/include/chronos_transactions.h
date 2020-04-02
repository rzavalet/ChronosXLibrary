#ifndef _CHRONOS_TRANSACTIONS_H_
#define _CHRONOS_TRANSACTIONS_H_

#define   ID_SZ           10

typedef struct chronosSymbol_t {
  int  symbolIdx;
  int  symbolId;
  char symbol[ID_SZ];
} chronosSymbol_t;

typedef struct chronosViewPortfolioInfo_t {
  char accountId[ID_SZ];
} chronosViewPortfolioInfo_t;

typedef struct chronosSellInfo_t {
  char accountId[ID_SZ];

  int  symbolId;
  char symbol[ID_SZ];

  float price;

  int amount;
} chronosSellInfo_t;

typedef struct chronosPurchaseInfo_t {
  char accountId[ID_SZ];

  int  symbolId;
  char symbol[ID_SZ];

  float price;

  int amount;
} chronosPurchaseInfo_t;

typedef struct chronosUpdateStockInfo_t {
  int  symbolIdx;
  char symbol[ID_SZ];
  float price;
} chronosUpdateStockInfo_t;



/*---------------------------------
 * Important ENUMS
 *-------------------------------*/
typedef enum chronosUserTransaction_t {
  CHRONOS_USER_TXN_MIN = 0,
  CHRONOS_USER_TXN_VIEW_STOCK = CHRONOS_USER_TXN_MIN,
  CHRONOS_USER_TXN_VIEW_PORTFOLIO,
  CHRONOS_USER_TXN_PURCHASE,
  CHRONOS_USER_TXN_SALE,
  CHRONOS_USER_TXN_MAX,
  CHRONOS_SYS_TXN_UPDATE_STOCK=CHRONOS_USER_TXN_MAX,
  CHRONOS_USER_TXN_INVAL=CHRONOS_USER_TXN_MAX,
} chronosUserTransaction_t;

#if 0
typedef enum chronosSystemTransaction_t {
  CHRONOS_SYS_TXN_MIN = 0,
  CHRONOS_SYS_TXN_UPDATE_STOCK = CHRONOS_SYS_TXN_MIN,
  CHRONOS_SYS_TXN_MAX,
  CHRONOS_SYS_TXN_INVAL=CHRONOS_SYS_TXN_MAX
} chronosSystemTransaction_t;
#endif

extern const char *chronos_user_transaction_str[];
extern const char *chronos_system_transaction_str[];


#define CHRONOS_TXN_NAME(_txn_type) \
  ((CHRONOS_USER_TXN_MIN<=(_txn_type) && (_txn_type) < CHRONOS_USER_TXN_MAX) ? (chronos_user_transaction_str[(_txn_type)]) : chronos_system_transaction_str[CHRONOS_USER_TXN_MAX-(_txn_type)])

#define CHRONOS_TXN_IS_VALID(_txn_type) \
  (CHRONOS_USER_TXN_MIN<=(_txn_type) && (_txn_type) < CHRONOS_USER_TXN_MAX)

#endif
