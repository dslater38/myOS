#!/bin/bash

qemu-system-x86_64 \
-accel kvm \
-cpu Skylake-Client-v3 \
-m 2048 \
-boot order=d \
-S \
-gdb tcp:127.0.0.1:2000 \
-no-reboot \
-cdrom os.iso \
-drive file=hda.img,format=raw,index=0,if=ide \
-drive file=hdb.img,format=raw,index=1,if=ide \
-serial stdio \
-smp 1 \
-usb \
-vga std


# -drive format=raw,media=,file=os.iso 

# -hda hda.img 