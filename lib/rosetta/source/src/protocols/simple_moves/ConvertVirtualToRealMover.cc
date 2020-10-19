// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available
// (c) under license. The Rosetta software is developed by the contributing
// (c) members of the Rosetta Commons. For more information, see
// (c) http://www.rosettacommons.org. Questions about this can be addressed to
// (c) University of Washington UW TechTransfer,email:license@u.washington.edu.

/// @file protocols/simple_moves/ConvertVirtualToRealMover.cc
/// @brief Mover for switching virtual residues back to real residues
/// @author Sebastian Rämisch (raemisch@scripps.edu)
/// @author Jared Adolf-Bryfogle (jadolfbr@gmail.com)

// Unit headers
#include <protocols/simple_moves/ConvertVirtualToRealMover.hh>
#include <protocols/simple_moves/ConvertVirtualToRealMoverCreator.hh>

// Core headers
#include <core/pose/Pose.hh>
#include <core/chemical/ResidueType.hh>
#include <core/select/residue_selector/ResidueSelector.hh>

// Basic/Utility headers
#include <basic/Tracer.hh>
#include <basic/citation_manager/UnpublishedModuleInfo.hh>
#include <basic/citation_manager/CitationCollection.hh>
#include <utility>
#include <utility/tag/Tag.hh>

// Protocol headers
#include <protocols/rosetta_scripts/util.hh>

// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>


static basic::Tracer TR( "protocols.simple_moves.ConvertVirtualToRealMover" );

namespace protocols {
namespace simple_moves {

ConvertVirtualToRealMover::ConvertVirtualToRealMover():
	protocols::moves::Mover( ConvertVirtualToRealMover::mover_name() ),
	selector_()
{

}

ConvertVirtualToRealMover::ConvertVirtualToRealMover( core::select::residue_selector::ResidueSelectorCOP selector):
	protocols::moves::Mover( ConvertVirtualToRealMover::mover_name() ),
	selector_(std::move(selector))
{

}

ConvertVirtualToRealMover::~ConvertVirtualToRealMover()= default;

ConvertVirtualToRealMover::ConvertVirtualToRealMover( ConvertVirtualToRealMover const & src ):
	Mover( src )
{
	using namespace core::select::residue_selector;
	if ( src.selector_ ) selector_ = src.selector_->clone();
}

bool
ConvertVirtualToRealMover::mover_provides_citation_info() const {
	return true;
}

// Provide a list of authors and their e-mail addresses, as strings.
utility::vector1< basic::citation_manager::UnpublishedModuleInfoCOP >
ConvertVirtualToRealMover::provide_authorship_info_for_unpublished() const {
	using namespace basic::citation_manager;

	utility::vector1< UnpublishedModuleInfoCOP > modules;
	modules.push_back(utility::pointer::make_shared< UnpublishedModuleInfo >(
		mover_name(),
		CitedModuleType::Mover,
		"Jared Adolf-Bryfogle",
		"The Scripps Research Institute, La Jolla, CA",
		"jadolfbr@gmail.com"
		));

	modules.push_back(utility::pointer::make_shared< UnpublishedModuleInfo >(
		mover_name(),
		CitedModuleType::Mover,
		"Sebastian Rämisch",
		"The Scripps Research Institute, La Jolla, CA",
		"raemisch@scripps.edu"
		));

	if ( selector_ != nullptr ) {
		merge_into_unpublished_collection_vector( selector_->provide_authorship_info_for_unpublished(),  modules );
	}

	return modules;
}

/// @brief Although this mover has no citation info, the residue selector that it calls might have some.
utility::vector1< basic::citation_manager::CitationCollectionCOP >
ConvertVirtualToRealMover::provide_citation_info() const {
	if ( selector_ != nullptr ) {
		return selector_->provide_citation_info();
	}
	return utility::vector1< basic::citation_manager::CitationCollectionCOP >();
}

void
ConvertVirtualToRealMover::set_residue_selector( core::select::residue_selector::ResidueSelectorCOP selector){
	selector_ = selector;
}

void
ConvertVirtualToRealMover::parse_my_tag(
	utility::tag::TagCOP tag ,
	basic::datacache::DataMap &data
)
{
	if ( tag->hasOption( "residue_selector" ) ) {
		// set the selector_ private variable
		selector_ = protocols::rosetta_scripts::parse_residue_selector( tag, data );
		if ( !selector_ ) {
			throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError,  "ResidueSelector passed to VirtualToFa mover could not be found." );
		}
	}
}

void
ConvertVirtualToRealMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) {
	using namespace utility::tag;
	AttributeList attributes;
	protocols::rosetta_scripts::attributes_for_parse_residue_selector(attributes);
	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(),

		"Author: Jared Adolf-Bryfogle (jadolfbr@gmail.com) and Sebastian Raemisch (raemisch@scripps.edu)\n"
		"Mover for switching from VIRTUAL residues to REAL residues."
		" A VIRTUAL residue is one that is not scored or output.",

		attributes );
}

protocols::moves::MoverOP
ConvertVirtualToRealMover::clone() const
{
	return utility::pointer::make_shared< ConvertVirtualToRealMover >( *this );
}

protocols::moves::MoverOP
ConvertVirtualToRealMover::fresh_instance() const
{
	return utility::pointer::make_shared< ConvertVirtualToRealMover >();
}

std::string
ConvertVirtualToRealMover::get_name() const
{
	return ConvertVirtualToRealMover::mover_name();
}

std::string
ConvertVirtualToRealMover::mover_name()
{
	return "ConvertVirtualToRealMover";
}

void
ConvertVirtualToRealMover::show( std::ostream & output ) const
{
	protocols::moves::Mover::show( output );
}

std::ostream &
operator<<( std::ostream & os, ConvertVirtualToRealMover const & mover )
{
	mover.show(os);
	return os;
}

void
ConvertVirtualToRealMover::apply( core::pose::Pose & pose)
{

	/// Get a ResidueSubset (list of bools) from ResidueSelector
	core::select::residue_selector::ResidueSubset subset;
	if ( selector_ ) {
		subset = selector_->apply( pose );
	} else {
		subset.resize( pose.total_residue(), true );
		TR << "No residues selected. Switch all residues from virtual to full atom." << std::endl;
	}

	/// Go through the subset list and switch to a full atom residue type if subset[i]=1
	for ( core::Size i=1; i<=pose.total_residue(); ++i ) {
		if ( !subset[i] ) continue; //Skip residues masked by the ResidueSelector.
		pose.virtual_to_real( i );
	}

}

/////////////// Creator ///////////////

protocols::moves::MoverOP
ConvertVirtualToRealMoverCreator::create_mover() const
{
	return utility::pointer::make_shared< ConvertVirtualToRealMover >();
}


std::string
ConvertVirtualToRealMoverCreator::keyname() const
{
	return ConvertVirtualToRealMover::mover_name();
}

void
ConvertVirtualToRealMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	ConvertVirtualToRealMover::provide_xml_schema( xsd );
}
} //protocols
} //simple_moves
