// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/helical_bundle_predict/HBP_TemperatureScheduleGenerator.fwd.hh
/// @brief A base class to generate a temperature ramping scheme for a simulated annealing trajectory.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org)

#ifndef INCLUDED_protocols_helical_bundle_predict_HBP_TemperatureScheduleGenerator_fwd_hh
#define INCLUDED_protocols_helical_bundle_predict_HBP_TemperatureScheduleGenerator_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>


// Forward
namespace protocols {
namespace helical_bundle_predict {

class HBP_TemperatureScheduleGenerator;

typedef utility::pointer::shared_ptr< HBP_TemperatureScheduleGenerator > HBP_TemperatureScheduleGeneratorOP;
typedef utility::pointer::shared_ptr< HBP_TemperatureScheduleGenerator const > HBP_TemperatureScheduleGeneratorCOP;

} //protocols
} //helical_bundle_predict

#endif //INCLUDED_protocols_helical_bundle_predict_HBP_TemperatureScheduleGenerator_fwd_hh