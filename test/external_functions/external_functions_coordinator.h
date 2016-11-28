//
//  external_functions_coordinator.hpp
//  test
//
//  Created by Timothy Prepscius on 11/20/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#pragma once

#include "safe_vmir.h"

typedef void (*Function)(void);
typedef Function *FunctionPtr;

typedef struct {
  FunctionPtr function_ptr;
  const char *signature;
} function_link_t;

// register_function uses the name char pointer until the function is deregistered.
// it does not delete nor strdup it
void register_function (const char *name, function_link_t link);

vm_ext_function_t *restricted_function_resolver(const char *fn, void *opaque);

ir_unit_t *vmir_get_thread_ir_unit();

#define vmir_wrap_function(F) { (FunctionPtr)vmir_wrap_callable(F), typeid(F).name() }
#define vmir_wrap_method(TM) { (FunctionPtr)vmir_wrap_callable(TM), typeid(&TM).name() }

#define vmir_export_function(F) { vmir_signature_callable(F), vmir_wrap_function(F) }
#define vmir_export_method(F) { vmir_signature_callable(F), vmir_wrap_method(F) }
