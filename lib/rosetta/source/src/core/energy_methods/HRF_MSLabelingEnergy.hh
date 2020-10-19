// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/energy_methods/HRF_MSLabelingEnergy.hh /// @brief  energy term use for scoring predicted HRF_MSLabeling
/// @author Melanie Aprahamian

// define all those functions in cc file!!

#ifndef INCLUDED_core_scoring_methods_MSLabelingEnergy_hh
#define INCLUDED_core_scoring_methods_MSLabelingEnergy_hh

#include <core/energy_methods/HRF_MSLabelingEnergyCreator.hh>
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


class HRF_MSLabelingEnergy : public ContextDependentOneBodyEnergy {
public:

	HRF_MSLabelingEnergy();

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
	void init_from_file();
	utility::vector1< std::pair< core::Size, core::Real > > prot_factor_;
	core::Real dist_midpoint_;
	core::Real dist_exponent_;
	core::Real slope_;
	core::Real intercept_;
	core::Real fade_outer_;
	core::Real fade_dist_;
};

}
}
}

#endif // INCLUDED_core_scoring_methods_MSLabelingEnergy_HH