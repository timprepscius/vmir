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


void Empty::printValue()
{
	printf("this class is empty %d\n", x);
}

ValueHolder::ValueHolder (int _x)
{
	x = _x;
}

void ValueHolder::printValue()
{
	printf("%d\n", x);
}