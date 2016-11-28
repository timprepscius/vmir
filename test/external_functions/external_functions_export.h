//
//  external_functions_export.h
//  test
//
//  Created by Timothy Prepscius on 11/27/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#pragma once

#include "external_functions_coordinator.h"
#include "external_functions_templates.h"
#include "external_functions_signatures.h"

#define vmir_wrap_function(F) { (FunctionPtr)vmir_wrap_callable(F), typeid(F).name() }
#define vmir_wrap_method(TM) { (FunctionPtr)vmir_wrap_callable(TM), typeid(&TM).name() }

#define vmir_export_function(F) { vmir_signature_callable(F), vmir_wrap_function(F) }
#define vmir_export_method(F) { vmir_signature_callable(F), vmir_wrap_method(F) }
