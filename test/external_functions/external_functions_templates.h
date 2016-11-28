//
//  external_functions_templates.h
//  test
//
//  Created by Timothy Prepscius on 11/27/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#pragma once

#include "external_functions_coordinator.h"

namespace vmir {

template<typename M, M m, typename T, typename... A>
class WrapMemberFunctionV
{
public:
	static void f (void *, T *t, A... args) {
		T *n = vmir_to_nativeptr(vmir_get_thread_ir_unit(), t);
		(n->*m)(args...);
	}
} ;

template<typename M, M m, typename R, typename T, typename... A>
class WrapMemberFunction
{
public:
	static void f (R *r, T *t, A... args) {
		T *n = vmir_to_nativeptr(vmir_get_thread_ir_unit(), t);
		*r = (n->*m)(args...);
	}
} ;

template<typename M, M m, typename R, typename C, typename... A>
constexpr auto generateCompileTimeWrapper(R (C::*method)(A...))
{
	return &WrapMemberFunction<M, m, R, C, A...>::f;
}

template<typename M, M m, typename C, typename... A>
constexpr auto generateCompileTimeWrapper(void (C::*method)(A...))
{
	return &WrapMemberFunctionV<M, m, C, A...>::f;
}


//-----------------------------------------

template<typename F, F fn, typename... A>
class WrapFunctionV
{
public:
	static void f (void *, A... args) {
		(*fn)(args...);
	}
} ;

template<typename F, F fn, typename R, typename... A>
class WrapFunction
{
public:
	static void f (R *r, A... args) {
		*r = (*fn)(args...);
	}
} ;

template<typename M, M m, typename R, typename... A>
constexpr auto generateCompileTimeWrapper(R (*method)(A...))
{
	return &WrapFunction<M, m, R, A...>::f;
}

template<typename M, M m, typename... A>
constexpr auto generateCompileTimeWrapper(void (*method)(A...))
{
	return &WrapFunctionV<M, m, A...>::f;
}


//------------------------------------------

#define vmir_wrap_callable(M) vmir::generateCompileTimeWrapper<decltype(&M), &M>(&M)

} // namespace
