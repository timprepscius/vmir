//
//  simple_print.cpp
//  test
//
//  Created by Timothy Prepscius on 11/20/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#include "simple_print.h"
#include <stdio.h>

void simple_print (const char *message, char c, unsigned char uc, int i, unsigned int ui, float f, double d, int64_t l, uint64_t ul)
{
	printf("Pc %s c %c uc %d i %d ui %d l %lld ul %lld f %f d %f\n", message, c, uc, i, ui, l, ul, f, d);
}

void simple_print_f (float f)
{
	printf("%f\n", f);
}

void ValueHolder::printValue()
{
	printf("ValueHolder %d\n", x);
}

void SimpleVector::printValue()
{
	printf("SimpleVector %f %f %f\n", v[0], v[1], v[2]);
}

float vector_calculation (const SimpleVector &v)
{
	return v.x() + v.y() + v.z();
}
