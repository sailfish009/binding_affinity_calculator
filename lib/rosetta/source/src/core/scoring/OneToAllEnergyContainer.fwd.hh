// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/OneToAllEnergyContainer.hh
/// @brief  A container interface for storing and scoring long range energies
/// @author Frank DiMaio

#ifndef INCLUDED_core_scoring_OneToAllEnergyContainer_fwd_hh
#define INCLUDED_core_scoring_OneToAllEnergyContainer_fwd_hh

// Unit headers

// Package headers

// Utility headers
#include <utility/pointer/owning_ptr.hh>


namespace core {
namespace scoring {

class OneToAllNeighborIterator;
typedef utility::pointer::shared_ptr< OneToAllNeighborIterator > OneToAllNeighborIteratorOP;


class OneToAllEnergyContainer;
typedef utility::pointer::shared_ptr< OneToAllEnergyContainer > OneToAllEnergyContainerOP;

} // namespace scoring
} // namespace core

#endif
