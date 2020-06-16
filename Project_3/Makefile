BIN = bin
MKFS = bin/mkfs
LS = bin/ls
CAT = bin/cat
TEE = bin/tee
CP = bin/cp
DUMP = bin/dump
MKDIR = bin/mkdir
LN = bin/ln
RM = bin/rm
FSCK = bin/fsck
DEFRAG = bin/defrag
all: $(BIN) $(MKFS) $(LS) $(CAT) $(TEE) $(CP) $(DUMP) $(MKDIR) $(LN) $(RM) $(FSCK) $(DEFRAG)

bin:
	mkdir $(BIN)

$(FSCK): fsck.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o
	g++ fsck.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o -o $(FSCK)


$(LN): ln.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o
	g++ ln.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o -o $(LN)

$(RM): rm.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o
	g++ rm.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o -o $(RM)

$(CP): cp.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o
	g++ cp.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o -o $(CP)

$(TEE): tee.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o
	g++ tee.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o -o $(TEE)

$(LS): ls.cc Stat.o DirectoryEntry.o Kernel.o FileSystem.o FileDescriptor.o ProcessContext.o
	g++ ls.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o -o $(LS)

$(MKDIR): mkdir.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o
	g++ mkdir.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o -o $(MKDIR)

$(CAT): cat.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o
	g++ cat.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o -o $(CAT)

$(MKFS): mkfs.cc BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o Kernel.o FileSystem.o FileDescriptor.o ProcessContext.o Stat.o
	g++ mkfs.cc BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o Kernel.o FileSystem.o FileDescriptor.o ProcessContext.o Stat.o -o $(MKFS)
  
$(DEFRAG): defrag.cc BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o Kernel.o FileSystem.o FileDescriptor.o ProcessContext.o Stat.o
	g++ defrag.cc BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o Kernel.o FileSystem.o FileDescriptor.o ProcessContext.o Stat.o -o $(DEFRAG)

$(DUMP): dump.cc
	g++ dump.cc -o $(DUMP)

Kernel.o: Kernel.cc
	g++ -c Kernel.cc

ProcessContext.o: ProcessContext.cc
	g++ -c ProcessContext.cc

FileDescriptor.o: FileDescriptor.cc
	g++ -c FileDescriptor.cc

FileSystem.o: FileSystem.cc
	g++ -c FileSystem.cc

Stat.o: Stat.cc
	g++ -c Stat.cc

DirectoryEntry.o: DirectoryEntry.cc
	g++ -c DirectoryEntry.cc

BitBlock.o: BitBlock.cc
	g++ -c BitBlock.cc

IndexNode.o: IndexNode.cc
	g++ -c IndexNode.cc

Block.o: Block.cc
	g++ -c Block.cc

SuperBlock.o: SuperBlock.cc
	g++ -c SuperBlock.cc

clean:
	rm *.o
	rm -rf bin/*
	rmdir bin
