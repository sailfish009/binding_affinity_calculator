// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file
/// @brief
/// @author Spencer Bliven <blivens@u.washington.edu>
/// @date 6/26/2009

#ifndef INCLUDED_protocols_simple_moves_MutateResidue_hh
#define INCLUDED_protocols_simple_moves_MutateResidue_hh

#include <protocols/simple_moves/MutateResidue.fwd.hh>
#include <protocols/moves/Mover.hh>

// Project Headers
#include <core/pose/Pose.fwd.hh>
#include <core/chemical/AA.hh>
#include <core/types.hh>
#include <core/select/residue_selector/ResidueSelector.hh>

//parsing
#include <utility/tag/Tag.fwd.hh>
#include <basic/datacache/DataMap.fwd.hh>

#include <utility/vector1.hh>

namespace protocols {
namespace simple_moves {

/// @brief A mover to mutate a single residue
class MutateResidue : public protocols::moves::Mover
{
private:
	typedef protocols::moves::Mover parent;
public:
	/// @brief default ctor
	MutateResidue();
	/// @brief copy ctor
	MutateResidue(MutateResidue const& dm);
	/// @brief Mutate a single residue to a new amino acid
	/// @details new_res is three letter code in capital letters, example PHE
	MutateResidue( core::Size const target, std::string const & new_res ); // moving string
	MutateResidue( core::Size const target, int new_res/*one letter code*/);  // Changing char --> int so PyRosetta could use overloaded function

	MutateResidue( core::Size const target, core::chemical::AA const aa);

	~MutateResidue() override = default;

	void apply( core::pose::Pose & pose ) override;
	protocols::moves::MoverOP clone() const override {
		return (utility::pointer::make_shared< protocols::simple_moves::MutateResidue >( *this ) );
	}
	protocols::moves::MoverOP fresh_instance() const override {
		return utility::pointer::make_shared< MutateResidue >();
	}

	void parse_my_tag( utility::tag::TagCOP tag,
		basic::datacache::DataMap &
	) override;

	/// @brief Set this mover's target residue index.
	/// @author Vikram K. Mulligan (vmullig@uw.edu)
	void set_target( std::string const &target_in ) { target_ = target_in; }

	/// @brief Set this mover's target residue index, based on the Rosetta indexing.
	///
	void set_target(core::Size const target_in);

	/// @brief Set a selector if specified by the user
	///
	void set_selector( core::select::residue_selector::ResidueSelectorCOP selector_in);

	/// @brief Get this mover's target residue index.
	/// @author Vikram K. Mulligan (vmullig@uw.edu)
	std::string target() const { return target_; }

	/// @brief Set the residue to mutate to.
	/// @details This is the full name, not the three-letter code.
	/// @author Vikram K. Mulligan (vmullig@uw.edu)
	void set_res_name( std::string const &name_in ) { res_name_ = name_in; }

	void set_res_name( core::chemical::AA const & aa);

	/// @brief Set whether this mover updates polymer-dependent atoms in the residue that it mutates.
	/// @author Vikram K. Mulligan (vmullig@uw.edu)
	void set_update_polymer_dependent( bool const setting ) { update_polymer_dependent_ = setting; }

	/// @brief Set whether this mover updates polymer-dependent atoms in the residue that it mutates.
	/// @author Vikram K. Mulligan (vmullig@uw.edu)
	inline bool update_polymer_dependent() const { return update_polymer_dependent_; }

	/// @brief Set whether disulfide bonds are properly broken when
	/// mutating *from* a disulfide-bonded cysteine *to* anything else.
	/// @details True by default.  If false, the other cysteine will still think that
	/// it is disulfide-bonded to something, and scoring will fail.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
	void set_break_disulfide_bonds( bool const setting );

	/// @brief Get whether disulfide bonds are properly broken when
	/// mutating *from* a disulfide-bonded cysteine *to* anything else.
	/// @details True by default.  If false, the other cysteine will still think that
	/// it is disulfide-bonded to something, and scoring will fail.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
	inline bool break_disulfide_bonds() const { return break_disulfide_bonds_; }

	/// @brief Get the residue to mutate to.
	/// @details This is the full name, not the three-letter code.
	/// @author Vikram K. Mulligan (vmullig@uw.edu)
	std::string res_name() { return res_name_; }

	/// @brief Set whether the mover should try to preserve atoms' xyz coordinates or not.
	/// @author Vikram K. Mulligan (vmullig@uw.edu)
	void set_preserve_atom_coords( bool const val ) { preserve_atom_coords_ = val; }

	/// @brief Get whether the mover should try to preserve atoms' xyz coordinates or not.
	/// @author Vikram K. Mulligan (vmullig@uw.edu)
	bool preserve_atom_coords() { return preserve_atom_coords_; }



	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );

private: //Functions.

	/// @brief Actually make a mutation at a position, based on the current configuration of
	/// this mover.
	void make_mutation(core::pose::Pose &, core::Size);

	/// @brief Given that position X is involved in a disulfide bond, break the disulfide between
	/// the residue at position X and its partner.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
	void
	break_a_disulfide(
		core::pose::Pose & pose,
		core::Size const rosetta_target
	) const;


private:

	/// @brief The index of the residue to mutate.
	/// @details In Rosetta residue numbering, PDB numbering, or ReferencePose numbering.
	std::string target_;

	/// @brief The name (full name, not three-letter code) of the residue to mutate to.
	///
	std::string res_name_;

	/// @brief Should the mover try to preserve the xyz coordinates of the atoms in the side-chain if the
	/// new residue has an atom name matching an atom name in the old residue?  Default false.
	bool preserve_atom_coords_ = false;

	/// @brief If true, mutates the residue to itself, ie. Ala -> Ala. This fixes some problems with TerCards being attached to residues that shouldn't have them for RotamerLinks.
	bool mutate_self_ = false;

	/// @brief If true, updates polymer-dependent atoms.
	/// @details false by default.
	bool update_polymer_dependent_ = false;

	/// @brief If true, ensure that disulfide bonds are properly broken when
	/// mutating *from* a disulfide-bonded cysteine *to* anything else.
	/// @details True by default.  If false, the other cysteine will still think that
	/// it is disulfide-bonded to something, and scoring will fail.
	bool break_disulfide_bonds_ = true;

	///@brief gets a residue selector
	///
	core::select::residue_selector::ResidueSelectorCOP selector_;
};

} // moves
} // protocols

#endif //INCLUDED_protocols_simple_moves_MutateResidue_HH_
