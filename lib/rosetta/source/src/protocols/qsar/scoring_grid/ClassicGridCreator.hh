// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   /git/src/protocols/qsar/scoring_grid/ClassicGridCreator.hh
/// @author Sam DeLuca

#ifndef INCLUDED_protocols_qsar_scoring_grid_CLASSICGRIDCREATOR_HH
#define INCLUDED_protocols_qsar_scoring_grid_CLASSICGRIDCREATOR_HH

#include <protocols/qsar/scoring_grid/GridCreator.hh>
#include <utility/tag/Tag.fwd.hh>

namespace protocols {
namespace qsar {
namespace scoring_grid {

class ClassicGridCreator : public GridCreator
{
public:
	GridBaseOP create_grid(utility::tag::TagCOP tag) const override;
	GridBaseOP create_grid() const override;

	std::string keyname() const override;
	void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const override;
	//static std::string grid_name();
};

}
}
}


#endif /* CLASSICGRIDCREATOR_HH_ */
