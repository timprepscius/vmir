
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
//
// Now what I need is a neat way of wrapping methods with arguments and sending those arguments
// into a function, I bet boost has this done already

void printValueTemp (ValueHolder *v)
{
	vmir_to_nativeptr(vmir_get_thread_ir_unit(), v)->printValue();
}

void printSimpleVectorTemp (SimpleVector *v)
{
	vmir_to_nativeptr(vmir_get_thread_ir_unit(), v)->printValue();
	
	// if not using a flag for vmir ptrs, this must be used
	// v->printValue();
}


std::map<const char *, function_link_t> external_functions = {
	{ "_Z12simple_printPKcchijfdxy", { (void *)simple_print, typeid(simple_print).name() } },
	{ "_Z14simple_print_ff", { (void *)simple_print_f, typeid(simple_print_f).name() } },
	{ "_ZN11ValueHolder10printValueEv", { (void *)printValueTemp, typeid(&ValueHolder::printValue).name() }  },
	{ "_ZN12SimpleVector10printValueEv", { (void *)printSimpleVectorTemp, typeid(&SimpleVector::printValue).name() }  },
	{ "_Z18vector_calculationRK12SimpleVector", { (void *)vector_calculation, typeid(vector_calculation).name() } }
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