// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/constel/MasterFilter.hh
/// @brief Class to manage the application of filters to constellations.
/// @author Andrea Bazzoli

#ifndef INCLUDED_MasterFilter_hh
#define INCLUDED_MasterFilter_hh

#include <core/pose/Pose.fwd.hh>
#include <utility/vector1.fwd.hh>
#include <core/types.hh>

namespace protocols {
namespace constel {

class MasterFilter {

public:
	typedef bool (*FiltPtr) (core::pose::Pose const& ps, utility::vector1<core::Size> const& cnl);

private:
	/// @brief Array of filters applied to a constellation.
	static utility::vector1<FiltPtr> filters;

public:

	/// @brief Adds a filter to the 'filters' array.
	static void addfilt(FiltPtr f);

	/// @brief Tells whether a constellation is valid.
	static bool is_constel_valid(core::pose::Pose const& ps,
		utility::vector1<core::Size> const& cnl);
};

} // constel
} // protocols

#endif
