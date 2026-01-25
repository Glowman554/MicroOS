nm "$1" | awk '$2 ~ /^[TtDdBbRr]$/ { print $1 " " $3 }' | sort > "$2"
echo "ffffffff xxxxxx" >> "$2"
