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

class Empty {

protected:
	int x;

public:
	Empty () { x = 0xBADF; }
	void printValue ();
} ;

class ValueHolder {

protected:
	int x;

public:
	ValueHolder (int _x);
	void printValue ();
} ;