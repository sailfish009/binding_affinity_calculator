// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/helical_bundle/PerturbBundleHelix.fwd.hh
/// @brief  Defines owning pointers for PerturbBundleHelix mover class.
/// @author Vikram K. Mulligan (vmullig@uw.edu)

#ifndef INCLUDED_protocols_helical_bundle_PerturbBundleHelix_fwd_hh
#define INCLUDED_protocols_helical_bundle_PerturbBundleHelix_fwd_hh

#include <utility/pointer/owning_ptr.hh>
#include <utility/vector1.hh>

namespace protocols {
namespace helical_bundle {

class PerturbBundleHelix; // fwd declaration
typedef utility::pointer::shared_ptr< PerturbBundleHelix > PerturbBundleHelixOP;
typedef utility::pointer::shared_ptr< PerturbBundleHelix const > PerturbBundleHelixCOP;
typedef utility::vector1<PerturbBundleHelixOP> PerturbBundleHelixOPs;
typedef utility::vector1<PerturbBundleHelixCOP> PerturbBundleHelixCOPs;

} // namespace helical_bundle
} // namespace protocols

#endif // INCLUDED_protocols_helical_bundle_PerturbBundleHelix_fwd_hh
