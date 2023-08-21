function clean_dir {
	echo "Cleaning $1"
	(
		cd $1
		bash clean.sh
	)
}

if [ -d "pkgs" ]; then
	rm -rvf pkgs
fi

clean_dir brainasm