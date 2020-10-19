// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/energy_methods/PointWaterEnergy.hh
/// @brief  Statistical point water energy function
/// @author Frank DiMaio


#ifndef INCLUDED_core_scoring_methods_PointWaterEnergy_hh
#define INCLUDED_core_scoring_methods_PointWaterEnergy_hh

#include <core/energy_methods/PointWaterEnergy.fwd.hh>
#include <core/scoring/PointWaterPotential.hh>

// Project headers
#include <core/chemical/AtomType.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/scoring/methods/ContextIndependentTwoBodyEnergy.hh>
#include <core/scoring/methods/EnergyMethodOptions.fwd.hh>
#include <core/scoring/trie/RotamerTrieBase.fwd.hh>
#include <core/scoring/trie/TrieCountPairBase.fwd.hh>

// Utility headers
#include <utility/vector1.hh>

// C++ headers

#include <cmath>

namespace core {
namespace scoring {
namespace methods {

///
class PointWaterEnergy : public methods::ContextIndependentTwoBodyEnergy  {
public:
	typedef methods::ContextIndependentTwoBodyEnergy  parent;
public:

	///
	PointWaterEnergy( methods::EnergyMethodOptions const &opt );

	///
	PointWaterEnergy( PointWaterEnergy const & src );

	/// clone
	methods::EnergyMethodOP
	clone() const override;

	/// stashes nblist if use_nblist is true
	void
	setup_for_minimizing(
		pose::Pose & pose,
		ScoreFunction const & sfxn,
		kinematics::MinimizerMapBase const & min_map
	) const override;

	void
	setup_for_derivatives( pose::Pose & pose, ScoreFunction const & ) const override;

	void
	setup_for_scoring( pose::Pose & pose, ScoreFunction const & ) const override;

	/////////////////////////////////////////////////////////////////////////////
	// scoring
	/////////////////////////////////////////////////////////////////////////////

	void
	residue_pair_energy(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		pose::Pose const & pose,
		ScoreFunction const &,
		EnergyMap & emap
	) const override;

	bool
	defines_score_for_residue_pair(
		conformation::Residue const & res1,
		conformation::Residue const & res2,
		bool res_moving_wrt_eachother
	) const override;

	void
	eval_residue_pair_derivatives(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		ResSingleMinimizationData const &,
		ResSingleMinimizationData const &,
		ResPairMinimizationData const &,
		pose::Pose const & pose,
		EnergyMap const & weights,
		utility::vector1< DerivVectorPair > & r1_atom_derivs,
		utility::vector1< DerivVectorPair > & r2_atom_derivs
	) const override;

	void
	eval_intrares_energy(
		conformation::Residue const &,
		pose::Pose const &,
		ScoreFunction const &,
		EnergyMap &
	) const override;

	bool
	defines_intrares_energy( EnergyMap const & /*weights*/ ) const override { return true; }

	bool
	defines_intrares_dof_derivatives( pose::Pose const & ) const override { return false; }

	bool
	minimize_in_whole_structure_context( pose::Pose const & ) const override { return false; }

	Distance
	atomic_interaction_cutoff() const override { return 6.0; }

	void indicate_required_context_graphs( utility::vector1< bool > & ) const override { }

	/// Private methods
private:

	PointWaterPotential const & potential_;
	core::Real pwater_ref_wt_, pwater_water_bonus_, pwater_water_bonus_width_;

	core::Size version() const override;

};

} // namespace elec
} // namespace scoring
} // namespace core

#endif