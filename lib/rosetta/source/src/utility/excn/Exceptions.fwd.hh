// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   utility/excn/Exceptions.fwd.hh
/// @brief  Declarations for many of the common exception subclasses
/// @author Oliver Lange
/// @author Sergey Lyskov


#ifndef INCLUDED_utility_excn_Exceptions_FWD_HH
#define INCLUDED_utility_excn_Exceptions_FWD_HH

namespace utility {
namespace excn {

class Exception;
//using EXCN_Msg_Exception = EXCN_Exception;
class IOError;
class BadInput;
class FileNotFound;
class RangeError;
class KeyError;
class NullPointerError;
class RosettaScriptsOptionError;
class JD2Failure;

}
}

#endif