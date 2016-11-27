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

void SimpleVector::printOther (int other)
{
	printf("%d\n", other);
}

IntVector8 IntVector8::returnOne()
{
	IntVector8 a;
	IntVector8 *b = &a;
	return a;
}

IntVector8 makeIntVector8()
{
	IntVector8 a;
	IntVector8 *b = &a;
	return a;
}

IntVector1 makeIntVector1()
{
	IntVector1 a;
	IntVector1 *b = &a;
	return a;
}

void IntVector8::printValue ()
{
	printf ("%d %d %d %d\n", v[0], v[1], v[2], v[3]);
}

IntVector4 IntVector4::returnOne()
{
	IntVector4 a;
	IntVector4 *b = &a;
	return a;
}

void IntVector1::printValue ()
{
	printf ("%04x\n", v[0]);
}

IntVector1 IntVector1::returnOne()
{
	IntVector1 a;
	IntVector1 *b = &a;
	return a;
}

IntVector4 makeIntVector4()
{
	IntVector4 a;
	IntVector4 *b = &a;
	return a;
}

void IntVector4::printValue ()
{
	printf ("%04x%04x%04x%04x\n", v[0], v[1], v[2], v[3]);
}


float vector_calculation (const SimpleVector &v)
{
	return v.x() + v.y() + v.z();
}

int MyNamespace::MySubNamespace::MyClass::MyFunction(int myarg)
{
	printf("Test namespaces %d\n", myarg);
	return 0;
}