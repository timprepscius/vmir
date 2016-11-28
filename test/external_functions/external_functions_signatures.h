//
//  vmir_function_signatures.h
//  test
//
//  Created by Timothy Prepscius on 11/27/16.
//  Copyright © 2016 Timothy Prepscius. All rights reserved.
//

#pragma once

#include <string>

namespace vmir {

template<typename R, typename C, typename... A>
struct MemberFunctionPointer
{
	typedef R Return;
	typedef C Class;
};

template<typename R, typename C, typename... A>
constexpr auto inferMemberFunctionPointer(R (C::*method)(A...))
{
	return MemberFunctionPointer<R,C,A...>{};
}

template<typename R, typename... A>
struct FunctionPointer
{
	typedef R Return;
};

template<typename R, typename... A>
constexpr auto inferFunctionPointer(R (*method)(A...))
{
	return FunctionPointer<R,A...>{};
}

class GenerateMethodSignature
{
public:
	static const char *mangledName (const char *bfs, const char *rs, const char *ts, const char *ms)
	{
		const char *fs = bfs + strlen(bfs);
		while (fs > bfs && *(fs-1) != ':')
			fs--;

		std::string r = "_Z";
		if (ts[0] != 'N')
			r += "N";
		r += ts;
		if (ts[0] == 'N')
			r.pop_back();

		r += std::to_string(strlen(fs));
		r += fs;
		r += "E";

		const char *ms_ = ms + strlen ("M") + strlen(ts) + strlen ("F");
		if (*ms_ == 'S')
			ms_+=2;
		else
			ms_+=1;

		r += ms_;
		r.pop_back();

		printf("calculated signature %s\n", r.c_str());

		// this is very bad but... for demonstration purposes
		return strdup(r.c_str());
	}

	static const char *mangledName (const char *fs, const char *rs, const char *ms)
	{
		std::string r = "_Z";

		r += std::to_string(strlen(fs));
		r += fs;

		const char *ms_ = ms + strlen ("P") + strlen ("F") + strlen(rs);
		r += ms_;
		r.pop_back();

		printf("calculated signature %s\n", r.c_str());

		// this is very bad but... for demonstration purposes
		return strdup(r.c_str());
	}
} ;

template<typename R, typename T, typename... A>
constexpr const char * generateSignature(R (T::*m)(A...), const char *functionName)
{
	return GenerateMethodSignature::mangledName(functionName, typeid(R).name(), typeid(T).name(), typeid(decltype(m)).name());
}

template<typename R, typename... A>
constexpr auto generateSignature(R (*m)(A...), const char *functionName)
{
	return GenerateMethodSignature::mangledName(functionName, typeid(R).name(), typeid(decltype(m)).name());
}

} // namespace

#define vmir_signature_callable(M) vmir::generateSignature(&M, #M)


