qemu-system-i386 \
	-smp 1 \
	-m 4G \
	-drive file=os.img,format=raw \
	-trace events=events.txt,file=trace.log \
	-no-reboot \
	-no-shutdown \
	-d int,guest_errors \
	-D qemu.log \
	-monitor stdio

# putting unused arguments here because for some reason commenting them out in the command messes things up
# -d in_asm,exec \
# -dfilter 0x45d+0x28 \
# -D trace.log
