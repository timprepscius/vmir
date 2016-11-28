
#include "external_functions_coordinator.h"
#include "external_pointers_coordinator.h"
#include "external_functions_export.h"
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

void doNothing ()
{
	int x = 1;
	x = 2;
}

void mov32 (int32_t lhs_, int32_t rhs_, int32_t size, int32_t align, int8_t vol)
//void mov32 (void *lhs, void *rhs, int32_t size, int32_t align, int8_t vol)
{
	void *lhs = vmir_vmaddr_to_host(vmir_get_thread_ir_unit(), lhs_);
	void *rhs = vmir_vmaddr_to_host(vmir_get_thread_ir_unit(), rhs_);
	
	memmove(
		lhs, rhs,
		size
	);
}

std::map<const char *, function_link_t> external_functions = {
	vmir_export_function(simple_print),
	vmir_export_function(simple_print_f),
	vmir_export_function(makeIntVector8),
	vmir_export_function(makeIntVector4),
	vmir_export_function(makeIntVector1),
	vmir_export_method(ValueHolder::printValue),
	vmir_export_method(SimpleVector::printValue),
	vmir_export_method(SimpleVector::printOther),
	vmir_export_method(IntVector8::returnOne),
	vmir_export_method(IntVector8::printValue),
	vmir_export_method(IntVector4::returnOne),
	vmir_export_method(IntVector4::printValue),
	vmir_export_method(IntVector1::returnOne),
	vmir_export_method(IntVector1::printValue),
	vmir_export_method(MyNamespace::MySubNamespace::MyClass::MyFunction),
	vmir_export_function(vector_calculation),
	{ "_ZSt20__throw_length_errorPKc", vmir_wrap_function(doNothing) },
	{ "_ZSt17__throw_bad_allocv", vmir_wrap_function(doNothing) },
	{ "llvm.memmove.p0i8.p0i8.i32", vmir_wrap_function(mov32) }
};

int main(int argc,char **argv)
{
  uint8_t *file = NULL;
  long long fileSize;
	
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