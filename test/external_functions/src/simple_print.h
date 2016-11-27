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

class IntVector8 {
	int v[8];

public:
	IntVector8 () {
		v[0] = 0xBADF;
		v[1] = 0x00D5;
		v[2] = 0xBADF;
		v[3] = 0x00D1;
		v[4] = 0xBADF;
		v[5] = 0x00D5;
		v[6] = 0xBADF;
		v[7] = 0x00D1;
	}
	
	IntVector8 returnOne ();
	void printValue ();
};

class IntVector4 {
	int v[4];

public:
	IntVector4 () {
		v[0] = 0xBADF;
		v[1] = 0x00D5;
		v[2] = 0xBADF;
		v[3] = 0x00D1;
	}
	
	IntVector4 returnOne ();
	void printValue ();
};

class IntVector1 {
	int v[4];

public:
	IntVector1 () {
		v[0] = 0xBADF;
	}
	
	IntVector1 returnOne ();
	void printValue ();
};

IntVector8 makeIntVector8();
IntVector4 makeIntVector4();
IntVector1 makeIntVector1();

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

