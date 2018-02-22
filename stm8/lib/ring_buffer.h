#ifndef RINGBUFFER_H
#define RINGBUFFER_H

/** simple implementation of ring buffers */

#define DEFINE_RING(NAME, SIZE) uint8_t NAME##_ring[SIZE]; \
                                uint8_t NAME##_start = 0; \
                                uint8_t NAME##_end = 0

#define RING_PUT(NAME, VALUE) 	{NAME##_end++; \
                                NAME##_end %= sizeof(NAME##_ring); \
                                NAME##_ring[NAME##_end] = VALUE;}

#define RING_GET(NAME, DEST)    {NAME##_start++; \
                                NAME##_start %= sizeof(NAME##_ring); \
                                DEST = NAME##_ring[NAME##_start];}

#define RING_NOTEMPTY(NAME) (NAME##_start != NAME##_end)

#endif
