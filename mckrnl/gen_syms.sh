nm $1 | grep -i " t " | awk '{ print ""$1" "$3"" }' | sort > $2
echo "ffffffff xxxxxx" >> $2