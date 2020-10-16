// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file DPScoringScheme.hh
/// @brief class definition for a profile-profile scoring scheme for
/// comparing two sequence profiles using the DP city-block distance.
/// @details Simply based on comparing single profiles from two protein
/// sequences by computing the sum of P(A) - P(B) for all matching probabilities
/// in the profiles A and B. Also includes with affine gap penalties of the form
/// penalty = A + Bk, where A represents the penalty for starting a gap, and B
/// represents the penalty for extending a previously opened gap by k
/// characters.
/// @author James Thompson

#ifndef INCLUDED_core_sequence_DPScoringScheme_hh
#define INCLUDED_core_sequence_DPScoringScheme_hh

#include <core/types.hh>
#include <core/sequence/Sequence.fwd.hh>
#include <core/sequence/ScoringScheme.hh>

namespace core {
namespace sequence {

class DPScoringScheme : public ScoringScheme {
public:

	/// @brief ctor
	DPScoringScheme(
		Real open   = -4,
		Real extend = -1
	)
	{
		gap_open  ( open );
		gap_extend( extend );
		type("DP");
	}

	ScoringSchemeOP clone() const override {
		return utility::pointer::make_shared< DPScoringScheme >(
			gap_open(),
			gap_extend()
		);
	}

	/// @brief dtor
	~DPScoringScheme() override = default;

	Real score( SequenceOP seq1, SequenceOP seq2, Size pos1, Size pos2 ) override;
}; // class DPScoringScheme

} // sequence
} // core

#endif
