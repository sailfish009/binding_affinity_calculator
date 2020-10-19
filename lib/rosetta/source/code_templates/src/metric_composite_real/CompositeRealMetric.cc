// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file --path--/--class--.cc
/// @brief --brief--
/// @author --name-- (--email--)

// Unit headers
#include <--path--/--class--.hh>
#include <--path--/--class--Creator.hh>

// Core headers
#include <core/simple_metrics/CompositeRealMetric.hh>
#include <core/simple_metrics/util.hh>

#include <core/select/residue_selector/ResidueSelector.hh>
#include <core/select/residue_selector/util.hh>

// Basic/Utility headers
#include <basic/Tracer.hh>
#include <basic/datacache/DataMap.hh>
#include <utility/tag/Tag.hh>
#include <utility/string_util.hh>
#include <utility/pointer/memory.hh>

// XSD Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <basic/citation_manager/UnpublishedModuleInfo.hh>
#include <basic/citation_manager/CitationCollection.hh>

#ifdef    SERIALIZATION
// Utility serialization headers
#include <utility/serialization/serialization.hh>

// Cereal headers
#include <cereal/types/polymorphic.hpp>
#endif // SERIALIZATION

static basic::Tracer TR( "--namespace_dot--.--class--" );


--namespace--

	/////////////////////
	/// Constructors  ///
	/////////////////////

/// @brief Default constructor
--class--::--class--():
	core::simple_metrics::CompositeRealMetric()
{}

////////////////////////////////////////////////////////////////////////////////
/// @brief Destructor (important for properly forward-declaring smart-pointer members)
--class--::~--class--(){}

core::simple_metrics::SimpleMetricOP
--class--::clone() const {
	return utility::pointer::make_shared< --class-- >( *this );
}

std::string
--class--::name() const {
	return name_static();
}

std::string
--class--::name_static() {
	return "--class--";

}
std::string
--class--::metric() const {

	return "SHORT_NAME_FOR_SCOREFILE_HEADER_DEFAULT";
}

utility::vector1< std::string >
--class--::get_metric_names() const {

}

void
--class--::parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap &  )
{
    SimpleMetric::parse_base_tag( tag );
	if (tag->hasOption("bogus_option")){
		return;
	}
}

void
--class--::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) {
	using namespace utility::tag;
	using namespace core::select::residue_selector;

	AttributeList attlist;
	attlist + XMLSchemaAttribute::attribute_w_default("bogus_option", xsct_rosetta_bool, "test bogus option", "false");

	//attributes_for_parse_residue_selector( attlist, "residue_selector",
	//	"Selector specifying residues." );

	core::simple_metrics::xsd_simple_metric_type_definition_w_attributes(xsd, name_static(),
		"A metric for measuring ... and adding it to the resulting score file.", attlist);
}

std::map< std::string, core::Real >
--class--::calculate(const pose::Pose & ) const {

}

bool
--class--::simple_metric_is_unpublished() const {
	return true;
}

utility::vector1< basic::citation_manager::UnpublishedModuleInfoCOP >
--class--::provide_authorship_info_for_unpublished() const {
	basic::citation_manager::UnpublishedModuleInfoOP authors (
		utility::pointer::make_shared< basic::citation_manager::UnpublishedModuleInfo >(
			name(),
			basic::citation_manager::CitedModuleType::SimpleMetric,
			"--name--",
			"TODO: institution",
			"--email--",
			"Wrote the --class--."
		)
	);

	utility::vector1< basic::citation_manager::UnpublishedModuleInfoCOP > returnvec{ authors };
	return returnvec;
}

void
--class--Creator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const {
	--class--::provide_xml_schema( xsd );
}

std::string
--class--Creator::keyname() const {
	return --class--::name_static();
}

core::simple_metrics::SimpleMetricOP
--class--Creator::create_simple_metric() const {
	return utility::pointer::make_shared< --class-- >( );
}

--end_namespace--


#ifdef    SERIALIZATION



template< class Archive >
void
--namespace_2colon--::--class--::save( Archive & arc ) const {
	arc( cereal::base_class< core::simple_metrics::CompositeRealMetric>( this ) );
	//arc( CEREAL_NVP( output_as_pdb_nums_ ) );

}

template< class Archive >
void
--namespace_2colon--::--class--::load( Archive & arc ) {
	arc( cereal::base_class< core::simple_metrics::CompositeRealMetric >( this ) );
	//arc( output_as_pdb_nums_ );


}

SAVE_AND_LOAD_SERIALIZABLE( --namespace_2colon--::--class-- );
CEREAL_REGISTER_TYPE( --namespace_2colon--::--class-- )

CEREAL_REGISTER_DYNAMIC_INIT( --path_underscore--_--class-- )
#endif // SERIALIZATION




