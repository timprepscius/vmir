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
	simple_print("hello", 'x', 254, 42, 84, 3.14158, 1.41123123123123124124124888886666, 1234, 4321);
}

void script_function_with_argument (ValueHolder *v)
{
	v->printValue();
}

void script_function_interchange ()
{
	SimpleVector v(1, 2, 3);
	v.printValue();
	v.printOther(42);
	
	float x = vector_calculation (v);
	simple_print_f(x);
	
	MyNamespace::MySubNamespace::MyClass c;
	c.MyFunction(42);
}

}