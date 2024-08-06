if [ -d "installer" ]; then
	rm -rvf installer
fi

if [ -f "nyan.bin" ]; then
	rm -v nyan.bin
fi