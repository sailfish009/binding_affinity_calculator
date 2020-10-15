#!/bin/bash

$ROSETTA3_SRC/bin/relax.default.linuxgccrelease \
 -database $ROSETTA3_DB \
 -in::file::extended_pose 1 \
 -in::file::fasta 1onc.fasta \
 -in::file::alignment 1onc_1oj8.fasta \
 -in::file::template_pdb 1OJ8A.pdb \
 -relax:fast \
 -relax::default_repeats 4 \
 -relax::jump_move true \
 -edensity::mapfile 1ONC.5A.mrc \
 -edensity::mapreso 5.0 \
 -edensity::grid_spacing 2.0 \
 -edensity::whole_structure_ca_wt 0.1 \
 -overwrite
