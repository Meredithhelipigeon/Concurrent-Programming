#define AUTOMATIC_SIGNAL \
uCondition bench; \

#define WAITUNTIL( pred, before, after ) \
    if (!pred) { \
        while (!bench.empty()) bench.signal(); \
        do { \
            before; \
            bench.wait(); \
            after; \
        } while(!pred); \
    } \

#define EXIT() \
    while ( ! bench.empty() ) bench.signal(); \
