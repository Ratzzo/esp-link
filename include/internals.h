//probably defined somewhere on the closed source stuff, well shit.
#pragma pack(push, 1)
typedef struct romheader_t {
	uint8_t magic;
	uint8_t n_segments;
	uint8_t flash_interface;
	uint8_t flash_freq : 4;
	uint8_t flash_size : 4;
	uint32_t entry_point;
	uint32_t map_address; //what memory this will be copied to, 0 otherwise
	uint32_t size;
} romheader_t;

typedef struct segmentheader_t {
	uint32_t offset;
	uint32_t size;
} segmentheader_t;

#pragma pack(pop)
