//
//  external_pointers_coordinator_using_vmir.cpp
//  test
//
//  Created by Timothy Prepscius on 11/22/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#include "external_pointers_coordinator.h"
#include "external_functions_coordinator.h"

#include <unordered_map>


// -----------------
// this implementation uses the vmir heap as a means to store ptrs
// this makes the vmir -> native transition much faster.
//
// it still uses a map from native -> vmir, however this could also be done away with
// if the vmir pointers are kept track of, within the native implemenation
// ------------------

extern "C" {
#include "../../src/vmir.h"

void *vmir_heap_malloc(ir_unit_t *iu, int size);
void vmir_heap_free(ir_unit_t *iu, void *ptr);
}

template<typename T, typename U>
struct ptr_pair_hash {
    inline std::size_t operator()(const std::pair<T,U> & v) const {
		return std::hash<T>()(v.first) ^ std::hash<U>()(v.second);
    }
};

const vmir_ptr is_native_ptr_flag = 1 << 31;
const vmir_ptr remove_native_ptr_flag = ~is_native_ptr_flag;

std::unordered_map< std::pair<ir_unit_t *, native_ptr>, vmir_ptr, ptr_pair_hash<ir_unit_t *, native_ptr>> ptr_native_to_vm_map;

vmir_ptr vmir_map_native_ptr (ir_unit_t *iu, native_ptr ptr)
{
	auto i = ptr_native_to_vm_map.find({iu, ptr});
	if (i != ptr_native_to_vm_map.end())
		return i->second;

	native_ptr *pp = (void **)vmir_heap_malloc(iu, sizeof (void **));
	*pp = ptr;
	
	vmir_ptr map = vmir_host_to_vmaddr(iu, pp);
	ptr_native_to_vm_map[{iu, ptr}] = map;
	
	map |= is_native_ptr_flag;
	return map;
}

void vmir_unmap_native_ptr (ir_unit_t *iu, void *ptr)
{
	auto i = ptr_native_to_vm_map.find({iu,ptr});
	if (i != ptr_native_to_vm_map.end())
	{
		vmir_heap_free(iu, vmir_vmaddr_to_host(iu, i->second));
		ptr_native_to_vm_map.erase(i);
	}
}

vmir_ptr vmir_get_vmptr(ir_unit_t *iu, native_ptr ptr)
{
	auto i = ptr_native_to_vm_map.find({iu,ptr});
	if (i != ptr_native_to_vm_map.end())
		return i->second;
	
	return 0;
}

native_ptr vmir_get_nativeptr(ir_unit_t *iu, vmir_ptr ptr)
{
	if (ptr & is_native_ptr_flag)
	{
		void **ptr_ = (void **)vmir_vmaddr_to_host(iu, ptr);
		return *ptr_;
	}
	
	return vmir_vmaddr_to_host(iu, ptr);
}

native_ptr vmir_get_nativeptr_from_nativeptr(ir_unit_t *iu, native_ptr ptr)
{
	vmir_ptr p = vmir_host_to_vmaddr(iu, ptr);
	if (p & is_native_ptr_flag)
	{
		void **ptr_ = (void **)vmir_vmaddr_to_host(iu, p & remove_native_ptr_flag);
		return *ptr_;
	}
	
	return ptr;
}

