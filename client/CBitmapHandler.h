/*
 * CBitmapHandler.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

struct SDL_Surface;
class CInputStream;

namespace BitmapHandler
{
    SDL_Surface * loadBitmapFromMem(std::string fname, ui8* buffer, size_t size, bool setKey = true);
	//Load file from /DATA or /SPRITES
	SDL_Surface * loadBitmap(std::string fname, bool setKey=true);
}
