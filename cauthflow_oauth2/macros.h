#ifndef MACROS_H
#define MACROS_H

// marker for entry into each CPP area
#define ENTRYPOINT
#define INTERNAL static
#define MIN(x,y)    ((x <= y) ? x : y)

#define XSTR(s) STR(s)
#define STR(s) #s

#endif /* MACROS_H */
