/*
 * Merlin++: C++ Class Library for Charged Particle Accelerator Simulations
 * Copyright (c) 2001-2018 The Merlin++ developers
 * This file is covered by the terms the GNU GPL version 2, or (at your option) any later version, see the file COPYING
 * This file is derived from software bearing the copyright notice in merlin4_copyright.txt
 */

#ifndef SupportStructure_h
#define SupportStructure_h 1

#include "merlin_config.h"
#include "AcceleratorSupport.h"
#include "SequenceFrame.h"
#include "Transform3D.h"
#include "Rotation3D.h"

/**
 *	A SupportStructure represents a mechanical support
 *	system, on which accelerator components can be mounted.
 *	A SupportStructure is mounted to the ground by either
 *	one Support placed at the centre of enclosed geometry,
 *	or by two supports at the exit and entrance points.
 *
 *	SupportStructure and its associated Support class are
 *	primarily intended for application of ground motion and
 *	vibration.
 */

class SupportStructure: public SequenceFrame
{
public:

	typedef enum
	{
		simple,
		girder

	} Type;

	/**
	 *	Copy constructor.
	 */
	SupportStructure(const SupportStructure& rhs);

	/**
	 *	Destructor.
	 */
	~SupportStructure();

	/**
	 *	Appends this structures support(s) to the SupportList.
	 *	Returns the number of supports appended.
	 */
	int ExportSupports(AcceleratorSupportList& supports);

	/**
	 *	Returns the local frame transformation. The result
	 *	includes results of effects of the support offsets, as
	 *	well as local transformations of the girder.
	 *
	 *	@return Local frame transformation
	 */
	virtual Transform3D GetLocalFrameTransform() const;

	/**
	 *	When called, SupportStructure sets up is Accelerator
	 *	Structure objects. This function should only be called
	 *	after the AcceleratorModel is complete.
	 */
	virtual void ConsolidateConstruction();

protected:

	SupportStructure(const std::string& id, Type type);

private:

	AcceleratorSupport* sup1;
	AcceleratorSupport* sup2;

	/**
	 *	Updates (if necessary) the local frame transformation
	 *	due to the support offsets.
	 */
	void UpdateSupportTransform() const;

	/**
	 *	Rotation used to convert the support motion into the
	 *	local entrance plane reference frame.
	 */
	Rotation3D Rg;

	/**
	 *	Cached transformation state. Used to calculate the local
	 *	frame transformation. Is recalculated if an offset of an
	 *	accelerator support has changed.
	 */
	mutable Transform3D Ts;
};

/**
 *	Represents a long mount structure (girder) which has two
 *	supports at either end. A girder can tilt under the
 *	action of the two supports.
 */

class GirderMount: public SupportStructure
{
public:

	explicit GirderMount(const std::string& id);

	/**
	 *	Returns "SupportStructure".
	 *	@return SupportStructure
	 */
	virtual const std::string& GetType() const;

	/**
	 *	Virtual constructor.
	 */
	virtual ModelElement* Copy() const;
};

/**
 *	An accelerator mount which has a single support located
 *	at the centre of the geometry (local origin).
 */

class SimpleMount: public SupportStructure
{
public:

	explicit SimpleMount(const std::string& id);

	/**
	 *	Returns "SupportStructure".
	 *	@return SupportStructure
	 */
	virtual const std::string& GetType() const;

	/**
	 *	Virtual constructor.
	 */
	virtual ModelElement* Copy() const;
};

inline GirderMount::GirderMount(const std::string& id) :
	SupportStructure(id, girder)
{
}

inline SimpleMount::SimpleMount(const std::string& id) :
	SupportStructure(id, simple)
{
}

#endif
