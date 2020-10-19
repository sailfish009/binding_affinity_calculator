// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/select/residue_selector/ResidueSelector.cc
/// @brief  The ResidueSelector class identifies a subset of residues from a Pose
/// @author Andrew Leaver-Fay (leaverfa@email.unc.edu)

// Unit headers
#include <core/select/residue_selector/ResidueSelector.hh>

// Utility headers
#include <utility/tag/Tag.hh>

#ifdef    SERIALIZATION
// Utility serialization headers
#include <utility/serialization/serialization.hh>

// Cereal headers
#include <cereal/types/polymorphic.hpp>
#endif // SERIALIZATION

namespace core {
namespace select {
namespace residue_selector {

ResidueSelector::ResidueSelector() = default;

ResidueSelector::~ResidueSelector() = default;

/// @details Noop implementation in the base class in the case that a derived
/// class has no need to read data from an input tag
void ResidueSelector::parse_my_tag(
	utility::tag::TagCOP ,
	basic::datacache::DataMap &
)
{}

/// @brief Does this residue selector provide information about how to cite it?
/// @details Defaults to false.  Derived classes may override this to provide citation info.  If set to
/// true, the provide_citation_info() override should also be provided.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org)
bool
ResidueSelector::residue_selector_provides_citation_info() const {
	return false;
}

/// @brief Provide the citation.
/// @returns A vector of citation collections.  This allows the residue selector to provide citations for
/// itself and for any modules that it invokes.
/// @details The default implementation of this function provides an empty vector.  It may be
/// overriden by residue selectors wishing to provide citation information.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org)
utility::vector1< basic::citation_manager::CitationCollectionCOP >
ResidueSelector::provide_citation_info() const {
	return utility::vector1< basic::citation_manager::CitationCollectionCOP >();
}

/// @brief Does this residue selector indicate that it is unpublished (and, by extension, that the author should be
/// included in publications resulting from it)?
/// @details Defaults to false.  Derived classes may override this to provide authorship info.  If set to
/// true, the provide_authorship_info_for_unpublished() override should also be provided.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org)
bool
ResidueSelector::residue_selector_is_unpublished() const {
	return false;
}

/// @brief Provide a list of authors and their e-mail addresses, as strings.
/// @returns A list of pairs of (author, e-mail address).  Empty list if not unpublished.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org)
utility::vector1< basic::citation_manager::UnpublishedModuleInfoCOP >
ResidueSelector::provide_authorship_info_for_unpublished() const {
	return utility::vector1< basic::citation_manager::UnpublishedModuleInfoCOP >();
}

} //namespace residue_selector
} //namespace select
} //namespace core

#ifdef    SERIALIZATION

/// @brief Automatically generated serialization method
template< class Archive >
void
core::select::residue_selector::ResidueSelector::save( Archive & ) const {}

/// @brief Automatically generated deserialization method
template< class Archive >
void
core::select::residue_selector::ResidueSelector::load( Archive & ) {}

SAVE_AND_LOAD_SERIALIZABLE( core::select::residue_selector::ResidueSelector );
CEREAL_REGISTER_TYPE( core::select::residue_selector::ResidueSelector )

CEREAL_REGISTER_DYNAMIC_INIT( core_select_residue_selector_ResidueSelector )
#endif // SERIALIZATION