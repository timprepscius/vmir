
#include "external_functions_coordinator.h"
#include "external_pointers_coordinator.h"
#include "src/simple_print.h"

#include <map>
#include <typeinfo>
#include <string>

#include <functional>

// for the readFileIntoMemory
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

void readFileIntoMemory (const char *fileName, uint8_t **memory, long long *size)
{
  int fd = open(fileName, O_RDONLY);
  if(fd == -1) {
    perror("open");
    exit(1);
  }

  struct stat st;
  if(fstat(fd, &st)) {
    perror("stat");
    exit(1);
  }

  uint8_t *buf = (uint8_t *)malloc(st.st_size);
  if(read(fd, buf, st.st_size) != st.st_size) {
    perror("read");
    exit(1);
  }
  close(fd);
	
  *memory = buf;
  *size = st.st_size;
}


// I need a better way of doing this:
//
// the problem is that pointers inside the vm are 32 bits long
// vmir automatically shifts them into the actual heap when traveling out of the vm and into the host
//
// however, if I send in a pointer, it is decaptiated and the real pointer is not recoverable.
// to solve this I create a system where I store the actual pointer inside of the 64 bit variable
// and then do a "vmir_to_nativeptr"
//
// this works fine when a class is instantiated outside or inside, but not both..
// I really need a flag..  I think I could do this by setting the 31st bit.
//
// Implemented a flag..

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

//-----------------------------------

template<typename M, M m, typename... A>
class WrapMemberFunction
{
	typedef typename decltype(inferMemberFunctionPointer(m))::Class T;
	typedef typename decltype(inferMemberFunctionPointer(m))::Return R;
public:
	static void f (R *r, T *t, A... args) {
		T *n = vmir_to_nativeptr(vmir_get_thread_ir_unit(), t);
		*r = (n->*m)(args...);
	}
} ;

template<typename M, M m, typename... A>
class WrapMemberFunctionV
{
	typedef typename decltype(inferMemberFunctionPointer(m))::Class T;
	typedef typename decltype(inferMemberFunctionPointer(m))::Return R;
public:
	static void f (R *r, T *t, A... args) {
		T *n = vmir_to_nativeptr(vmir_get_thread_ir_unit(), t);
		(n->*m)(args...);
	}
} ;

//-----------------------------------------

template<typename F, F fn, typename... A>
class WrapFunction
{
	typedef typename decltype(inferFunctionPointer(fn))::Return R;
public:
	static void f (R *r, A... args) {
		*r = (*fn)(args...);
	}
} ;

template<typename F, F fn, typename... A>
class WrapFunctionV
{
	typedef typename decltype(inferFunctionPointer(fn))::Return R;
public:
	static void f (R *r, A... args) {
		(*fn)(args...);
	}
} ;

//------------------------------------------


#define WrapFunc(M) &WrapFunction<decltype(&M), &M>::f
#define WrapFuncA(M, args...) &WrapFunction<decltype(&M), &M, args>::f
#define WrapFuncV(M) &WrapFunctionV<decltype(&M), &M>::f
#define WrapFuncVA(M, args...) &WrapFunctionV<decltype(&M), &M, args>::f

#define WrapMethod(TM) &WrapMemberFunction<decltype(&TM), &TM>::f
#define WrapMethodA(TM, args...) &WrapMemberFunction<decltype(&TM), &TM, args>::f

#define WrapMethodV(TM) &WrapMemberFunctionV<decltype(&TM), &TM>::f
#define WrapMethodVA(TM, args...) &WrapMemberFunctionV<decltype(&TM), &TM, args>::f


template<typename M, M m, typename... A>
class GenerateMethodSignature
{
	typedef typename decltype(inferMemberFunctionPointer(m))::Class T;
	typedef typename decltype(inferMemberFunctionPointer(m))::Return R;
	
	
public:
	static const char *mangledName (const char *fs)
	{
		const char *ts = typeid(T).name();
		const char *rs = typeid(R).name();
		const char *ms = typeid(M).name();
		
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
} ;

#define ExportSignature(T, M) GenerateMethodSignature<decltype(&T::M), &T::M>::mangledName(#M)
const char *myMethodSignature = ExportSignature(MyNamespace::MySubNamespace::MyClass, MyFunction);


#define ExportFunction(F) { (FunctionPtr)WrapFunc(F), typeid(F).name() }
#define ExportFunctionA(F, args...) { (FunctionPtr)WrapFuncA(F, args), typeid(F).name() }
#define ExportFunctionV(F) { (FunctionPtr)WrapFuncV(F), typeid(F).name() }
#define ExportFunctionVA(F, args...) { (FunctionPtr)WrapFuncVA(F, args), typeid(F).name() }
#define ExportMethodV(TM) { (FunctionPtr)WrapMethodV(TM), typeid(&TM).name() }
#define ExportMethodVA(TM, args...) { (FunctionPtr)WrapMethodVA(TM, args), typeid(&TM).name() }
#define ExportMethod(TM) { (FunctionPtr)WrapMethod(TM), typeid(&TM).name() }
#define ExportMethodA(TM, args...) { (FunctionPtr)WrapMethodA(TM, args), typeid(&TM).name() }

std::map<const char *, function_link_t> external_functions = {
	{ "_Z12simple_printPKcchijfdxy", ExportFunctionVA(
		simple_print, char *, char, unsigned char, int, unsigned int, float, double, uint32_t, uint64_t
	) },
	{ "_Z14simple_print_ff", ExportFunctionVA(simple_print_f, float) },
	{ "_Z14makeIntVector8v", ExportFunction(makeIntVector8) },
	{ "_Z14makeIntVector4v", ExportFunction(makeIntVector4) },
	{ "_Z14makeIntVector1v", ExportFunction(makeIntVector1) },
	{ ExportSignature(ValueHolder, printValue), ExportMethodV(ValueHolder::printValue)  },
	{ ExportSignature(SimpleVector, printValue), ExportMethodV(SimpleVector::printValue) },
	{ ExportSignature(SimpleVector, printOther), ExportMethodVA(SimpleVector::printOther, int) },
	{ ExportSignature(IntVector8, returnOne), ExportMethod(IntVector8::returnOne) },
	{ ExportSignature(IntVector8, printValue), ExportMethodV(IntVector8::printValue) },
	{ ExportSignature(IntVector4, returnOne), ExportMethod(IntVector4::returnOne) },
	{ ExportSignature(IntVector4, printValue), ExportMethodV(IntVector4::printValue) },
	{ ExportSignature(IntVector1, returnOne), ExportMethod(IntVector1::returnOne) },
	{ ExportSignature(IntVector1, printValue), ExportMethodV(IntVector1::printValue) },
	{ ExportSignature(MyNamespace::MySubNamespace::MyClass, MyFunction), ExportMethodA(MyNamespace::MySubNamespace::MyClass::MyFunction, int) },
	{ "_Z18vector_calculationRK12SimpleVector", ExportFunctionA(vector_calculation, const SimpleVector &) }
//	{ "_ZN12SimpleVector10printOtherEi", ExportMethodA(SimpleVector::printOther, int) },
//	{ "_Z18vector_calculationRK12SimpleVector", ExportFunction(vector_calculation) }
};

const char *s = typeid(SimpleVector).name();
const char *ss = typeid(&SimpleVector::printValue).name();

int main(int argc,char **argv)
{
  uint8_t *file = NULL;
  long long fileSize;
	
  printf ("%s %s\n%s\n", s, ss, myMethodSignature);
	
  readFileIntoMemory("test_external_functions_script.bc", &file, &fileSize);

#define MB(x) ((x) * 1024 * 1024)
  void *mem = malloc(MB(64));

  ir_unit_t *iu = vmir_create(mem, MB(64), MB(1), MB(1), NULL);
  vmir_set_external_function_resolver(iu, restricted_function_resolver);
	
  for (auto &i: external_functions)
  {
	register_function(i.first, i.second);
  }

  if(vmir_load(iu, file, (int)fileSize)) {
    free(mem);
    vmir_destroy(iu);
    return -1;
  }
	
  ir_function_t *f;
  f = vmir_find_function(iu, "script_function");

  union {
    uint32_t u32;
    uint64_t u64;
  } ret;

  int r = vmir_vm_function_call(iu, f, (void *)&ret);

  f = vmir_find_function(iu, "script_function_with_argument");
	
  ValueHolder *v = new ValueHolder(42);
  vmir_ptr vp = vmir_map_native_ptr(iu, v);
	
  r = vmir_vm_function_call(iu, f, (void *)&ret, vp);

  vmir_unmap_native_ptr(iu, v);
  delete v;
	
	
  f = vmir_find_function(iu, "script_function_interchange");
  r = vmir_vm_function_call(iu, f, (void *)&ret, vp);
	
  free(mem);

  vmir_destroy(iu);

  return r;
}