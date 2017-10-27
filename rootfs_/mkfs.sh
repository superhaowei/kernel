sudo rm dev/ etc/ home/ tmp/ var/ lib/ sys/ proc/ mnt/ root/ -rf
mkdir dev
mkdir etc
mkdir home
mkdir root
mkdir tmp
mkdir var
mkdir mnt
mkdir lib
mkdir sys
mkdir proc
mkdir lib/modules
mkdir lib/modules/2.6.33-arm1
cd dev
sudo mknod -m 0666 console c 5 1
sudo mknod -m 0666 null c 1 3
sudo mknod -m 0666 tty0 c 4 1
sudo mknod -m 0666 tty1 c 4 2
sudo mknod -m 0666 tty2 c 4 3
sudo mknod -m 0666 tty3 c 4 4
sudo mknod -m 0666 ttyS0 c 4 5
sudo mknod -m 0666 ttyS1 c 4 6
sudo mknod -m 0666 ttyS2 c 4 7
sudo mknod -m 0666 ttyS3 c 4 8
sudo mknod -m 0666 mtdblock0 b 31 0
sudo mknod -m 0666 mtdblock1 b 31 1
sudo mknod -m 0666 mtdblock2 b 31 2
