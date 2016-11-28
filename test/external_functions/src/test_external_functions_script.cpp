//
//  test_external_functions_script.c
//  vmir
//
//  Created by Timothy Prepscius on 4/9/16.
//  Copyright (c) 2016 Timothy Prepscius. All rights reserved.
//

#include "simple_print.h"
#include <vector>
#include <algorithm>

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
	
	IntVector4 iv4 = makeIntVector4();
	iv4.printValue();

	IntVector8 iv8 = makeIntVector8();
	iv8.printValue();
	
	IntVector1 iv1 = makeIntVector1();
	iv1.printValue();
	
	float x = vector_calculation (v);
	simple_print_f(x);
	
	MyNamespace::MySubNamespace::MyClass c;
	c.MyFunction(42);
	
	std::vector<int> iv;
	iv.push_back(84);
	iv.push_back(87);
	iv.push_back(86);
	iv.push_back(85);
	
	std::sort(iv.begin(), iv.end());
	for (auto &i : iv)
		v.printOther(i);
}

}