#!/bin/bash 
#
# (c) Copyright Rosetta Commons Member Institutions.
# (c) This file is part of the Rosetta software suite and is made available under license.
# (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
# (c) For more information, see http://www.rosettacommons.org. Questions about this can be
# (c) addressed to University of Washington CoMotion, email: license@uw.edu.
#
#  @author Ray Yu-Ruei Wang, wangyr@u.washington.edu
#
if [ ! -f run.lock ]; then
    ../../../scripts/cal_NonOverlapScores.py 
    sleep 5
    if [ -f run.lock ]; then # this means the previous step runs
        ../../../scripts/clean_scorefile.py \
            --selected_frags_path ../../Step1_Place_fragments_into_density/candidate_fragment_placements \
            -w \
            -t nonoverlap \
            -nw 10  
    fi
fi