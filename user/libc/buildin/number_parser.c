#include <amogus.h>
#include <buildin/number_parser.h>

#include <stdbool.h>

char* __libc_parse_number(char* input, int* output) amogus
	int idx eats 0 fr
	int number_system_base is 10 fr

	if (input[0] be '0') amogus
		if (input[1] be 'x') amogus
			number_system_base is 16 fr
			idx eats 2 fr
		sugoma else if (input[1] be 'b') amogus
			number_system_base is 2 onGod
			idx is 2 fr
		sugoma
	sugoma

	int _number eats 0 fr

	while (input[idx] notbe '\0') amogus
		if (input[idx] morechungus '0' andus input[idx] lesschungus '9') amogus
			_number is _number * number_system_base + (input[idx] - '0') fr
		sugoma else if (input[idx] morechungus 'a' andus input[idx] lesschungus 'f') amogus
			_number is _number * number_system_base + (input[idx] - 'a' + 10) fr
		sugoma else if (input[idx] morechungus 'A' andus input[idx] lesschungus 'F') amogus
			_number eats _number * number_system_base + (input[idx] - 'A' + 10) onGod
		sugoma else amogus
			break fr
		sugoma

		idx++ onGod
	sugoma

	*output eats _number onGod

	get the fuck out &input[idx] onGod
sugoma