// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/energy_methods/HRFDynamicsEnergy.hh
/// @brief  energy term use for scoring predicted HRFDynamicsEnergy
/// @author Sarah Biehn

#ifndef INCLUDED_core_scoring_methods_HRFDynamicsEnergy_hh
#define INCLUDED_core_scoring_methods_HRFDynamicsEnergy_hh

#include <core/energy_methods/HRFDynamicsEnergyCreator.hh>
#include <core/scoring/methods/ContextDependentOneBodyEnergy.hh>
#include <core/scoring/ScoreType.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/chemical/AtomType.hh>
#include <core/pose/Pose.fwd.hh>
#include <utility/vector1.fwd.hh>
#include <utility/vector1.hh>


namespace core {
namespace scoring {
namespace methods {


class HRFDynamicsEnergy : public ContextDependentOneBodyEnergy {
public:

	typedef methods::ContextDependentOneBodyEnergy parent;
	HRFDynamicsEnergy( methods::EnergyMethodOptions const & options );

	/// clone
	EnergyMethodOP
	clone() const override;

	/////////////////////////////////////////////////////////////////////////////
	// scoring
	/////////////////////////////////////////////////////////////////////////////

	void
	residue_energy(
		conformation::Residue const & rsd,
		pose::Pose const & pose,
		EnergyMap & emap
	) const override;

	void
	setup_for_scoring(
		pose::Pose & pose, ScoreFunction const &
	) const override;

	void
	finalize_total_energy(
		pose::Pose &,
		ScoreFunction const &,
		EnergyMap &
	) const override {}

	core::Size version() const override;

	void
	indicate_required_context_graphs(
		utility::vector1< bool > & /*context_graphs_required*/
	) const override;

private:
	std::string hrf_dynamics_input_file_;
	void init_from_file();
	utility::vector1< std::pair< core::Size, core::Real > > input_nc_;
};

}
}
}

#endif // INCLUDED_core_scoring_methods_HRFDynamicsEnergy_HH
