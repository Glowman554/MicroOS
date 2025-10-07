#pragma once

void init_program_launcher();
void draw_desktop_program_launcher();

void launcher_tick();

void launch_program_for_file_extension(const char* file);