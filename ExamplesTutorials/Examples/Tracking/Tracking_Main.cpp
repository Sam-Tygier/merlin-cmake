/*
 * Merlin++: C++ Class Library for Charged Particle Accelerator Simulations
 * Copyright (c) 2001-2018 The Merlin++ developers
 * This file is covered by the terms the GNU GPL version 2, or (at your option) any later version, see the file COPYING
 * This file is derived from software bearing the copyright notice in merlin4_copyright.txt
 */

#include <fstream>

#include "PhysicalUnits.h"
#include "MADInterface.h"
#include "ParticleBunch.h"
#include "ParticleTracker.h"

#define BEAMENERGY 5.0 * GeV

using namespace PhysicalUnits;
using namespace ParticleTracking;

int main()
{
	// Construct the AcceleratorModel
	// from a lattice file produced by MAD
	MADInterface madi("../lattices/MERLINFodo.lattice.txt", BEAMENERGY);

	ofstream madlog("mad.log");
	madi.SetLogFile(madlog);
	madi.SetLoggingOn();

	AcceleratorModel* theModel = madi.ConstructModel();

	// Construct a bunch of particles
	// to track through the lattice.
	// Here we just set up a bunch of 20 particles
	// at 3 mm intervals in x.
	ParticleBunch* theBunch = new ParticleBunch(BEAMENERGY);

	PSvector p(0);
	for(int xi = 1; xi <= 20; xi++)
	{
		p.x() = xi * 0.003;
		theBunch->AddParticle(p);
	}

	// Construct a ParticleTracker to perform the tracking
	ParticleTracker tracker(theModel->GetBeamline(), theBunch);

	// Do the tracking, writing the phase-space co-ordinates
	// of each particle in the bunch to a file after each turn
	ofstream trackingLog("Tracking.dat");
	for(int turn = 0; turn < 200; turn++)
	{
		if(turn == 0)
		{
			tracker.Run();
		}
		else
		{
			tracker.Continue();
		}
		ParticleBunch& tracked = tracker.GetTrackedBunch();
		tracked.Output(trackingLog);
	}

	delete theBunch;
	delete theModel;

	cout << "Finished!" << endl;

	return 0;
}
