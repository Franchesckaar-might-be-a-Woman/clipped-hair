#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct AddressSpace {
	uint32_t start_address;
	uint32_t base_size;
	uint8_t *bytes;
	// XXX: have a write function (increment byte_counter)
	// Also do dynamic realloc there.
	int byte_counter;
};

struct AddressSpace *address_space_create(uint32_t start_address, uint32_t base_size);
struct AddressSpace *address_space_unify(struct AddressSpace *first_space, struct AddressSpace *second_space);
