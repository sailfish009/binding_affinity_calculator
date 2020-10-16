// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.
//////////////////////////////////////////////////////////////////////
///
/// @brief
/// A class for reading in the orbital type properties
///
/// @details
/// This class contains the ORBITALS INTERNAL_ICOOR data that is read in from residue_io.cc. Actually,
/// the data is set when residue_io.cc calls the command from residuetype.cc set_orbital_icoor. The data
/// is set and chills out in memory until needed. The actual xyz coordinates are not stored at this point.
/// xyz coordinates are constructed when conformation/Residue.hh asks for the build function in this class.
/// At that point, the coordinates are built and returned.
///
/// But wait, you say, why do you store the names of the atoms instead of the index of the atoms!? Well, the
/// problem occurs when residuetype reorders the indices of the atoms. When this occurrs, the indices for the icoor
/// are not reordered here. Another problem ocurs because orbital indices are not reordered in this process because
/// orbital indices are seperate from the atom indices. Regardless, when you build the xyz coords, this step is transparent
/// because the function orbital_xyz() in residue.hh takes care of this conversion of indice to string.
///
/// @note NOTE!!!!!!!!!!! The internal coordinates cannot contain an orbital as the stub1, stub2, or stub3 atom.
/// This is because the xyz coordinates are not updated when the conformation changes. The stub1, stub2, stub2 atoms
/// must be actual atoms and not orbitals!!!!! (design feature or flaw? you decide)
///
///
/// @author
/// Steven Combs
///
///
/////////////////////////////////////////////////////////////////////////


#include <core/chemical/orbitals/ICoorOrbitalData.hh>

// Utility headers
#include <core/kinematics/Stub.hh>

#ifdef    SERIALIZATION
// Utility serialization headers
#include <utility/serialization/serialization.hh>
#include <cereal/types/string.hpp>
#endif // SERIALIZATION

namespace core {
namespace chemical {
namespace orbitals {

/// @brief construct ICoorOrbitalData.
ICoorOrbitalData::ICoorOrbitalData():
	phi_(0.0),
	theta_(0.0),
	distance_(0.0)
{}


//Testing size orbitals
ICoorOrbitalData::ICoorOrbitalData(
	Real phi,
	Real theta,
	Real distance,
	std::string const & stub1,
	std::string const & stub2,
	std::string const & stub3
):
	phi_(phi),
	theta_(theta),
	distance_(distance),
	stub1_(stub1),
	stub2_(stub2),
	stub3_(stub3)
{}

/// @brief return the phi for a given orbital
Real ICoorOrbitalData::phi() const
{
	return phi_;
}

/// @brief return the theta for a given orbital
Real ICoorOrbitalData::theta() const
{
	return theta_;
}

/// @brief return the distance for a given orbital
Real ICoorOrbitalData::distance() const
{
	return distance_;
}


/// @brief build the xyz coordinates for an orbital based upon the stub1, stub2, stub3 xyz coordinates.
/// @note NOTE!!!!!!!!!!! The internal coordinates cannot contain an orbital as the stub1, stub2, or stub3 atom.
/// This is because the xyz coordinates are not updated when the conformation changes. The stub1, stub2, stub2 atoms
/// must be actual atoms and not orbitals!!!!11111!!!!!!!11111!(design feature or flaw? you decide)
Vector
ICoorOrbitalData::build(
	Vector stub1_xyz,
	Vector stub2_xyz,
	Vector stub3_xyz) const
{
	debug_assert( kinematics::Stub( stub1_xyz,
		stub2_xyz,
		stub3_xyz).is_orthogonal( 0.001 ) );

	return kinematics::Stub(stub1_xyz, stub2_xyz, stub3_xyz).spherical(phi_, theta_, distance_);
}


}
}
}

#ifdef    SERIALIZATION

/// @brief Automatically generated serialization method
template< class Archive >
void
core::chemical::orbitals::ICoorOrbitalData::save( Archive & arc ) const {
	arc( CEREAL_NVP( phi_ ) ); // Real
	arc( CEREAL_NVP( theta_ ) ); // Real
	arc( CEREAL_NVP( distance_ ) ); // Real
	arc( CEREAL_NVP( stub1_ ) ); // std::string
	arc( CEREAL_NVP( stub2_ ) ); // std::string
	arc( CEREAL_NVP( stub3_ ) ); // std::string
}

/// @brief Automatically generated deserialization method
template< class Archive >
void
core::chemical::orbitals::ICoorOrbitalData::load( Archive & arc ) {
	arc( phi_ ); // Real
	arc( theta_ ); // Real
	arc( distance_ ); // Real
	arc( stub1_ ); // std::string
	arc( stub2_ ); // std::string
	arc( stub3_ ); // std::string
}
SAVE_AND_LOAD_SERIALIZABLE( core::chemical::orbitals::ICoorOrbitalData );
#endif // SERIALIZATION
