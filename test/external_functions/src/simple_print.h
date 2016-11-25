//
//  simple_print.hpp
//  test
//
//  Created by Timothy Prepscius on 11/20/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#pragma once

#include <stdint.h>

void simple_print (const char *message, char c, unsigned char uc, int i, unsigned int ui, float f, double d, int64_t l, uint64_t ul);
void simple_print_f (float f);

class ValueHolder {

protected:
	int x;

public:
	ValueHolder (int _x)
	{
		x = _x;
	}
	
	void printValue ();
} ;

class SimpleVector {
protected:
	float v[3];
	
public:
	SimpleVector (float x, float y, float z)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
	}
	
	float &x () { return v[0]; }
	float &y () { return v[1]; }
	float &z () { return v[2]; }

	const float &x () const { return v[0]; }
	const float &y () const { return v[1]; }
	const float &z () const { return v[2]; }
	
	void printValue ();
	void printOther (int other);
} ;

float vector_calculation (const SimpleVector &v);

	namespace MyNamespace {
	namespace MySubNamespace {
	class MyClass
	{
	public:
		int MyFunction (int myarg);
	} ;
	} // namespace
	} // namespace

