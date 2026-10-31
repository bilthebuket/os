qemu-system-i386 \
	-smp 1 \
	-m 4G \
	-drive file=os.img,format=raw \
	-trace events=events.txt,file=trace.log \
	-no-reboot \
	-no-shutdown \
	-monitor stdio

# putting unused arguments here because for some reason commenting them out in the command messes things up
#	-trace events=events.txt,file=trace.log \
#	-d in_asm,exec \
#	-dfilter 0x0..0x2000 \
#	-D qemu.log \
#	-s \
#	-S \
