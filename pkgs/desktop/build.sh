if [ -d "installer" ]; then
	rm -rvf installer
fi

bash ../run_installer.sh $1 $2 $3
cp -v installer/install.mex ../pkgs/desktop_install.mex