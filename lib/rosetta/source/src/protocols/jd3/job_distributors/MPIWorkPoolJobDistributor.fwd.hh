// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/jd3/job_distributors/MPIWorkPoolJobDistributor.fwd.hh
/// @brief  forward header for MPIWorkPoolJobDistributor
/// @author Andy Watkins (amw579@nyu.edu)

#ifndef INCLUDED_protocols_jd3_job_distributors_MPIWorkPoolJobDistributor_fwd_hh
#define INCLUDED_protocols_jd3_job_distributors_MPIWorkPoolJobDistributor_fwd_hh

//utility headers
#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace jd3 {
namespace job_distributors {

class MPIWorkPoolJobDistributor;
typedef utility::pointer::shared_ptr< MPIWorkPoolJobDistributor > MPIWorkPoolJobDistributorOP;
typedef utility::pointer::shared_ptr< MPIWorkPoolJobDistributor const > MPIWorkPoolJobDistributorCOP;


}//job_distributors
}//jd3
}//protocols

#endif //INCLUDED_protocols_jd2_MPIWorkPoolJobDistributor_FWD_HH
