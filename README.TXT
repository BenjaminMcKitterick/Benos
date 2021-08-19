
The following lines of code have been copied into .bashrc in order to set environmental variables to compile and build the benos.iso image using the makefile.

export TARGET=i686-elf
export PATH=$HOME/benos/opt/cross/bin:$PATH
export PATH=/usr/bin:$PATH

once these variables have been saved to .bashrc reboot. 

The following command can be used to emulate benos using qemu. Xorriso needs to be installed for support. If you receive an mformat invocation error install 'mtools' with 'sudo apt-get install mtools'.

qemu-system-i386 -cdrom benos.iso
