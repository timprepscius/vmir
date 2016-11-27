//
//  external_functions_coordinator.cpp
//  test
//
//  Created by Timothy Prepscius on 11/20/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#include "external_functions_coordinator.h"

#include <map>

// this needs to be changed to handle actual mangling syntax
static void skip_type (const char **c)
{
	bool once = true;
	bool longName = (**c == 'N');
	while (longName || once)
	{
		once = false;
		
		if (**c == 'E')
			longName = false;
		else
			while (isupper(**c))
				++*c;
		
		if (isnumber(**c))
		{
			while (isnumber(**c))
			{
				char number[255];
				char *n;

				// kludge for now
				n = number;
				while (isnumber(**c))
				{
					*n++ = **c;
					(*c)++;
				}
				
				*n = 0;
				
				int count = atoi(number);
				*c += count;
			}
		}
		else
		{
			++*c;
		}
	}
}

ir_unit_t *thread_ir_unit = nullptr;

ir_unit_t *vmir_get_thread_ir_unit()
{
	return thread_ir_unit;
}

int vmir_ext_ext_with_signature (void *fn, const char *signature, void *ret, const void *rf, ir_unit_t *iu)
{
	typedef void (*PFN)();
	PFN pfn = (PFN)fn;

	int64_t _rdi, _rsi, _rdx, _rcx, _r8, _r9;
	int64_t *registers[] = { &_rdi, &_rsi, &_rdx, &_rcx, &_r8, &_r9 };
	int registersAvailable = sizeof(registers)/sizeof(registers[0]);
	
	int64_t _xmm0, _xmm1, _xmm2, _xmm3, _xmm4, _xmm5, _xmm6, _xmm7;
	int64_t *floats[] = { &_xmm0, &_xmm1, &_xmm2, &_xmm3, &_xmm4, &_xmm5, &_xmm6, &_xmm7 };
	int floatsAvailable = sizeof(floats)/sizeof(floats[0]);

	int64_t registersUsed = 0;
	int64_t floatsUsed = 0;
	
	// one register is the return value
	int64_t retval;
	*registers[registersUsed++] = (int64_t)&retval;
	
	// ----------------------------------------
	
	char stack[256];
	char *s = stack;
	char *m = 0;
	char returnType = 0;
	
	const char *c;
	for (c=signature; *c!='E'; c++)
	{
		switch (*c)
		{
		case 'F':
			returnType = *++c;
			if (isnumber(returnType))
			{
				returnType = 'S';
				*registers[0] = (uint64_t)vmir_vm_ptr(&rf, iu);
				skip_type(&c);
				--c;
			}
			
		break;
		
		case 'M':
		case 'P':
		case 'R':
			if (registersUsed < registersAvailable)
			{
				m = (char *)registers[registersUsed++];
			}
			else
			{
				m = (char *)s;
				s += sizeof(void *);
			}
			
			*((void **)m) = vmir_vm_ptr(&rf, iu);
			
			++c;
			skip_type(&c);
			--c;
		break;
				
		case 'h':
		case 'c':
		case 'i':
		case 'j':
			if (registersUsed < registersAvailable)
			{
				m = (char *)registers[registersUsed++];
			}
			else
			{
				m = (char *)s;
				s += sizeof(uint32_t);
			}
		
			*((int32_t *)m) = vmir_vm_arg32(&rf);
		break;
		
		case 'l':
		case 'm':
		case 'x':
		case 'y':
			if (registersUsed < registersAvailable)
			{
				m = (char *)registers[registersUsed++];
			}
			else
			{
				m = (char *)s;
				s += sizeof(uint64_t);
			}
		
			*((int64_t *)m) = vmir_vm_arg64(&rf);
		break;

		case 'f':
			if (floatsUsed < floatsAvailable)
			{
				m = (char *)floats[floatsUsed++];
			}
			else
			{
				m = (char *)s;
				s += sizeof(float);
			}
		
			*((float *)m) = vmir_vm_arg_flt(&rf);
		break;
		
		case 'd':
			if (floatsUsed < floatsAvailable)
			{
				m = (char *)floats[floatsUsed++];
			}
			else
			{
				m = (char *)s;
				s += sizeof(double);
			}
		
			*((double *)m) = vmir_vm_arg_dbl(&rf);
		break;
		default:
			(void)0;
			// exception?
		}
	}
	
	char *pStack;
	char *i = stack;
	int32_t result_i4;
	int64_t result_i8;
	float result_r4;
	double result_r8;
	
	int stack_argument_size = s - stack;
	const int extra_stack_for_returns = 256;
    alloca(stack_argument_size + extra_stack_for_returns);
    asm {
        mov pStack, rsp
    };
	
    while (i != s)
        *pStack++ = *i++;

	// ----------------------------------------
	__asm {
		cmp registersUsed,0
		jle end_set_registers
		mov rdi, _rdi

		cmp registersUsed,1
		jle end_set_registers
		mov rsi, _rsi;

		cmp registersUsed,2
		jle end_set_registers
		mov rdx, _rdx;

		cmp registersUsed,3
		jle end_set_registers
		mov rcx, _rcx

		cmp registersUsed,4
		jle end_set_registers
		mov r8, _r8

		cmp registersUsed,5
		jle end_set_registers
		mov r9, _r9;
		
	end_set_registers:
		
		cmp floatsUsed,0
		jle end_set_floats
		movsd xmm0, _xmm0

		cmp floatsUsed,1
		jle end_set_floats
		movsd xmm1, _xmm1

		cmp floatsUsed,2
		jle end_set_floats
		movsd xmm2, _xmm2

		cmp floatsUsed,3
		jle end_set_floats
		movsd xmm3, _xmm3

		cmp floatsUsed,4
		jle end_set_floats
		movsd xmm4, _xmm4

		cmp floatsUsed,5
		jle end_set_floats
		movsd xmm5, _xmm5

		cmp floatsUsed,6
		jle end_set_floats
		movsd xmm6, _xmm6
		
		cmp floatsUsed,7
		jle end_set_floats
		movsd xmm7, _xmm7

	end_set_floats:
		
		mov rax, floatsUsed
	}

	__asm {
		call pfn
	}

	switch (returnType)
	{
		case 'f':
			vmir_vm_ret32(ret, *(int32_t *)&retval);
        break;

		case 'd':
			vmir_vm_ret64(ret, *(int64_t *)&retval);
        break;
	
		case 'c':
		case 'i':
			vmir_vm_ret32(ret, *(int32_t *)&retval);
        break;

		case 'l':
		case 'm':
		case 'x':
			vmir_vm_ret64(ret, *(int64_t *)&retval);
        break;
		
		case 'P':
			vmir_vm_ret32(ret, vmir_host_to_vmaddr(iu, (void *)retval));
		break;
		case 'S':
		case 'v':
		break;
	};
	
	
	
				/*
	switch (returnType)
	{
		case 'f':
            __asm {
                call pfn
				movss result_r4, xmm0;
            }

			vmir_vm_ret32(ret, *(int32_t *)&result_r4);
        break;

		case 'd':
            __asm {
                call pfn
				movsd result_r8, xmm0;
            }

			vmir_vm_ret64(ret, *(int64_t *)&result_r8);
        break;
	
		case 'c':
		case 'i':
            __asm {
				call pfn
                mov result_i4, eax
            }
			
			vmir_vm_ret32(ret, *(int32_t *)&result_i4);
        break;

		case 'l':
		case 'm':
		case 'x':
            __asm {
				call pfn
                mov result_i8, rax
            }
			
			vmir_vm_ret64(ret, *(int64_t *)&result_i8);
        break;
		
		case 'S':
		case 'P':
            __asm {
//				sub rsp, 256
            }
			
            __asm {
				call pfn
            }

			__asm {
                mov result_i8, rax
				sub rsp, 256
			}
			
			vmir_vm_ret32(ret, vmir_host_to_vmaddr(iu, (void *)result_i8));
        break;

		case 'v':
			__asm {
				call pfn
			}
	}
		*/

	return 0;
}

int vm_exec_ext(void *fn, void *ret, const void *rf, ir_unit_t *iu)
{
	thread_ir_unit = iu;
	
	function_link_t *fl = (function_link_t *)fn;
	if (fl->signature == 0)
	{
		vm_ext_function_t *f = (vm_ext_function_t*)fl->function_ptr;
		return f(ret, rf, iu);
	}
	
	return vmir_ext_ext_with_signature(fl->function_ptr, fl->signature, ret, rf, iu);
}

//---------------------------------------
// the function catalog
//---------------------------------------

struct cmp_str
{
   bool operator()(const char *a, const char *b) const
   {
      return std::strcmp(a, b) < 0;
   }
};


static std::map<const char *, function_link_t, cmp_str> external_functions;

void register_function (const char *name, function_link_t link)
{
	external_functions[name] = link;
}

// so there is just a little bit confusing going on in this function...
// Because I'm using a function_link_t cast to a void *, instead of a function pointer *,
// I always need to make sure that the external functions (even the libc ones) have that correct
// structure..  this is why, I test if the parent default external_function_resolver has the
// requested function, and if so, create the housing struct with a null signature
vm_ext_function_t *restricted_function_resolver(const char *fn, void *opaque)
{
	auto i = external_functions.find(fn);
	if (i!=external_functions.end())
	{
		return (vm_ext_function_t *)&i->second;
	}

	vm_ext_function_t *super = vmir_default_external_function_resolver(fn, opaque);
	if (super)
	{
		register_function(fn, { (FunctionPtr)super, 0 });
		return restricted_function_resolver(fn, opaque);
	}

	return 0; // what is current C these days 0 or NULL?
}

void register_functions(const std::map<const char *, function_link_t, cmp_str> &rhs)
{
	for (auto &i : rhs)
	{
		register_function(i.first, i.second);
	}
}
