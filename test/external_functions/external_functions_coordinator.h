//
//  external_functions_coordinator.hpp
//  test
//
//  Created by Timothy Prepscius on 11/20/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#pragma once

#include <setjmp.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <sys/queue.h>
#include <assert.h>
#include <sys/param.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

extern "C" {
#include "../../src/vmir.h"
}

typedef struct {
  void *function_ptr;
  const char *signature;
} function_link_t;

// register_function uses the name char pointer until the function is deregistered.
// it does not delete nor strdup it
void register_function (const char *name, function_link_t link);

vm_ext_function_t *restricted_function_resolver(const char *fn, void *opaque);