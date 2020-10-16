// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/constel/NeighTeller.hh
/// @brief A class to determine neighboring relationships between or among residues.
/// @author Andrea Bazzoli

#ifndef Included_NeighTeller_HH
#define Included_NeighTeller_HH

#include <core/pose/Pose.hh>
#include <core/conformation/Residue.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/methods/EnergyMethodOptions.hh>
#include <core/scoring/EnergyMap.hh>
#include <utility/vector1.fwd.hh>
#include <iosfwd>

namespace protocols {
namespace constel {

class NeighTeller {

	core::scoring::ScoreFunctionOP scorefxn;
	core::scoring::methods::EnergyMethodOptions eopts;
	core::scoring::EnergyMap unweighted_emap;
	core::Real fa_atr_weight;
	core::Real const interaction_score_threshold;

	/// @brief: boolean table telling whether two residues in a set are neighbors,
	///  for sets of at most MAXNGB residues.
	static const int MAXNGB = 10;
	utility::vector1< utility::vector1<bool> > nmap;
	void print_nmap(core::Size const nres, std::ostream& os) const;

public:
	NeighTeller(core::pose::Pose& ref_pose);

	/// @brief: tells whether a probe residue is a neighbor of a target residue.
	bool isneigh(core::conformation::Residue const & tgt, core::conformation::Residue const & prb, core::pose::Pose const& ref_pose);

	/// @brief: tells whether a set of residues form a tree of neighbors.
	bool is_neigh_tree(utility::vector1<core::Size> const& set, core::pose::Pose const& ps);
};

/// @brief Creates the list of residues that are neighbors of a given residue.
void mk_neigh_list(core::Size const tgtnum, utility::vector1<bool>& neighs, core::pose::Pose& ps);

} // constel
} // protocols

#endif
