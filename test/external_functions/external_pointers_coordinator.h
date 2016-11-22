//
//  external_pointers_coordinator.hpp
//  test
//
//  Created by Timothy Prepscius on 11/22/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#pragma once

#include "safe_vmir.h"

typedef uint32_t vmir_ptr;
typedef void *native_ptr;

vmir_ptr vmir_map_native_ptr (ir_unit_t *iu, native_ptr ptr);
void vmir_unmap_native_ptr (ir_unit_t *iu, native_ptr ptr);

vmir_ptr vmir_get_vmptr(ir_unit_t *iu, native_ptr ptr);
native_ptr vmir_get_nativeptr(ir_unit_t *iu, vmir_ptr ptr);
native_ptr vmir_get_nativeptr_from_nativeptr(ir_unit_t *iu, native_ptr ptr);

template<typename T>
T *vmir_to_nativeptr(ir_unit_t *iu, T *ptr)
{
	return (T *)vmir_get_nativeptr_from_nativeptr(iu, ptr);
}
