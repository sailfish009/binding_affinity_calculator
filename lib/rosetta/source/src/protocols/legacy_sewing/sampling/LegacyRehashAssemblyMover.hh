// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file LegacyRehashAssemblyMover.hh
///
/// @brief A Mover that uses more legacy_sewing to close gaps from discontinous assemblies
/// @author Tim Jacobs

#ifdef NOT_IN_SCONS_DEPRECATED

#ifndef INCLUDED_protocols_legacy_sewing_sampling_LegacyRehashAssemblyMover_HH
#define INCLUDED_protocols_legacy_sewing_sampling_LegacyRehashAssemblyMover_HH

//Unit headers
#include <protocols/legacy_sewing/sampling/LegacyRehashAssemblyMover.fwd.hh>

//Package headers
#include <protocols/legacy_sewing/sampling/LegacyAssemblyMover.hh>
#include <protocols/legacy_sewing/conformation/Assembly.hh>

//Protocol headers
#include <core/pose/Pose.hh>

#include <protocols/loophash/BackboneDB.hh>
#include <protocols/loophash/LoopHashLibrary.fwd.hh>

#include <protocols/loops/Loops.hh>


namespace protocols {
namespace legacy_sewing  {

class LegacyRehashAssemblyMover : public LegacyAssemblyMover {

public:

	LegacyRehashAssemblyMover();

	protocols::moves::MoverOP
	clone() const;

	protocols::moves::MoverOP
	fresh_instance() const;

	std::string
	get_name() const;

	virtual
	bool
	generate_assembly(
		AssemblyOP & assembly
	);

	//bool
	//rearrange_assembly(
	//	AssemblyOP & assembly
	//) const;

	void
	parse_my_tag(
		TagCOP tag,
		basic::datacache::DataMap & data
	) override;

private:

	core::Real max_loop_distance_;
	std::map< int, Model > bridge_models_;

};

} //legacy_sewing
} //protocols

#endif

#endif
