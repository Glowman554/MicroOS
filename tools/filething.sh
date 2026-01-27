prepare_upload() {
	local file_name="$1"

	if [[ -z "$file_name" ]]; then
		echo "Usage: prepare_upload <file_name>" >&2
		return 1
	fi

	if [[ -z "$UPLOAD_SERVER" || -z "$UPLOAD_AUTH_TOKEN" ]]; then
		echo "UPLOAD_SERVER and UPLOAD_AUTH_TOKEN must be set" >&2
		return 1
	fi

	local response
	response=$(curl -sS -w "\n%{http_code}" \
		-X POST "$UPLOAD_SERVER/api/v1/prepare" \
		-H "Content-Type: application/json" \
		-H "Authentication: $UPLOAD_AUTH_TOKEN" \
		-d "{\"name\":\"$file_name\"}"
	)

	local body
	local status
	body=$(echo "$response" | sed '$d')
	status=$(echo "$response" | tail -n1)

	if [[ "$status" -lt 200 || "$status" -ge 300 ]]; then
		echo "Failed to prepare upload (HTTP $status)" >&2
		echo "$body" >&2
		return 1
	fi

	echo "$body"
}

upload() {
	local file="$1"
	local url="$2"
	local token="$3"

	if [[ -z "$file" || -z "$url" || -z "$token" ]]; then
		echo "Usage: upload <file> <url> <token>" >&2
		return 1
	fi

	if [[ ! -f "$file" ]]; then
		echo "File not found: $file" >&2
		return 1
	fi

	local status
	status=$(curl -sS -o /dev/null -w "%{http_code}" \
		-X POST "$url" \
		-H "Authentication: $token" \
		--data-binary @"$file"
	)

	if [[ "$status" -lt 200 || "$status" -ge 300 ]]; then
		echo "Failed to upload file (HTTP $status)" >&2
		return 1
	fi
}


upload_file() {
	local file="$1"

	if [[ -z "$file" ]]; then
		echo "Usage: upload_file <file>" >&2
		return 1
	fi

	local response
	response=$(prepare_upload "$(basename "$file")") || return 1

	local upload_url
	local upload_token

	upload_url=$(echo "$response" | jq -r '.url')
	upload_token=$(echo "$response" | jq -r '.uploadToken')

	if [[ -z "$upload_url" || -z "$upload_token" || "$upload_url" == "null" ]]; then
		echo "Invalid prepare_upload response" >&2
		return 1
	fi

	upload "$file" "$upload_url" "$upload_token"

	echo "$upload_url"
}
