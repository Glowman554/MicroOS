#include <stdio.h>
#include <wav.h>

int main(int argc, char* argv[], char* envp[]) {
	if (argc != 2) {
		printf("Usage: %s <audio file>\n", argv[0]);
		printf("Supported formats: .wav\n");
		return 1;
	}

	char* path = argv[1];

	if (!play_wav_file(path)) {
		printf("Failed to play WAV file: %s\n", path);
	}

	return 1;
}
