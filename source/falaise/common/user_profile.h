//! \file falaise/common/user_profile.h - User profile
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen Normandie

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FALAISE_COMMON_USER_PROFILE_H
#define FALAISE_COMMON_USER_PROFILE_H

// Standard Library:
#include <string>
#include <set>

namespace falaise {

  namespace common {

    //! Return the set of supported user profiles. User profiles offer differents
    //! guarantees on data production.
    //!
    //! Use case for Monte Carlo data production (FLSimulate):
    //! - "expert" : This mode gives access to all configuration parameters
    //!   and command line switches. It is reserved for very experienced users
    //!   with a very good knowledge of the Bayeux/Falaise simulation framework.
    //!   It is supposed to be used to explore, debug, commission new simulation contexts.
    //!   No Monte Carlo data produced with this mode should be accepted in the official
    //!   datasets. Use it at your own risk!
    //! - "normal" : This mode gives access to a limited, but still rich, set of
    //!   configuration parameters and command line switches. Users have the
    //!   possibility to setup his/her own simulation setup for investigating and/or
    //!   testing new Monte-Carlo configurations. This is the default mode.
    //!   No Monte Carlo data produced with this mode should be accepted in the official
    //!   datasets but each configuration provided within this mode could be promoted
    //!   to an official one, after validation and acceptation by the Board!
    //! - "production" : This mode gives access to a more limited set of configuration parameters
    //!   and command line switches. Users have only the possibility to use blessed
    //!   setups (registered through URNs). Strict rules for data production are used.
    //!   This is the profile for production of 'blessed' Monte Carlo datasets. Only Monte Carlo
    //!   data produced with this mode should be accepted as official ones.
    //!
    const std::set<std::string> & supported_user_profiles();

  } // namespace common

} // namespace falaise

#endif // FALAISE_COMMON_USER_PROFILE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
