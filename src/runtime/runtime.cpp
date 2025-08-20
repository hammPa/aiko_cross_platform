#include "runtime.hpp"

extern "C" void runtime_input(char* buffer) {
    if(fgets(buffer, 256, stdin)) {
        // hapus newline jika ada
        buffer[strcspn(buffer, "\n")] = 0;
    }
}