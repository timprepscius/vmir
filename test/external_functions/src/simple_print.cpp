//
//  simple_print.cpp
//  test
//
//  Created by Timothy Prepscius on 11/20/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#include "simple_print.h"
#include <stdio.h>

void simple_print (const char *message, char c, int i, float f, double d)
{
	printf("%s %c %d %f %f\n", message, c, i, f, d);
}
