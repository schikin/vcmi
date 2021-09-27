/*
 * Geometries.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "Geometries.h"
#include <SDL_events.h>

Point::Point(const SDL_MouseMotionEvent &a)
	:x(a.x),y(a.y)
{}

Rect Rect::around(const Rect &r, int width) /*creates rect around another */
{
	return Rect(r.x - width, r.y - width, r.w + width * 2, r.h + width * 2);
}

Rect Rect::centerIn(const Rect &r)
{
	return Rect(r.x + (r.w - w) / 2, r.y + (r.h - h) / 2, w, h);
}
