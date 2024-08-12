/*
Minetest
Copyright (C) 2010-2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#pragma once

#include "exceptions.h"
#include "irrlichttypes.h"
#include <Keycodes.h>
#include <IEventReceiver.h>
#include <string>
#include <unordered_map>

class UnknownKeycode : public BaseException
{
public:
	UnknownKeycode(const std::string &s) :
		BaseException(s) {};
};

/* A key press, consisting of a scancode or a keycode */
class KeyPress
{
public:
	KeyPress() {};

	KeyPress(const std::string_view &name);

	KeyPress(const irr::SEvent::SKeyInput &in) :
		scancode(in.SystemKeyCode) {};

	std::string sym() const;
	std::string name() const;

	bool operator==(const KeyPress &o) const {
		return scancode == o.scancode;
	}

	operator bool() const {
		return scancode != 0;
	}

	static const KeyPress &getSpecialKey(const std::string &name);

private:
	u32 scancode = 0;

	static std::unordered_map<std::string, KeyPress> specialKeyCache;
};

// Global defines for convenience
// This implementation defers creation of the objects to make sure that the
// IrrlichtDevice is initialized.
#define EscapeKey KeyPress::getSpecialKey("KEY_ESCAPE")
#define LMBKey KeyPress::getSpecialKey("KEY_LBUTTON")
#define MMBKey KeyPress::getSpecialKey("KEY_MBUTTON") // Middle Mouse Button
#define RMBKey KeyPress::getSpecialKey("KEY_RBUTTON")

// Key configuration getter
const KeyPress &getKeySetting(const char *settingname);

// Clear fast lookup cache
void clearKeyCache();

irr::EKEY_CODE keyname_to_keycode(const char *name);
