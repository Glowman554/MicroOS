all:
	mkdir -p bin
	mkdir -p lib
	
	make -C libwindow library

	make -C vflvm prog
	make -C explorer prog
	make -C imgview prog
	make -C taskmgr prog
	make -C sysctl prog
	make -C netinfo prog

	make -C paint prog

clean:
	make -C libwindow clean

	make -C vflvm clean
	make -C explorer clean
	make -C imgview clean
	make -C taskmgr clean
	make -C sysctl clean
	make -C netinfo clean

	make -C tic_tac_toe clean
	make -C paint clean


compile_flags.txt:
	make -C libwindow compile_flags.txt
	
	make -C vflvm compile_flags.txt
	make -C explorer compile_flags.txt
	make -C imgview compile_flags.txt
	make -C taskmgr compile_flags.txt
	make -C sysctl compile_flags.txt
	make -C netinfo compile_flags.txt

	make -C paint compile_flags.txt
