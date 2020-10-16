// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/SecondaryStructureEnergy.hh
/// @brief  Statistically derived rotamer pair potential class declaration
/// @author Phil Bradley


#ifndef INCLUDED_core_scoring_methods_SecondaryStructureEnergy_hh
#define INCLUDED_core_scoring_methods_SecondaryStructureEnergy_hh

// Unit Headers
#include <core/energy_methods/SecondaryStructureEnergy.fwd.hh>

// Package headers
#include <core/scoring/methods/WholeStructureEnergy.hh>
#include <core/scoring/SecondaryStructurePotential.fwd.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>

#include <utility/vector1.hh>


// Utility headers


namespace core {
namespace scoring {
namespace methods {


class SecondaryStructureEnergy : public WholeStructureEnergy  {
public:
	typedef WholeStructureEnergy  parent;

public:


	SecondaryStructureEnergy();


	SecondaryStructureEnergy( SecondaryStructureEnergy const & src );


	/// clone
	EnergyMethodOP
	clone() const override;

	/////////////////////////////////////////////////////////////////////////////
	// scoring
	/////////////////////////////////////////////////////////////////////////////

	void
	setup_for_scoring( pose::Pose & pose, ScoreFunction const & scorefxn ) const override;


	void
	finalize_total_energy(
		pose::Pose & pose,
		ScoreFunction const &,
		EnergyMap & totals
	) const override;


	/// @brief The SecondaryStructureEnergy class requires that the EnergyGraph
	/// span 12 Angstroms between centroids.  The centroids residues build-in a
	/// 3 Angstrom radius each.
	Distance
	atomic_interaction_cutoff() const override;


	void indicate_required_context_graphs( utility::vector1< bool > & context_graphs_required ) const override;


	/////////////////////////////////////////////////////////////////////////////
	// data
	/////////////////////////////////////////////////////////////////////////////

private:

	/// const-ref to scoring database
	SecondaryStructurePotential const & potential_;
	core::Size version() const override;
};


}
}
}

#endif // INCLUDED_core_scoring_ScoreFunction_HH
