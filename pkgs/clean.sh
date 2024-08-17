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
clean_dir phoenix_tools
clean_dir phoenixv2_tools
clean_dir nyanmbr
clean_dir doom
clean_dir saf
