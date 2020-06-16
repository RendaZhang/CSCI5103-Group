# Project 4

Group Members
--------------------------------------------------

- William Croteau, crote010@umn.edu
- Renda Zhang, zhan4067@umn.edu
<br>


Description
-------------------------------------------------
The file that were added/changed:
```diff
+ Makefile, scullBuffer.c, scullBuffer.h, consumer.c, producer.c, install.sh, remove.sh, test.sh, readme.md
```
**Please include them and all other files in this directory for the final submission.** 
<br>
test.sh run a bunch of consumers and producers in the background process.
<br>

How to run our program
--------------------------------------------------

First, in the current directory inside VM, do the following
```
chmod +x install.sh
chmod +x test.sh
chmod +x remove.sh
```
<br>

To install the scullBuffer device:
(sudo may ask you for your root password.)
```
./install.sh [max buffer items]
```
<br>

To test the basic functionality:
```
echo "Hello world" | tee /dev/scullBuffer0
cat /dev/scullBuffer0
```
<br>

To remove the scullBuffer device:
```
./remove.sh
```
<br>

To test the producer and consumer, run Bash script:
```
./install.sh 5
./test.sh
```
<br>

Our Virtual Machine info
--------------------------------------------------
To access the virtual machine, run the following in the cse lab machine.
```
ssh student@csel-x07-umh.cselabs.umn.edu
```
Enter the password:
```
OaL#v7rT
```
