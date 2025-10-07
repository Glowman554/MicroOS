all:
	mkdir -p bin
	mkdir -p lib
	
	make -C libwindow library

	make -C cfggen prog

	make -C desktop prog
	make -C vflvm prog
	make -C vterminal prog
	make -C explorer prog
	make -C imgview prog
	make -C vedit prog
	make -C taskmgr prog
	make -C vlinux prog
	make -C sysctl prog
	make -C netinfo prog

	make -C tic_tac_toe prog

clean:
	make -C libwindow clean

	make -C cfggen clean

	make -C desktop clean
	make -C vflvm clean
	make -C vterminal clean
	make -C explorer clean
	make -C imgview clean
	make -C vedit clean
	make -C taskmgr clean
	make -C vlinux clean
	make -C sysctl clean
	make -C netinfo clean

	make -C tic_tac_toe clean


compile_flags.txt:
	make -C libwindow compile_flags.txt

	make -C cfggen compile_flags.txt
	
	make -C desktop compile_flags.txt
	make -C vflvm compile_flags.txt
	make -C vterminal compile_flags.txt
	make -C explorer compile_flags.txt
	make -C imgview compile_flags.txt
	make -C vedit compile_flags.txt
	make -C taskmgr compile_flags.txt
	make -C vlinux compile_flags.txt
	make -C sysctl compile_flags.txt
	make -C netinfo compile_flags.txt

	make -C tic_tac_toe compile_flags.txt
