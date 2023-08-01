#pragma once

#include <renderer/psf1_font.h>
#include <config.h>

void init_text_mode_emulation(psf1_font_t font);

void text_mode_emulation_update();

#ifdef TEXT_MODE_EMULATION
#define EMU_UPDATE() text_mode_emulation_update()
#else
#define EMU_UPDATE()
#endif