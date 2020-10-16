// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file utility/tag/AutoBool.hh
/// @brief enum class that can be used for booleans with an automatic "let Rosetta decide" option
/// @author Jack Maguire

#ifndef INCLUDED_utility_tag_AutoBool_hh
#define INCLUDED_utility_tag_AutoBool_hh

//#include <utility/pointer/owning_ptr.hh>
//#include <utility/pointer/access_ptr.hh>

namespace utility {
namespace tag {

enum class AutoBool {
	False = 0,
	True,
	Auto
};

}
}
#endif
