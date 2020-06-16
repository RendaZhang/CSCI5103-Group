#include "Kernel.h"
#include "DirectoryEntry.h"
#include "Stat.h"
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096
#define OUTPUT_MODE 0700

int main(int argc, char ** argv) {
    const char* PROGRAM_NAME = "defrag";

    if(Kernel::initialize() == false) {
        cout << "Failed to initialized Kernel" << endl;
        Kernel::exit(1);
    }

    if(argc != 1){
        cout << PROGRAM_NAME << ": usage: " << PROGRAM_NAME;
        Kernel::exit(1) ;
    }

    Kernel::defrag();

    Kernel::exit(0);
}
