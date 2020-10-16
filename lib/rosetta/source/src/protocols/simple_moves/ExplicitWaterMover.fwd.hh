// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file ExplicitWaterMover.fwd.hh
/// @author Ryan Pavlovicz (rpavlov@uw.edu)
/// @author Frank DiMaio (dimaio@uw.edu)

#ifndef INCLUDED_protocols_moves_ExplicitWaterMover_fwd_hh
#define INCLUDED_protocols_moves_ExplicitWaterMover_fwd_hh

#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace simple_moves {

class ExplicitWaterMover;
typedef utility::pointer::shared_ptr< ExplicitWaterMover > ExplicitWaterMoverOP;
typedef utility::pointer::shared_ptr< ExplicitWaterMover const > ExplicitWaterMoverCOP;

} // moves
} // protocols

#endif
