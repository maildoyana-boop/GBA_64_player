#include <stdlib.h>
void* anonymousMemoryMap(size_t size) { return calloc(1, size); }
void mappedMemoryFree(void* memory, size_t size) { (void) size; free(memory); }
