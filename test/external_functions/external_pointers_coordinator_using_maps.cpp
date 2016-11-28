//
//  external_pointers_coordinator.cpp
//  test
//
//  Created by Timothy Prepscius on 11/22/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#include "external_pointers_coordinator.h"

#include <unordered_map>

using namespace vmir;

volatile uint32_t native_ptr_map_counter = 0;
std::unordered_map<native_ptr, vmir_ptr> ptr_native_to_vm_map;
std::unordered_map<vmir_ptr, native_ptr> ptr_vm_to_native_map;

vmir_ptr vmir_map_native_ptr (ir_unit_t *iu, void *ptr)
{
	auto i = ptr_native_to_vm_map.find(ptr);
	if (i != ptr_native_to_vm_map.end())
		return i->second;

	vmir_ptr id = ++native_ptr_map_counter;
	ptr_native_to_vm_map[ptr] = id;
	ptr_vm_to_native_map[id] = ptr;
	
	return id;
}

void vmir_unmap_native_ptr (ir_unit_t *iu, native_ptr ptr)
{
	auto i = ptr_native_to_vm_map.find(ptr);
	if (i != ptr_native_to_vm_map.end())
	{
		ptr_vm_to_native_map.erase(i->second);
		ptr_native_to_vm_map.erase(i);
	}
}

vmir_ptr vmir_get_vmptr(ir_unit_t *iu, native_ptr ptr)
{
	auto i = ptr_native_to_vm_map.find(ptr);
	if (i != ptr_native_to_vm_map.end())
		return i->second;
	
	return 0;
}

native_ptr vmir_get_nativeptr(ir_unit_t *iu, vmir_ptr ptr)
{
	auto i = ptr_vm_to_native_map.find(ptr);
	if (i != ptr_vm_to_native_map.end())
		return i->second;
	
	return nullptr;
}

void *vmir_get_nativeptr_from_nativeptr(ir_unit_t *iu, native_ptr ptr)
{
	return vmir_get_nativeptr(iu, vmir_host_to_vmaddr(iu, ptr));
}
