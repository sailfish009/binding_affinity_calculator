// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/stepwise/screener/StepWiseResiduePairScreener.hh
/// @brief
/// @details
/// @author Rhiju Das, rhiju@stanford.edu


#ifndef INCLUDED_protocols_stepwise_screener_StepWiseResiduePairScreener_HH
#define INCLUDED_protocols_stepwise_screener_StepWiseResiduePairScreener_HH

#include <protocols/stepwise/screener/StepWiseScreener.hh>
#include <protocols/stepwise/screener/StepWiseResiduePairScreener.fwd.hh>

namespace protocols {
namespace stepwise {
namespace screener {

class StepWiseResiduePairScreener: public StepWiseScreener {

public:

	//constructor
	StepWiseResiduePairScreener( core::Size const res1, core::Size const res2 );

	//destructor
	~StepWiseResiduePairScreener() override;

public:

	std::string
	name() const override = 0;

	StepWiseScreenerType
	type() const override = 0;

	void
	fast_forward( sampler::StepWiseSamplerOP sampler ) override;

protected:

	core::Size const res1_, res2_;

};

} //screener
} //stepwise
} //protocols

#endif
