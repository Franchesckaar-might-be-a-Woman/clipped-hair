#include "address_space.h"

struct AddressSpace *address_space_create(uint32_t start_address, uint32_t base_size) {
	uint8_t *address_space_bytes = (uint8_t *) malloc(base_size * sizeof(uint8_t));
	memset(address_space_bytes, 0, base_size);

	struct AddressSpace *address_space = (struct AddressSpace *) malloc(sizeof(struct AddressSpace));
	address_space->start_address = start_address;
	address_space->base_size = base_size;
	address_space->bytes = address_space_bytes;
	address_space->byte_counter = 0;

	return address_space;
}

struct AddressSpace *address_space_unify(struct AddressSpace *first_space, struct AddressSpace *second_space) {
	struct AddressSpace *address_space;
	uint32_t first_start, first_end, second_start, second_end;
	uint32_t gap_size, unified_size;

	if(first_space->start_address > second_space->start_address) {
		struct AddressSpace *tmp_space;

		tmp_space = first_space;
		first_space = second_space;
		second_space = tmp_space;
	}

	// By hypothesis, no overlap:
	// +---------+-----+---------+
	// | Block 1 |     | Block 2 |
	// +---------+-----+---------+
	first_start = first_space->start_address;
	first_end = first_start + (uint32_t) first_space->byte_counter;
	second_start = second_space->start_address;
	second_end = second_start + (uint32_t) second_space->byte_counter;

	gap_size = second_start - first_end;
	unified_size = second_end - first_start;

	address_space = address_space_create(first_start, unified_size);
	address_space->byte_counter = unified_size;

	memcpy(address_space->bytes, first_space->bytes, first_space->byte_counter);
	memset(address_space->bytes + first_space->byte_counter, 0, gap_size);
	memcpy(address_space->bytes + first_space->byte_counter + gap_size, second_space->bytes, second_space->byte_counter);

	free((void *) first_space);
	free((void *) second_space);

	return address_space;
}

