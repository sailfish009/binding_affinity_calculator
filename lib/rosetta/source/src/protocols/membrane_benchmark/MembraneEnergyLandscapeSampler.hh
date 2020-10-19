// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/membrane_benchmark/MembraneEnergyLandscapeSampler.hh
/// @brief Sample all points on a 2D membrane energy landscape given implicit model and protein dimensions
/// @author Rebecca Alford (rfalford12@gmail.com)

#ifndef INCLUDED_protocols_membrane_benchmark_MembraneEnergyLandscapeSampler_HH
#define INCLUDED_protocols_membrane_benchmark_MembraneEnergyLandscapeSampler_HH

// Unit headers
#include <protocols/membrane_benchmark/MembraneEnergyLandscapeSampler.fwd.hh>
#include <protocols/moves/Mover.hh>

// Protocol headers
#include <protocols/membrane/TranslationRotationMover.hh>
#include <protocols/membrane/util.hh>

#include <protocols/filters/Filter.fwd.hh>

// Core headers
#include <core/pose/Pose.fwd.hh>
#include <core/scoring/ScoreFunction.fwd.hh>

// Basic/Utility headers
#include <basic/datacache/DataMap.fwd.hh>
//#include <utility/tag/XMLSchemaGeneration.fwd.hh> //transcluded from Mover

namespace protocols {
namespace membrane_benchmark {

///@brief Sample all points on a 2D membrane energy landscape given implicit model and protein dimensions
class MembraneEnergyLandscapeSampler : public protocols::moves::Mover {

public:

	/////////////////////
	/// Constructors  ///
	/////////////////////

	/// @brief Default constructor
	MembraneEnergyLandscapeSampler();

	/// @brief Non-defualt cnstructor for landscape sampling
	MembraneEnergyLandscapeSampler(
		std::string sfxn_weights,
		std::string rotation_type,
		bool interface = false
	);

	/// @brief Copy constructor (not needed unless you need deep copies)
	MembraneEnergyLandscapeSampler( MembraneEnergyLandscapeSampler const & src );

	/// @brief Destructor (important for properly forward-declaring smart-pointer members)
	~MembraneEnergyLandscapeSampler() override;

	/////////////////////
	/// Mover Methods ///
	/////////////////////

public:
	/// @brief Apply the mover
	void
	apply( core::pose::Pose & pose ) override;

	void
	show( std::ostream & output = std::cout ) const override;

	///////////////////////////////
	/// Rosetta Scripts Support ///
	///////////////////////////////

	/// @brief parse XML tag (to use this Mover in Rosetta Scripts)
	void
	parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	//MembraneEnergyLandscapeSampler & operator=( MembraneEnergyLandscapeSampler const & src );

	/// @brief required in the context of the parser/scripting scheme
	protocols::moves::MoverOP
	fresh_instance() const override;

	/// @brief required in the context of the parser/scripting scheme
	protocols::moves::MoverOP
	clone() const override;

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );

private: // methods

	protocols::membrane::RotationMoverOP
	get_rotation(
		core::Real const normal_angle,
		core::Vector const axis,
		core::Vector const rot_center,
		core::Size membrane_jump,
		std::string rotation_type
	);

private: // data

	core::scoring::ScoreFunctionOP sfxn_;
	std::string sfxn_weights_;
	std::string rotation_type_;
	bool interface_;

};

std::ostream &
operator<<( std::ostream & os, MembraneEnergyLandscapeSampler const & mover );

} //protocols
} //membrane_benchmark

#endif //protocols_membrane_benchmark_MembraneEnergyLandscapeSampler_HH