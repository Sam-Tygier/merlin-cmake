/*
 * Merlin++: C++ Class Library for Charged Particle Accelerator Simulations
 * Copyright (c) 2001-2018 The Merlin++ developers
 * This file is covered by the terms the GNU GPL version 2, or (at your option) any later version, see the file COPYING
 * This file is derived from software bearing the copyright notice in merlin4_copyright.txt
 */

#ifndef RWChannelState_h
#define RWChannelState_h 1

#include "merlin_config.h"
#include <vector>
#include "Channels.h"

class RWChannelState
{
public:

	RWChannelState(const std::vector<RWChannel*>& channels);
	void Reset();
	void CacheCurrentState();

private:

	RWChannelArray chs;
	RealVector values;
};

#endif
