make distclean
cp arch/arm/configs/stm32f769_defconfig .config
make uImage COMPILE=arm-uclinuxeabi-
cp arch/arm/boot/uImage ../uImage.bin
rm dumptext.S
arm-uclinuxeabi-objdump -d vmlinux >> dumptext.S
