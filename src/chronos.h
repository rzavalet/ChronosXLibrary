/*===================================
 * This is the MAIN chronos header file
 * Include this one in any file where
 * you want to call the chronos API
 *==================================*/
#ifndef _CHRONOS_H_
#define _CHRONOS_H_


/*---------------------------------
 * Important CONSTANTS 
 *-------------------------------*/
#define CHRONOS_SUCCESS         0
#define CHRONOS_FAIL            1



/*---------------------------------
 * Debugging routines
 *-------------------------------*/
#define CHRONOS_DEBUG_LEVEL_MIN   (0)
#define CHRONOS_DEBUG_LEVEL_MAX   (10)
#define CHRONOS_DEBUG_LEVEL_QUEUE (5)

extern int chronos_debug_level;

#define set_chronos_debug_level(_level)  \
  (chronos_debug_level = (_level))

#define chronos_debug(level,...) \
  do {                                                         \
    if (chronos_debug_level >= level) {                        \
      char _local_buf_[256];                                   \
      snprintf(_local_buf_, sizeof(_local_buf_), __VA_ARGS__); \
      fprintf(stderr, "DEBUG %s:%d: %s", __FILE__, __LINE__, _local_buf_);    \
      fprintf(stderr, "\n");	   \
    } \
  } while(0)



/*---------------------------------
 * Error routines
 *-------------------------------*/
#define chronos_msg(_prefix, ...) \
  do {                     \
    char _local_buf_[256];				     \
    snprintf(_local_buf_, sizeof(_local_buf_), __VA_ARGS__); \
    fprintf(stderr,"%s: %s: at %s:%d", _prefix,_local_buf_, __FILE__, __LINE__);   \
    fprintf(stderr,"\n");					     \
  } while(0)

#define chronos_info(...) \
  chronos_msg("INFO", __VA_ARGS__)

#define chronos_error(...) \
  chronos_msg("ERROR", __VA_ARGS__)

#define chronos_warning(...) \
  chronos_msg("WARN", __VA_ARGS__)

#endif
