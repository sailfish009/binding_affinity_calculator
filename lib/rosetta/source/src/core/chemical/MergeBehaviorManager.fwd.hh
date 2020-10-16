// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @brief
/// MergeBehaviorManager

#ifndef INCLUDED_core_chemical_MergeBehaviorManager_FWD_HH
#define INCLUDED_core_chemical_MergeBehaviorManager_FWD_HH

// Utility headers
#include <utility/pointer/owning_ptr.hh>

namespace core {
namespace chemical {

enum merge_residue_behavior
{
	mrb_do_not_merge,
	mrb_merge_w_prev,
	mrb_merge_w_next
};


class MergeBehaviorManager;

typedef utility::pointer::shared_ptr< MergeBehaviorManager > MergeBehaviorManagerOP;
typedef utility::pointer::shared_ptr< MergeBehaviorManager const > MergeBehaviorManagerCOP;

}
}

#endif

