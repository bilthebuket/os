qemu-system-i386 \
	-smp 1 \
	-m 4G \
	-drive file=os.img,format=raw \
	-trace events=ide_events.txt,file=ide_trace.log \
	-no-reboot \
	-no-shutdown \
	-d int,guest_errors \
	-D qemu.log \
	-monitor stdio

# putting unused arguments here because for some reason commenting them out in the command messes things up
# -trace events=pic_events.txt,file=pic_trace.log \
