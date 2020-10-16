// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   src/core/energy_methods/HRF_MSLabelingEnergyCreator.hh
/// @brief  Declaration for the class that connects HRF_MSLabelingEnergy with the ScoringManager
/// @author Melanie Aprahamian

#ifndef INCLUDED_core_scoring_methods_HRF_MSLabelingEnergyCreator_hh
#define INCLUDED_core_scoring_methods_HRF_MSLabelingEnergyCreator_hh

#include <core/scoring/methods/EnergyMethodCreator.hh>

//#include <core/scoring/methods/EnergyMethodOptions.fwd.hh>
//#include <core/scoring/methods/EnergyMethod.fwd.hh>

//#include <utility/vector1.hh>


namespace core {
namespace scoring {
namespace methods {

class HRF_MSLabelingEnergyCreator : public EnergyMethodCreator
{
public:
	/// @brief Instantiate a new HRF_MSLabelingEnergy
	methods::EnergyMethodOP
	create_energy_method(
		methods::EnergyMethodOptions const &
	) const override;

	/// @brief Return the set of score types claimed by the EnergyMethod
	/// this EnergyMethodCreator creates in its create_energy_method() function
	ScoreTypes
	score_types_for_method() const override;

};

}
}
}

#endif // INCLUDED_core_scoring_ScoreFunction_HH
