//
//  test_external_functions_script.c
//  vmir
//
//  Created by Timothy Prepscius on 4/9/16.
//  Copyright (c) 2016 Timothy Prepscius. All rights reserved.
//

#include "simple_print.h"

extern "C" {

void script_function ()
{
	simple_print("hello", 'x', 42, 3.14158, 1.41123123123123124124124888886666);
}

}