#ifndef __HASSE_H__
#define __HASSE_H__

typedef struct
{
    int from;
    int to;
} t_link;

typedef struct
{
    t_link* links;
    int size;
    int capacity;
} t_link_array;

void removeTransitiveLinks(t_link_array* p_link_array);

#endif