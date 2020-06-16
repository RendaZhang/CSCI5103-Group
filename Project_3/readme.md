# Project 3

Group Members
--------------------------------------------------------------------------------
- William Croteau, crote010@umn.edu
- Renda Zhang, zhan4067@umn.edu
<br>

Tests
--------------------------------------------------------------------------------
In the current directory with the Makefile,
run the following command (Copy and past to the terminal):

To test for link and unlink:
```
make
cd bin
./mkfs filesys.dat 256 40
./mkdir /home
./mkdir /home/jon
./ls /home
echo "howdy, podner" | ./tee /home/jon/hello.txt
./cp /home/jon/hello.txt /home/jon/greeting.txt
./ls /home/jon
./ln /home/jon/greeting.txt /home/jon/link1
./ln /home/jon/greeting.txt /home/jon/link2
./ls /home/jon
./rm /home/jon/link1
./ls /home/jon
./dump filesys.dat | more
```

To test for indirect block:
```
make
cd bin
./mkfs filesys.dat 64 40
./mkdir /home
./ls /home

echo "
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
" | ./tee /home/hello.txt

./ls /home

```


To test for fsck:
```
make
cd bin
./mkfs filesys.dat 256 40
./mkdir /home
./mkdir /home/jon
./ls /home
echo "howdy, podner" | ./tee /home/jon/hello.txt
./cp /home/jon/hello.txt /home/jon/greeting.txt
./ls /home/jon
./ln /home/jon/greeting.txt /home/jon/link1
./ln /home/jon/greeting.txt /home/jon/link2
./ls /home/jon
./rm /home/jon/link1
./ls /home/jon
./fsck

./fsck break
./fsck

```

To test for defrag:
```
make
cd bin
./mkfs filesys.dat 256 40
./mkdir /home

echo "
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
" | ./tee /home/hello.txt

echo "
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
" | ./tee /home/goodbye.txt

./rm /home/hello.txt
./defrag
```
<br>


Design Document (less than 1 page)
--------------------------------------------------------------------------------
*link:*

Link is implemented with `Kernel::link(oldpath, newpath)`. The newpath is created in the
directory to point to the same inode as `oldpath`. The number of links in the inode is
incremented each time one is added.
<br>
<br>

*unlink:*

Unlink is implemented with `Kernel::unlink(pathname)`. This `pathname` is removed from
this directory file. The number of links in the inode it was pointing to is decremented.
If the index node no longer has anything pointing to it, it will be freed.
<br>
<br>

*indirect-block:*

The indrect blocks is implemented in `IndexNode::getBlockAddress(int block)` and
`IndexNode::setBlockAddress(int block, int address)`. Helper function
`FileSystem::getMaxBlockNumber()` is also implemented.
<br>
<br>

*fsck.cc:*

It checks the file system for internal consistency, listing any problems
detected to standard output:
- All the i-nodes mentioned in directory entries have the correct nlinks.
- All blocks mentioned in the i-nodes are marked as allocated blocks.
- All blocks NOT mentioned in the i-nodes are marked as free blocks.

fsck is implemented with `Kernel::fsck()`, `Kernel::BreakFilesystem()`, and
`IndexNode::getBlocks(stack<int> &s)`, and `FileSystem::is_free(int block)`,
`FileSystem::scan(map<int, int> &allocated)`.

In `Kernel::fsck()`, it counts the inodes using DFS. only one traversal is done on
an inode which appear more than one time, and this is done using a hash table.
the number of links should be the number of directories that contain the inode
number.

In `Kernel::BreakFilesystem()`, it corrupt the file system for tests purpose.
command `fsck break` will call this function and the file system will be
corrupted.
<br>
<br>

*defrag (Extra Credit):*

De-fragments the disk by ensuring that all files contain contiguous blocks and
that there are no free blocks between files. Implemented in defrag.cc and `Kernel::defrag()`.
This was done by accounting for all of the blocks
currently in use and then reordering them by inode number. This
ensures that all of the blocks are push to the front. This has not been tested
for use with the indirect block. It does however work with the direct blocks.
An image of the filesytem usage is printed before and after the defrag occurs to
provide the user with a graphical view of the system.
