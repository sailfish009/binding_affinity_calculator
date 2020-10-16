// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   src/protocols/scoring/methods/EnsembleEnergyCreator.hh
/// @brief  Declaration for the class that connects EnsembleEnergy with the ScoringManager
/// @author Monica Berrondo

#ifndef INCLUDED_protocols_scoring_methods_EnsembleEnergyCreator_hh
#define INCLUDED_protocols_scoring_methods_EnsembleEnergyCreator_hh

#include <core/scoring/methods/EnergyMethodCreator.hh>

#include <core/types.hh>
#include <utility/vector1.hh>


namespace protocols {
namespace scoring {
namespace methods {

class EnsembleEnergyCreator : public core::scoring::methods::EnergyMethodCreator {

public:
	/// @brief Instantiate a new EnsembleEnergy
	core::scoring::methods::EnergyMethodOP create_energy_method(
		core::scoring::methods::EnergyMethodOptions const &
	) const override;

	/// @brief Return the set of score types claimed by the EnergyMethod
	/// this EnergyMethodCreator creates in its create_energy_method() function
	core::scoring::ScoreTypes
	score_types_for_method() const override;

};

}
}
}

#endif
