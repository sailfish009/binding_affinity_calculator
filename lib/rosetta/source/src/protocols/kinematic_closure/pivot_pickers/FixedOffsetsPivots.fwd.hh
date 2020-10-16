// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

#ifndef INCLUDED_protocols_kinematic_closure_pivot_pickers_FixedOffsetsPivots_FWD_HH
#define INCLUDED_protocols_kinematic_closure_pivot_pickers_FixedOffsetsPivots_FWD_HH

#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace kinematic_closure {
namespace pivot_pickers {

class FixedOffsetsPivots;

typedef utility::pointer::shared_ptr<FixedOffsetsPivots> FixedOffsetsPivotsOP;
typedef utility::pointer::shared_ptr<FixedOffsetsPivots const> FixedOffsetsPivotsCOP;

}
}
}

#endif

