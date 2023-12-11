#ifndef GLOBAL_H
#define GLOBAL_H

// Make with DEBUG=1 to compile with DEBUG defined
// $ make DEBUG=1
#ifdef DEBUG
    #define DEBUG_PRINT(msg, ...)   \
    {                               \
        printf(msg, __VA_ARGS__);   \
    }
#else
    #define DEBUG_PRINT(msg, ...) do{}while(0)
#endif

#endif /* GLOBAL_H */