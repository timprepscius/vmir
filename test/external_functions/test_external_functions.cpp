
#include "external_functions_coordinator.h"
#include "external_pointers_coordinator.h"
#include "src/simple_print.h"

#include <map>
#include <typeinfo>
#include <string>

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


//--------------------------
// main
//
// reads the scripts into memory and then the vmir
// finds the script function and executes it
//--------------------------

void printValueTemp (Empty *e)
{
	vmir_to_nativeptr(vmir_get_thread_ir_unit(), e)->printValue();
}

std::map<const char *, function_link_t> external_functions = {
	{ "_Z12simple_printPKcchijfdxy", { (void *)simple_print, typeid(simple_print).name() } },
	{ "_ZN11ValueHolderC1Ei", { (void *)simple_print, typeid(ValueHolder::ValueHolder(int)).name() } },
	{ "_ZN11ValueHolder10printValueEv", { (void *)simple_print, typeid(&ValueHolder::printValue).name() } },
	{ "_ZN5Empty10printValueEv", { (void *)printValueTemp, typeid(&Empty::printValue).name() }  }
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
	
  Empty *empty = new Empty();
  vmir_ptr emptyPtr = vmir_map_native_ptr(iu, empty);
	
  r = vmir_vm_function_call(iu, f, (void *)&ret, emptyPtr);

  vmir_unmap_native_ptr(iu, empty);
  free(mem);

  vmir_destroy(iu);

  return r;
}