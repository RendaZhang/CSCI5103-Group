#include "Kernel.h"
#include "DirectoryEntry.h"
#include "Stat.h"
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096
#define OUTPUT_MODE 0700

int main(int argc, char ** argv) {
    const char* PROGRAM_NAME = "fsck";

    if(Kernel::initialize() == false) {
        cout << "Failed to initialized Kernel" << endl;
        Kernel::exit(1);
    }

    if(argc == 2 && !strcmp(argv[1],"break")){
        // break the file system for test purpose
        Kernel::BreakFilesystem();
        cout << "break the filesystem" << endl;
        Kernel::exit(1);
        exit(0);
    }

    if(argc != 1) {
        cout << PROGRAM_NAME << ": usage: " << PROGRAM_NAME;
        Kernel::exit(1) ;
    }

    Kernel::fsck();

    Kernel::exit(0);
}
