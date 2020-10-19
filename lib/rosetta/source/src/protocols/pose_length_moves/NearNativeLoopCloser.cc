// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file src/protocols/moves/NearNativeLoopCloser.fwd.hh
/// @brief Uses KIC/CCD and a fast Look back to close loops near native
///
/// @author TJ Brunette tjbrunette@gmail.com
///
// Unit headers
#include <protocols/pose_length_moves/NearNativeLoopCloser.hh>
#include <protocols/pose_length_moves/NearNativeLoopCloserCreator.hh>
#include <protocols/moves/Mover.hh>

#include <protocols/simple_moves/SwitchChainOrderMover.hh>
#include <protocols/jd2/util.hh>

// Core Headers
#include <core/chemical/AA.hh>
#include <core/chemical/ChemicalManager.hh>
#include <core/chemical/util.hh>
#include <core/chemical/ResidueTypeSet.hh>
#include <core/conformation/Residue.hh>
#include <core/conformation/ResidueFactory.hh>
#include <core/scoring/func/HarmonicFunc.hh>
#include <core/scoring/dssp/Dssp.hh>
#include <core/sequence/ABEGOManager.hh>
#include <core/util/SwitchResidueTypeSet.hh>

#include <core/id/TorsionID.hh>
#include <core/id/types.hh>

#include <protocols/indexed_structure_store/SSHashedFragmentStore.hh>
#include <protocols/indexed_structure_store/FragmentStore.hh>

#include <core/kinematics/AtomTree.hh>
#include <core/kinematics/FoldTree.hh>
#include <core/kinematics/Jump.hh>
#include <core/kinematics/MoveMap.hh>

#include <core/optimization/MinimizerOptions.hh>
#include <core/optimization/Minimizer.hh>
#include <core/optimization/MinimizerOptions.hh>
#include <core/optimization/AtomTreeMinimizer.hh>
#include <core/optimization/CartesianMinimizer.hh>
#include <protocols/simple_moves/ConstrainToIdealMover.hh>


#include <core/pose/datacache/CacheableDataType.hh>
#include <core/pose/Pose.hh>
#include <core/pose/variant_util.hh>
#include <core/pose/chains_util.hh>
#include <core/pose/subpose_manipulation_util.hh>
#include <core/pose/PDBInfo.hh>

#include <core/scoring/dssp/Dssp.hh>
#include <core/scoring/func/CircularSplineFunc.hh>
#include <core/scoring/func/CircularHarmonicFunc.hh>
#include <core/scoring/func/Func.hh>
#include <core/scoring/func/Func.fwd.hh>
#include <core/scoring/constraints/CoordinateConstraint.hh>
#include <core/scoring/constraints/ConstraintIO.hh>
#include <core/scoring/constraints/ConstraintSet.hh>
#include <core/scoring/constraints/DihedralConstraint.hh>
#include <core/scoring/constraints/util.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>

#include <core/sequence/ABEGOManager.hh>
#include <core/sequence/SSManager.hh>

#include <core/types.hh>

#include <basic/datacache/DataMap.hh>
#include <basic/datacache/DataCache.hh>
#include <basic/datacache/BasicDataCache.hh>
#include <basic/datacache/CacheableString.hh>
#include <basic/datacache/CacheableStringMap.hh>

#include <basic/options/keys/remodel.OptionKeys.gen.hh>
#include <basic/options/option.hh>
#include <basic/Tracer.hh>

#include <utility/string_util.hh>
#include <utility/vector1.hh>
#include <utility/tag/Tag.hh>
#include <utility/VirtualBase.hh>

#include <numeric/conversions.hh>
#include <numeric/alignment/QCPKernel.hh>
#include <numeric/xyzVector.hh>
#include <numeric/xyzMatrix.hh>
#include <numeric/xyzTransform.hh>
#include <numeric/xyz.functions.hh>

#include <Eigen/Core>

#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <ctime>
//output
#include <utility/io/ozstream.hh>
#include <ObjexxFCL/format.hh>

// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

//#include <unistd.h>

static basic::Tracer TR( "protocols.pose_length_moves.NearNativeLoopCloser" );

namespace protocols {
namespace pose_length_moves {
using namespace core;
using namespace std;
using utility::vector1;

PossibleLoop::PossibleLoop(int resAdjustmentBeforeLoop, int resAdjustmentAfterLoop,core::Size loopLength,core::Size resBeforeLoop, core::Size resAfterLoop, char resTypeBeforeLoop, char resTypeAfterLoop, core::Size insertedBeforeLoopRes, core::Size insertedAfterLoopRes, core::pose::PoseOP fullLengthPoseOP, core::pose::PoseOP orig_atom_type_fullLengthPoseOP){
	TR << "creatingPotentialLoop" <<  resAdjustmentBeforeLoop << "," << resAdjustmentAfterLoop <<"," << loopLength << "," << resBeforeLoop << ","<< resAfterLoop << std::endl;
	resAdjustmentBeforeLoop_ = resAdjustmentBeforeLoop;
	resAdjustmentAfterLoop_ = resAdjustmentAfterLoop;
	loopLength_ = loopLength;
	resBeforeLoop_ = resBeforeLoop;
	fullLength_resBeforeLoop_ = resBeforeLoop;
	resAfterLoop_ = resAfterLoop;
	resTypeBeforeLoop_ = resTypeBeforeLoop;
	resTypeAfterLoop_ = resTypeAfterLoop;
	insertedBeforeLoopRes_ = insertedBeforeLoopRes;
	insertedAfterLoopRes_ = insertedAfterLoopRes;
	original_atom_type_fullLengthPoseOP_ = orig_atom_type_fullLengthPoseOP;
	fullLengthPoseOP_ = fullLengthPoseOP;
	below_distance_threshold_=false;
	stub_rmsd_match_= 9999;
	uncached_stub_rmsd_=9999;
	final_rmsd_=9999;
	outputed_ = false;
}

PossibleLoop::~PossibleLoop()= default;

void PossibleLoop::trimRegion(core::pose::PoseOP & poseOP, core::Size resStart, core::Size resStop){
	poseOP->conformation().delete_residue_range_slow(resStart,resStop);
	renumber_pdbinfo_based_on_conf_chains(*poseOP,true,false,false,false);
}


void PossibleLoop::extendRegion(bool towardCTerm, core::Size resStart, core::Size numberAddRes,core::pose::PoseOP & poseOP){
	using namespace basic::options;
	using namespace basic::options::OptionKeys;
	using namespace chemical;
	//could just pick a random phi/psi
	Real tmpPhi = -57.8;
	Real tmpPsi = -47.0;
	Real tmpOmega = 180.0; //I extend with helical parameters. They get replaced during kic.
	core::conformation::ResidueOP new_rsd( nullptr );
	string build_aa_type_one_letter =option[OptionKeys::remodel::generic_aa];
	string build_aa_type = name_from_aa(aa_from_oneletter_code(build_aa_type_one_letter[0]));
	debug_assert( poseOP != nullptr );
	core::chemical::ResidueTypeSetCOP rs( poseOP->residue_type_set_for_pose() );
	kinematics::FoldTree ft;
	if ( towardCTerm == true ) {
		ft = poseOP->fold_tree();
		ft.clear();
		ft.add_edge(1,resStart,core::kinematics::Edge::PEPTIDE);
		ft.add_edge(1,poseOP->size(),1); //edges need to be added in order so the fold tree is completely connected.
		ft.add_edge(poseOP->size(),resStart+1,core::kinematics::Edge::PEPTIDE);
		poseOP->fold_tree(ft);
		for ( core::Size ii=0; ii<numberAddRes; ++ii ) {
			new_rsd = core::conformation::ResidueFactory::create_residue( rs->name_map(build_aa_type) );
			poseOP->conformation().safely_append_polymer_residue_after_seqpos( *new_rsd,resStart+ii, true);
		}
	} else {
		ft = poseOP->fold_tree();
		ft.clear();
		ft.add_edge(1,resStart-1,core::kinematics::Edge::PEPTIDE);
		ft.add_edge(1,poseOP->size(),1); //edges need to be added in order so the fold tree is completely connected.
		ft.add_edge(poseOP->size(),resStart,core::kinematics::Edge::PEPTIDE);
		poseOP->fold_tree(ft);
		for ( core::Size ii=0; ii<numberAddRes; ++ii ) {
			new_rsd = core::conformation::ResidueFactory::create_residue( rs->name_map(build_aa_type) );
			poseOP->conformation().safely_prepend_polymer_residue_before_seqpos( *new_rsd,resStart, true);
		}
	}
	for ( core::Size ii=0; ii<numberAddRes; ++ii ) {
		poseOP->set_phi(resStart+ii, tmpPhi );
		poseOP->set_psi(resStart+ii, tmpPsi );
		poseOP->set_omega(resStart+ii, tmpOmega );
	}
	poseOP->set_phi(resStart+numberAddRes, tmpPhi );
	ft = poseOP->fold_tree();
	ft.clear();
	ft.add_edge(1,poseOP->size(),core::kinematics::Edge::PEPTIDE);
	poseOP->fold_tree(ft);
	renumber_pdbinfo_based_on_conf_chains(*poseOP,true,false,false,false);
}

void PossibleLoop::evaluate_distance_closure(){
	Real maxDist = loopLength_*4.3; //4.3 is the new max. Originally this was 4 but in crystal structure 4tql there is a 3 residue loop
	//(72-74) that when measured from 71-75 measures 12.6.
	core::Size tmpResidueBeforeLoop = resBeforeLoop_+resAdjustmentBeforeLoop_;
	core::Size tmpResidueAfterLoop = resBeforeLoop_+insertedBeforeLoopRes_+insertedAfterLoopRes_-resAdjustmentAfterLoop_+1;
	Real currentDist = fullLengthPoseOP_->residue(tmpResidueBeforeLoop).xyz("CA").distance(fullLengthPoseOP_->residue(tmpResidueAfterLoop).xyz("CA"));
	//std::cout << "XresBeforeLoop_" << tmpResidueBeforeLoop << " resAfterLoop_" << tmpResidueAfterLoop << " loopLength_" << loopLength_ << "resBeforeLoop_" << resBeforeLoop_ << "resAfterLoop_" << resAfterLoop_ << " currentDist" << currentDist << "maxDist" << maxDist << std::endl;
	if ( currentDist < maxDist ) {
		below_distance_threshold_=true;
		stub_rmsd_match_=999;//so they get sorted to the front
		uncached_stub_rmsd_=999;
	} else {
		below_distance_threshold_=false;
	}
}


void PossibleLoop::generate_stub_rmsd(){
	SSHashedFragmentStore_ = protocols::indexed_structure_store::SSHashedFragmentStore::get_instance();
	core::Size tmpResidueBeforeLoop = resBeforeLoop_+resAdjustmentBeforeLoop_;
	core::Size tmpResidueAfterLoop = resBeforeLoop_+insertedBeforeLoopRes_+insertedAfterLoopRes_-resAdjustmentAfterLoop_+1;
	std::vector< numeric::xyzVector<numeric::Real> > coordinates;
	vector1<core::Size> residues;
	residues.push_back(tmpResidueBeforeLoop-1);
	residues.push_back(tmpResidueBeforeLoop);
	residues.push_back(tmpResidueAfterLoop);
	residues.push_back(tmpResidueAfterLoop+1);
	for ( core::Size ii=1; ii<=residues.size(); ++ii ) {
		for ( std::string const & atom_name : SSHashedFragmentStore_->get_fragment_store()->fragment_specification.fragment_atoms ) {
			coordinates.push_back(fullLengthPoseOP_->residue(residues[ii]).xyz(atom_name));
		}
	}
	stub_rmsd_match_= 9999;
	stub_index_match_= 9999;
	stub_ss_index_match_= 9999;
	SSHashedFragmentStore_->lookback_stub(coordinates,resTypeBeforeLoop_,resTypeAfterLoop_,loopLength_, stub_rmsd_match_,stub_index_match_,stub_ss_index_match_);
}


void PossibleLoop::generate_uncached_stub_rmsd(){
	SSHashedFragmentStore_ = protocols::indexed_structure_store::SSHashedFragmentStore::get_instance();
	uncached_stub_rmsd_= 9999;
	uncached_stub_index_= 9999;
	core::Size tmpResidueBeforeLoop = resBeforeLoop_+resAdjustmentBeforeLoop_;
	core::Size tmpResidueAfterLoop = resBeforeLoop_+insertedBeforeLoopRes_+insertedAfterLoopRes_-resAdjustmentAfterLoop_+1;
	std::vector< numeric::xyzVector<numeric::Real> > coordinates;
	vector1<core::Size> residues;
	residues.push_back(tmpResidueBeforeLoop-1);
	residues.push_back(tmpResidueBeforeLoop);
	for ( core::Size ii=0; ii<9-2-loopLength_; ++ii ) {
		if ( tmpResidueAfterLoop+ii < fullLengthPoseOP_->size() ) {
			residues.push_back(tmpResidueAfterLoop+ii);
		}
	}
	for ( core::Size ii=1; ii<=residues.size(); ++ii ) {
		for ( std::string const & atom_name : SSHashedFragmentStore_->get_fragment_store()->fragment_specification.fragment_atoms ) {
			coordinates.push_back(fullLengthPoseOP_->residue(residues[ii]).xyz(atom_name));
		}
	}
	SSHashedFragmentStore_->lookback_uncached_stub(coordinates,stub_ss_index_match_,loopLength_,uncached_stub_rmsd_,uncached_stub_index_);
}

core::pose::PoseOP PossibleLoop::get_finalPoseOP(){
	bool fullatom_original = original_atom_type_fullLengthPoseOP_->is_fullatom();
	bool fullatom_finalPose = finalPoseOP_->is_fullatom();
	if ( !fullatom_original || (fullatom_original && fullatom_finalPose) ) {
		return(finalPoseOP_);
	} else {
		core::util::switch_to_residue_type_set(*finalPoseOP_, core::chemical::FA_STANDARD);
		//fix the residues before the loop
		for ( core::Size ii=1; ii<=resBeforeLoop_; ++ii ) {
			finalPoseOP_->replace_residue(ii, original_atom_type_fullLengthPoseOP_->residue(ii), true );
		}
		//fix the residues after the loop
		core::Size residue_after_loop_orig = fullLength_resBeforeLoop_+insertedBeforeLoopRes_+insertedAfterLoopRes_-resAdjustmentAfterLoop_+1;
		//std::cout << "fullLength_resBeforeLoop_" << fullLength_resBeforeLoop_ << "," <<  insertedBeforeLoopRes_ << "," << insertedAfterLoopRes_ << "," << resAdjustmentAfterLoop_ << std::endl;
		core::Size residue_to_change = finalPoseOP_->total_residue()-resAfterLoop_+1;
		for ( core::Size ii=0; ii<residue_to_change; ++ii ) {
			core::Size orig_res_numb =  residue_after_loop_orig+ii;
			core::Size current_res_numb = resAfterLoop_+ii;
			finalPoseOP_->replace_residue(current_res_numb, original_atom_type_fullLengthPoseOP_->residue(orig_res_numb), true );
		}
		return(finalPoseOP_);
	}

}

void PossibleLoop::generate_output_pose(bool output_closed,bool ideal_loop, Real rms_threshold,std::string allowed_loop_abegos, std::string closure_type){
	using namespace core::chemical;
	//step 1 : generate trim pose.
	//core::Size tmpResidueBeforeLoop = resBeforeLoop_+resAdjustmentBeforeLoop_;
	//core::Size tmpResidueAfterLoop = resAfterLoop_+insertedBeforeLoopRes_+insertedAfterLoopRes_+resAdjustmentAfterLoop_;
	//trim residues assume the residues after the loop are trimmed first
	core::Size trim_res_start_after_loop =resBeforeLoop_+insertedBeforeLoopRes_+1;
	core::Size trim_res_stop_after_loop =resBeforeLoop_+insertedBeforeLoopRes_+insertedAfterLoopRes_-resAdjustmentAfterLoop_;
	core::Size trim_res_start_before_loop = resBeforeLoop_+resAdjustmentBeforeLoop_+1;//want this there. Should be 1 after this residue
	core::Size trim_res_stop_before_loop = resBeforeLoop_+insertedBeforeLoopRes_;//last residue checks out
	//trim should be 401-401 on right side
	core::pose::PoseOP working_poseOP = fullLengthPoseOP_->clone();
	//remember these could change
	if ( trim_res_start_after_loop <= trim_res_stop_after_loop ) {
		trimRegion(working_poseOP,trim_res_start_after_loop,trim_res_stop_after_loop);
	}
	if ( trim_res_start_before_loop <= trim_res_stop_before_loop ) {
		trimRegion(working_poseOP,trim_res_start_before_loop,trim_res_stop_before_loop);
	}
	resBeforeLoop_=resBeforeLoop_+resAdjustmentBeforeLoop_;
	resAfterLoop_=resBeforeLoop_+1;
	if ( !output_closed ) {
		finalPoseOP_=working_poseOP;
	} else {
		SSHashedFragmentStore_ = protocols::indexed_structure_store::SSHashedFragmentStore::get_instance();
		core::Size fragment_length = SSHashedFragmentStore_->get_fragment_length();
		if ( closure_type == "kic" ) {
			core::scoring::ScoreFunctionOP scorefxn_tmp( core::scoring::ScoreFunctionFactory::create_score_function("score3"));
			extendRegion(true, resBeforeLoop_, loopLength_,working_poseOP);
			resAfterLoop_=resAfterLoop_+loopLength_;
			bool success = kic_closure(scorefxn_tmp,working_poseOP,resBeforeLoop_,resAfterLoop_,200);
			vector1<core::Size> resids;
			for ( int ii=resBeforeLoop_-3; ii<=(int)resBeforeLoop_+3; ii=ii+2 ) {
				core::Size tmp_resid = get_valid_resid(ii,*working_poseOP);
				resids.push_back(tmp_resid);
			}
			Real rmsd = SSHashedFragmentStore_->max_rmsd_in_region(*working_poseOP,resids);
			if ( success ) {
				final_rmsd_=rmsd;
				finalPoseOP_=working_poseOP;
			}
		}
		if ( closure_type == "lookback" ) {
			//Step 2 : Add loop residues
			extendRegion(true, resBeforeLoop_, loopLength_,working_poseOP);
			resAfterLoop_=resAfterLoop_+loopLength_;
			//Step 3 : Generate coordinate constraints
			working_poseOP->remove_constraints();
			add_coordinate_csts_from_lookback(stub_ss_index_match_,uncached_stub_index_,resBeforeLoop_-1,true,working_poseOP);
			add_dihedral_csts_from_lookback(stub_ss_index_match_,uncached_stub_index_,resBeforeLoop_-1,working_poseOP);
			//Step 3 : Generate score function
			core::scoring::ScoreFunctionOP scorefxn( core::scoring::ScoreFunctionFactory::create_score_function("score3") );
			scorefxn->set_weight(core::scoring::coordinate_constraint, 0.7);
			scorefxn->set_weight(core::scoring::dihedral_constraint, 20.0 );
			scorefxn->set_weight(core::scoring::rama,1.0);
			scorefxn->set_weight(core::scoring::cart_bonded, 1.0 );
			scorefxn->set_weight(core::scoring::cart_bonded_length,0.2);
			scorefxn->set_weight(core::scoring::cart_bonded_angle,0.5);
			scorefxn->set_weight(core::scoring::vdw, 2 );
			//step 3: assign_phi_psi
			assign_phi_psi_omega_from_lookback(stub_ss_index_match_,uncached_stub_index_, working_poseOP);
			//step 4: initial minimize
			minimize_loop(scorefxn,ideal_loop,working_poseOP);
			//Get rmsd
			vector1<core::Size> resids;
			for ( int ii=resBeforeLoop_-3; ii<=(int)resBeforeLoop_+3; ii=ii+2 ) {
				core::Size tmp_resid = get_valid_resid(ii,*working_poseOP);
				resids.push_back(tmp_resid);
			}
			Real current_rmsd = SSHashedFragmentStore_->max_rmsd_in_region(*working_poseOP,resids);
			//step 5: iterative minimize over multiple lookbacks
			core::sequence::SSManager SM;
			std::string fragStore_ss_string = SM.index2symbolString(stub_ss_index_match_,fragment_length);
			core::scoring::dssp::Dssp frag_dssp( *working_poseOP);
			frag_dssp.dssp_reduced();
			std::string tmp_dssp = frag_dssp.get_dssp_secstruct();
			std::string desired_dssp = tmp_dssp.substr(0,resBeforeLoop_-1-1)+fragStore_ss_string+tmp_dssp.substr(resBeforeLoop_+fragment_length-1-1,tmp_dssp.length());
			Real match_rmsd;
			core::Size match_index;
			core::Size match_ss_index;
			scorefxn->set_weight(core::scoring::coordinate_constraint, 0.4);
			//iterate---
			for ( core::Size jj=1; jj<=2 && current_rmsd<rms_threshold+0.5; ++jj ) {//The 0.5 RMSD is arbitrary.
				working_poseOP->remove_constraints();
				for ( core::Size ii=1; ii<=resids.size(); ++ii ) {
					std::string frag_ss = desired_dssp.substr(resids[ii]-1,9);
					Real tmp_rmsd = SSHashedFragmentStore_->lookback_account_for_dssp_inaccuracy(*working_poseOP,resids[ii],frag_ss,match_rmsd,match_index,match_ss_index);
					if ( tmp_rmsd < rms_threshold+0.5 ) {
						add_coordinate_csts_from_lookback(match_ss_index,match_index,resids[ii],false,working_poseOP);
					}
				}
				minimize_loop(scorefxn,ideal_loop,working_poseOP);
				current_rmsd = SSHashedFragmentStore_->max_rmsd_in_region(*working_poseOP,resids);
				//std::cout << "rd" << jj << " rmsd" << current_rmsd << std::endl;
			}
			final_rmsd_= current_rmsd;
			finalPoseOP_=working_poseOP;
			//Step 5: Kic to initially close loops if ideal
			if ( ideal_loop ) {
				//uses kic to close the last couple residues in the loop
				core::Size res_from_end_of_loop=3;
				core::Size firstLoopRes= resAfterLoop_-res_from_end_of_loop;
				core::Size lastLoopRes=resAfterLoop_;
				bool success = kic_closure(scorefxn,working_poseOP,firstLoopRes,lastLoopRes,10);
				Real rmsd = SSHashedFragmentStore_->max_rmsd_in_region(*working_poseOP,resids);
				if ( success ) {
					final_rmsd_=rmsd;
					finalPoseOP_=working_poseOP;
				} else {
					final_rmsd_ = 999;
				}
			}
			if ( allowed_loop_abegos!="" ) {
				bool valid_loop_abego = check_loop_abego(working_poseOP,resBeforeLoop_,resAfterLoop_,allowed_loop_abegos,final_rmsd_);
				if ( !valid_loop_abego ) {
					final_rmsd_=999;
				}
			}
		}
	}
}

bool PossibleLoop::check_loop_abego(core::pose::PoseOP & poseOP, core::Size resBeforeLoop, core::Size resAfterLoop, std::string allowed_loop_abegos, Real loop_rmsd ){
	utility::vector1< std::string > const abego_v = core::sequence::get_abego( *poseOP );
	core::Size neighboring_residues = 3;
	core::sequence::ABEGOManager am;
	std::string abego = am.get_abego_string(abego_v);
	core::Size loop_cut_length = resAfterLoop+neighboring_residues-(resBeforeLoop-neighboring_residues);
	std::string loop_abego = abego.substr(resBeforeLoop-neighboring_residues-1,loop_cut_length);//1 is for the string conversion
	utility::vector1< std::string > allowed_abegos_v( utility::string_split( allowed_loop_abegos , ',' ) );
	bool found = false;
	for ( auto & allowed_abego : allowed_abegos_v ) {
		core::Size position = loop_abego.find(allowed_abego);
		if ( position != std::string::npos ) {
			found=true;
			TR << "ABEGO loop found:" << allowed_abego << " with RMSD" << loop_rmsd << std::endl;
		}
	}
	return(found);
}


void PossibleLoop::assign_phi_psi_omega_from_lookback(core::Size db_index, core::Size fragment_index, core::pose::PoseOP & poseOP){
	vector<Real> phi_v = SSHashedFragmentStore_->get_fragment_store(db_index)->realVector_groups["phi"][fragment_index];
	vector<Real> psi_v = SSHashedFragmentStore_->get_fragment_store(db_index)->realVector_groups["psi"][fragment_index];
	vector<Real> omega_v = SSHashedFragmentStore_->get_fragment_store(db_index)->realVector_groups["omega"][fragment_index];
	//to keep the structure ideal I don't assign omega
	kinematics::FoldTree ft;
	ft = poseOP->fold_tree();
	ft.clear();
	ft.add_edge(1,resBeforeLoop_+loopLength_,core::kinematics::Edge::PEPTIDE);
	ft.add_edge(1,poseOP->size(),1); //edges need to be added in order so the fold tree is completely connected.
	ft.add_edge(poseOP->size(),resAfterLoop_,core::kinematics::Edge::PEPTIDE);
	poseOP->fold_tree(ft);
	for ( core::Size ii=0; ii<loopLength_+2; ++ii ) {//set phi psi for residues after loop
		poseOP->set_phi(resBeforeLoop_+ii,phi_v[ii+1]);
		poseOP->set_psi(resBeforeLoop_+ii,psi_v[ii+1]);
		poseOP->set_omega(resBeforeLoop_+ii,omega_v[ii+1]);
	}
}


vector<Real> PossibleLoop::get_center_of_mass(const Real* coordinates, int number_of_atoms){
	vector<Real> center;
	center.push_back(0);
	center.push_back(0);
	center.push_back(0);

	for ( int n = 0; n < number_of_atoms * 3; n += 3 ) {
		center[0] += coordinates[n + 0];
		center[1] += coordinates[n + 1];
		center[2] += coordinates[n + 2];
	}

	center[0] /= number_of_atoms;
	center[1] /= number_of_atoms;
	center[2] /= number_of_atoms;

	return(center);
}

void PossibleLoop::output_fragment_debug(std::vector< numeric::xyzVector<numeric::Real> > coordinates,std::string filename){
	using namespace ObjexxFCL::format;
	utility::io::ozstream out(filename, std::ios_base::app);
	core::Size resid = 1;
	for ( auto & coordinate : coordinates ) {
		out << "ATOM  " << I(5,resid) << "  CA  " <<
			"GLY" << ' ' << 'A' << I(4,resid ) << "    " <<
			F(8,3,coordinate.x()) <<
			F(8,3,coordinate.y()) <<
			F(8,3,coordinate.z()) <<
			F(6,2,1.0) << F(6,2,1.0) << '\n';
		resid++;
	}
	out << "ATOM  " << I(5,resid) << "  CA  " <<
		"GLY" << ' ' << 'A' << I(4,resid) << "    " <<
		F(8,3,0.0) <<
		F(8,3,0.0) <<
		F(8,3,0.0) <<
		F(6,2,1.0) << F(6,2,1.0) << '\n';
	resid++;
	out << "ENDMDL\n";
	out.close();
}



void PossibleLoop::add_coordinate_csts_from_lookback(core::Size stub_ss_index_match, core::Size fragment_index, core::Size pose_residue, bool match_stub_alone, core::pose::PoseOP & poseOP){
	using namespace basic::options;
	using namespace basic::options::OptionKeys;
	using namespace core::scoring;
	using namespace core::scoring::constraints;
	using namespace chemical;
	using namespace protocols::generalized_kinematic_closure;

	typedef numeric::alignment::QCPKernel< numeric::Real > QCPKernel;

	std::vector< numeric::xyzVector<numeric::Real> > fragCoordinates = SSHashedFragmentStore_->get_fragment_coordinates(stub_ss_index_match,fragment_index);
	std::vector< numeric::xyzVector<numeric::Real> > fragCoordinates_rot(9);

	if ( !match_stub_alone ) {
		//full length fragment
		std::vector< numeric::xyzVector<numeric::Real> > coordinates;
		for ( core::Size ii = 0;  ii < 9; ++ii ) {
			coordinates.push_back(poseOP->residue(pose_residue+ii).xyz("CA"));
		}

		Eigen::Transform<numeric::Real, 3, Eigen::Affine> superposition_transform;

		QCPKernel::CoordMap frag_cmap(&fragCoordinates.front().x(), 3, fragCoordinates.size());
		QCPKernel::CoordMap frag_rot_cmap(&fragCoordinates_rot.front().x(), 3, fragCoordinates_rot.size());
		QCPKernel::CoordMap coor_cmap(&coordinates.front().x(), 3, coordinates.size());

		Real rmsd = QCPKernel::calc_coordinate_superposition(frag_cmap, coor_cmap, superposition_transform);
		TR.Debug << "full fragment rmsd" << rmsd << std::endl;

		frag_rot_cmap = superposition_transform * frag_cmap;
	} else { //------Prepare stub match-----------------------------------------------------
		//1. Get coordinates coordintes
		// coordinates
		// fragCoordinates
		// coordinates_stub
		// coordinates_removed_com_stub
		// fragCoordinate_removed_com_stub
		//2. Remove COM from coordinates_removed_com_stub,coordinates_removed_com_stub
		//3. Generate rotMatrix between coordinates_removed_com_stub, coordinates_removed_com_stub
		//4. Generate centers of mass for coordinates_stub call this coordinates_stub_com
		//5. Generate removed_com_vectors using the cooridinates_stub_com called coordinates_removed_stub_com
		//6. apply rotMatrix to fragCoordinates store in fragCoordinates_rot
		//7. Combine data fragCoordinates_rot[ii].x()= coordinates[ii].x()-coordinates_removed_com_stub[ii].x()+fragCoordinates_rot_tmp[ii].x()
		std::vector< numeric::xyzVector<numeric::Real> > coordinates;
		std::vector< numeric::xyzVector<numeric::Real> > coordinates_stub;
		std::vector< numeric::xyzVector<numeric::Real> > fragCoordinates_stub;
		core::Size res_ct = 0;

		for ( core::Size ii = 0;  ii < 9; ++ii ) {
			coordinates.push_back(poseOP->residue(pose_residue+ii).xyz("CA"));

			if ( ii<=1 || ii>1+loopLength_ ) {
				res_ct++;
				coordinates_stub.push_back(poseOP->residue(pose_residue+ii).xyz("CA"));
				fragCoordinates_stub.push_back(fragCoordinates[ii]);
			}
		}

		Eigen::Transform<numeric::Real, 3, Eigen::Affine> superposition_transform;
		Real starting_rmsd = QCPKernel::calc_coordinate_superposition(
			QCPKernel::CoordMap(&fragCoordinates_stub.front().x(), 3, res_ct),
			QCPKernel::CoordMap(&coordinates_stub.front().x(), 3, res_ct),
			superposition_transform
		);
		TR.Debug << "stub fragment rmsd" << starting_rmsd << std::endl;

		QCPKernel::CoordMap frag_cmap(&fragCoordinates.front().x(), 3, fragCoordinates.size());
		QCPKernel::CoordMap frag_rot_cmap(&fragCoordinates_rot.front().x(), 3, fragCoordinates.size());

		frag_rot_cmap = superposition_transform * frag_cmap;

		//output_fragment_debug(fragCoordinates_rot,"X3_fragCoords_rotate.pdb");
		//In test simulations rmsd_v3 was 8.02771 and rmsd_v4 was 8.27703. The only difference is the removing of the COM of the coordintes. But I don't see any errors
		//I'm ignoring this bug for now.  Maybe forever.
		//I think I found this bug. It was due to fragCoordinates_rot not being defined
	}

	ConstraintCOPs csts;
	core::id::AtomID const anchor_atom( core::id::AtomID( poseOP->residue(1).atom_index("CA"), 1) );
	//core::scoring::func::HarmonicFuncOP coord_cst_func = utility::pointer::make_shared< core::scoring::func::HarmonicFunc >( 0.1, 1.0 );
	core::scoring::func::HarmonicFuncOP coord_cst_func = utility::pointer::make_shared< core::scoring::func::HarmonicFunc >( 0.0, 0.2 );
	for ( core::Size ii = 0;  ii < 9; ++ii ) {
		core::Size atomindex =  poseOP->residue( pose_residue ).atom_index( "CA" );
		csts.push_back( utility::pointer::make_shared< CoordinateConstraint >(core::id::AtomID( atomindex, pose_residue+ii),anchor_atom,fragCoordinates_rot[ii],coord_cst_func));
	}
	poseOP->add_constraints( csts );
}

void PossibleLoop::add_dihedral_csts_from_lookback(core::Size stub_ss_index_match,core::Size fragment_index,core::Size pose_residue, core::pose::PoseOP & poseOP){
	using namespace core::scoring::func;
	using numeric::conversions::radians;
	using namespace core::scoring::constraints;
	using namespace core::id;
	core::Size fragment_length = SSHashedFragmentStore_->get_fragment_length();
	vector<Real> phi_v = SSHashedFragmentStore_->get_fragment_store(stub_ss_index_match)->realVector_groups["phi"][fragment_index];
	vector<Real> psi_v = SSHashedFragmentStore_->get_fragment_store(stub_ss_index_match)->realVector_groups["psi"][fragment_index];
	vector<Real> omega_v = SSHashedFragmentStore_->get_fragment_store(stub_ss_index_match)->realVector_groups["omega"][fragment_index];
	core::Real phi_sd_deg = 30; //40
	core::Real psi_sd_deg = 30; //40
	core::Real omega_sd_deg=10; //10
	core::Real phi_sd_rad = numeric::conversions::radians(phi_sd_deg);
	core::Real psi_sd_rad = numeric::conversions::radians(psi_sd_deg);
	core::Real omega_sd_rad = numeric::conversions::radians(omega_sd_deg);
	for ( core::Size ii=pose_residue; ii<pose_residue+fragment_length; ++ii ) {
		AtomID c_0(poseOP->residue(ii-1).atom_index( "C" ),ii-1);
		AtomID n_1(poseOP->residue( ii ).atom_index( "N" ),ii);
		AtomID ca_1( poseOP->residue( ii ).atom_index( "CA" ),ii);
		AtomID c_1( poseOP->residue(ii).atom_index( "C" ), ii );
		AtomID n_2( poseOP->residue(ii+1).atom_index( "N" ),ii+1);
		AtomID ca_2( poseOP->residue(ii+1).atom_index( "CA" ),ii+1);
		// std::cout << "phi" << ii << "phi_v[ii-resBeforeLoop_+1]" << phi_v[ii-resBeforeLoop_+1] << std::endl;
		// std::cout << "psi" << ii << "psi_v[ii-resBeforeLoop_+1]" << psi_v[ii-resBeforeLoop_+1] << std::endl;
		// std::cout << "omega" << ii << "omega_v[ii-resBeforeLoop_+1]" << omega_v[ii-resBeforeLoop_+1] << std::endl;
		Real phi_radians = radians(phi_v[ii-pose_residue]);
		//std::cout << "phi" << phi_v[ii-resBeforeLoop_+1]  <<"," << phi_radians << std::endl;
		CircularHarmonicFuncOP phi_func(utility::pointer::make_shared<CircularHarmonicFunc>( phi_radians, phi_sd_rad ) );
		ConstraintOP phi_cst( utility::pointer::make_shared<DihedralConstraint>(
			c_0,n_1,ca_1,c_1, phi_func ) );
		poseOP->add_constraint( scoring::constraints::ConstraintCOP( phi_cst ) );
		//psi-----
		Real psi_radians = radians(psi_v[ii-pose_residue]);
		//std::cout << "psi" << psi_v[ii-resBeforeLoop_+1]  <<"," << psi_radians << std::endl;
		CircularHarmonicFuncOP psi_func(utility::pointer::make_shared<CircularHarmonicFunc>( psi_radians, psi_sd_rad) );
		ConstraintOP psi_cst( utility::pointer::make_shared<DihedralConstraint>(n_1,ca_1,c_1,n_2, psi_func  ) );
		poseOP->add_constraint( scoring::constraints::ConstraintCOP( psi_cst ) );
		//omega-----
		Real omega_radians = radians(omega_v[ii-pose_residue]);
		//std::cout << "omega" << omega_v[ii-resBeforeLoop_+1]  <<"," << omega_radians << std::endl;
		CircularHarmonicFuncOP omega_func(utility::pointer::make_shared<CircularHarmonicFunc>( omega_radians, omega_sd_rad) );
		ConstraintOP omega_cst( utility::pointer::make_shared<DihedralConstraint>(ca_1,c_1,n_2,ca_2, omega_func ) );
		poseOP->add_constraint( scoring::constraints::ConstraintCOP( omega_cst ) );
	}

}


Size PossibleLoop::get_valid_resid(int resid,core::pose::Pose const pose){
	if ( resid<1 ) {
		TR << "invalid resid encountered n-term" << resid << std::endl;
		resid = 1;
	}
	if ( resid+9-1>(int)pose.size() ) {
		TR << "invalid resid encountered c-term" << resid << std::endl;
		resid = (int)pose.size()-9+1;
	}
	return(core::Size(resid));
}

std::string PossibleLoop::get_description(){
	std::stringstream extraTagInfoString;
	extraTagInfoString << "S" << resBeforeLoop_ << "E" << resAfterLoop_ << "Sa"<< resAdjustmentBeforeLoop_ << "Ea" << resAdjustmentAfterLoop_ << "LL" << loopLength_;
	return(extraTagInfoString.str());
}

std::vector< numeric::xyzVector<numeric::Real> > PossibleLoop::get_coordinates_from_pose(core::pose::PoseOP const poseOP,core::Size resid,core::Size length){
	std::vector< numeric::xyzVector<numeric::Real> > coordinates;
	for ( core::Size ii =resid;  ii < resid+length; ++ii ) {
		coordinates.push_back(poseOP->residue(ii).xyz("CA"));
	}
	return(coordinates);
}

// Real PossibleLoop::get_vdw_change(core::pose::PoseOP poseOP){
//  core::scoring::ScoreFunctionOP scorefxn( new core::scoring::ScoreFunction );
//  scorefxn->set_weight( core::scoring::vdw, 1.0 );
//  Real loop_vdw_score = scorefxn->score(*poseOP);
//  Real orig_vdw_score = scorefxn->score(*original_poseOP_);
//  return(loop_vdw_score-orig_vdw_score);
// }



Real PossibleLoop::rmsd_between_coordinates(std::vector< numeric::xyzVector<numeric::Real> > fragCoordinates,std::vector< numeric::xyzVector<numeric::Real> > coordinates){
	typedef numeric::alignment::QCPKernel< numeric::Real > QCPKernel;

	return QCPKernel::calc_coordinate_rmsd(
		QCPKernel::CoordMap(&fragCoordinates.front().x(), 3, fragCoordinates.size()),
		QCPKernel::CoordMap(&coordinates.front().x(), 3, coordinates.size()));
}

//closes the loop robustly. However, the phi/psi omega fall into the wrong abego types.
bool PossibleLoop::kic_closure(core::scoring::ScoreFunctionOP scorefxn,core::pose::PoseOP & poseOP,core::Size firstLoopRes, core::Size lastLoopRes,core::Size numb_kic_cycles){
	using namespace basic::options;
	using namespace basic::options::OptionKeys;
	using namespace chemical;
	using namespace protocols::generalized_kinematic_closure;
	GeneralizedKICOP genKIC(utility::pointer::make_shared<GeneralizedKIC>());
	kinematics::FoldTree ft;
	ft = poseOP->fold_tree();
	ft.clear();
	ft.add_edge(1,resBeforeLoop_+loopLength_,core::kinematics::Edge::PEPTIDE);
	ft.add_edge(1,poseOP->size(),1); //edges need to be added in order so the fold tree is completely connected.
	ft.add_edge(poseOP->size(),resAfterLoop_,core::kinematics::Edge::PEPTIDE);
	poseOP->fold_tree(ft);
	genKIC->add_filter( "loop_bump_check" );
	//hold first res to the correct ABEGO
	scorefxn->score(*poseOP);
	for ( core::Size ii=firstLoopRes; ii<=lastLoopRes; ii++ ) {
		genKIC->add_loop_residue( ii );
	}
	genKIC->add_perturber( "randomize_alpha_backbone_by_rama" );
	for ( core::Size ii=firstLoopRes; ii<=lastLoopRes; ii++ ) {
		genKIC->add_residue_to_perturber_residue_list( ii );
	}
	genKIC->add_perturber( "set_dihedral" );
	utility::vector1 < core::id::NamedAtomID > atomset;
	atomset.push_back( core::id::NamedAtomID( "C", lastLoopRes-1 ) );
	atomset.push_back( core::id::NamedAtomID( "N", lastLoopRes ) );
	genKIC->add_atomset_to_perturber_atomset_list( atomset );
	genKIC->add_value_to_perturber_value_list(180.0);
	core::Size midLoop = ((lastLoopRes-firstLoopRes)/2)+firstLoopRes;
	genKIC->set_pivot_atoms( firstLoopRes, "CA", midLoop, "CA" , lastLoopRes, "CA" );
	genKIC->set_closure_attempts(numb_kic_cycles);
	genKIC->set_selector_type("lowest_energy_selector");
	genKIC->set_selector_scorefunction(scorefxn);
	genKIC->close_bond(lastLoopRes-1,"C",lastLoopRes,"N",lastLoopRes-1,"CA",lastLoopRes,"CA",1.32829/*bondlength*/,116.2/*bondangle1*/,121.7/*bondagle2*/,180/*torsion*/,false,false);
	poseOP->conformation().reset_chain_endings();
	genKIC->apply(*poseOP);
	bool kicSuccess = genKIC->last_run_successful();
	if ( kicSuccess ) {
		ft = poseOP->fold_tree();
		ft.clear();
		ft.add_edge(1,poseOP->size(),core::kinematics::Edge::PEPTIDE);
		poseOP->fold_tree(ft);
		//poseOP->conformation().set_polymeric_connection(lastLoopRes-1,lastLoopRes);
		//poseOP->conformation().update_polymeric_connection(lastLoopRes-1,true);
		return true;
	}
	return false;
}


void PossibleLoop::minimize_loop(core::scoring::ScoreFunctionOP scorefxn,bool ideal_loop,core::pose::PoseOP & poseOP){
	using namespace core::optimization;
	kinematics::FoldTree ft;
	ft = poseOP->fold_tree();
	ft.clear();
	ft.add_edge(1,resBeforeLoop_+loopLength_,core::kinematics::Edge::PEPTIDE);
	ft.add_edge(1,poseOP->size(),1); //edges need to be added in order so the fold tree is completely connected.
	ft.add_edge(poseOP->size(),resAfterLoop_,core::kinematics::Edge::PEPTIDE);
	poseOP->fold_tree(ft);
	core::Size firstLoopRes=resBeforeLoop_;
	core::Size lastLoopRes=resAfterLoop_;
	core::kinematics::MoveMap mm;
	mm.set_bb  ( false );
	mm.set_chi ( false );
	mm.set_jump( false );
	mm.set_bb_true_range(firstLoopRes,lastLoopRes);
	if ( ideal_loop ) {
		MinimizerOptions min_options("lbfgs_armijo_nonmonotone", 0.01, true, false, false);
		AtomTreeMinimizer minimizer;
		minimizer.run( *poseOP, mm,*scorefxn, min_options );
		//std::cout << "after atom tree minimize" << std::endl;
		//scorefxn->show(std::cout,*poseOP);
	} else { //use cartesian minimization if it's ok.
		MinimizerOptions min_options("lbfgs_armijo_nonmonotone", 0.01, true, false, false);
		CartesianMinimizer minimizer;
		minimizer.run( *poseOP, mm,*scorefxn, min_options );
		//std::cout << "after atom tree minimize" << std::endl;
		//scorefxn->show(std::cout,*poseOP);
	}
	ft = poseOP->fold_tree();
	ft.clear();
	ft.add_edge(1,poseOP->size(),core::kinematics::Edge::PEPTIDE);
	poseOP->fold_tree(ft);
}


NearNativeLoopCloser::NearNativeLoopCloser():moves::Mover("NearNativeLoopCloser"){
}






NearNativeLoopCloser::NearNativeLoopCloser(int resAdjustmentStartLow,int resAdjustmentStartHigh,int resAdjustmentStopLow,int resAdjustmentStopHigh,int resAdjustmentStartLow_sheet,int resAdjustmentStartHigh_sheet,int resAdjustmentStopLow_sheet,int resAdjustmentStopHigh_sheet,core::Size loopLengthRangeLow, core::Size loopLengthRangeHigh,core::Size resBeforeLoop,core::Size resAfterLoop,char chainBeforeLoop, char chainAfterLoop,Real rmsThreshold, Real max_vdw_change, bool idealExtension,bool ideal, bool output_closed,std::string closure_type,std::string allowed_loop_abegos){
	resAdjustmentStartLow_=resAdjustmentStartLow;
	resAdjustmentStartHigh_=resAdjustmentStartHigh;
	resAdjustmentStopLow_=resAdjustmentStopLow;
	resAdjustmentStopHigh_=resAdjustmentStopHigh;
	resAdjustmentStartLow_sheet_=resAdjustmentStartLow_sheet;
	resAdjustmentStartHigh_sheet_=resAdjustmentStartHigh_sheet;
	resAdjustmentStopLow_sheet_=resAdjustmentStopLow_sheet;
	resAdjustmentStopHigh_sheet_=resAdjustmentStopHigh_sheet;
	loopLengthRangeLow_=loopLengthRangeLow;
	loopLengthRangeHigh_=loopLengthRangeHigh;
	resBeforeLoop_=resBeforeLoop;
	resAfterLoop_=resAfterLoop;
	chainBeforeLoop_=chainBeforeLoop;
	chainAfterLoop_=chainAfterLoop;
	rmsThreshold_=rmsThreshold;
	idealExtension_=idealExtension;
	output_closed_ = output_closed;
	ideal_=ideal;
	output_all_=false;
	top_outputed_ = false;
	max_vdw_change_ = max_vdw_change;
	closure_type_ = closure_type;
	allowed_loop_abegos_ = allowed_loop_abegos;
	TR << "native loop closer init"<< resAdjustmentStartLow_ << "," << resAdjustmentStartHigh_ <<"," << resAdjustmentStopLow_ <<"," << resAdjustmentStopHigh_ << "," << loopLengthRangeLow_ << "," << loopLengthRangeHigh_ << std::endl;
	SSHashedFragmentStore_ = protocols::indexed_structure_store::SSHashedFragmentStore::get_instance();
	SSHashedFragmentStore_->set_threshold_distance(rmsThreshold_);
	SSHashedFragmentStore_->init_SS_stub_HashedFragmentStore();
}


Real NearNativeLoopCloser::close_loop(core::pose::Pose & pose) {
	//time_t start_time = time(NULL);
	//get name of pose for multi-pose mover-------------------
	pose_name_ = "No_Name_Found";
	if ( pose.pdb_info() && ( pose.pdb_info()->name() != "" ) ) {
		pose_name_ = pose.pdb_info()->name();
	} else if ( pose.data().has( core::pose::datacache::CacheableDataType::JOBDIST_OUTPUT_TAG ) ) {
		pose_name_ = static_cast< basic::datacache::CacheableString const & >( pose.data().get( core::pose::datacache::CacheableDataType::JOBDIST_OUTPUT_TAG ) ).str();
	} else {
		pose_name_ = protocols::jd2::current_input_tag();
	}
	//do actual closer
	//-------deal with multiple chains-for easier indexing-----------
	if ( chainBeforeLoop_!=chainAfterLoop_ ) { //connecting chains
		combine_chains(pose);
	} else { //internal loop
		vector1<int> chains = get_chains(pose);
		core::Size chain_id;
		if ( chains.size()==1 ) { //chain id need not be specified.
			chain_id = chains[1];
		} else {
			chain_id =  get_chain_id_from_chain(chainBeforeLoop_,pose);
		}
		if ( chain_id != 1 ) { //renumber chains so chain being modified is in front for ease in numbering
			utility::vector1<core::Size> orig_chain_order = get_chains(pose);
			std::stringstream int_to_string;
			int_to_string << chain_id;
			for ( core::Size ii=1; ii<=orig_chain_order.size(); ++ii ) {
				if ( orig_chain_order[ii]!=chain_id ) {
					int_to_string << orig_chain_order[ii];
				}
			}
			std::string new_chain_order=int_to_string.str();
			core::scoring::ScoreFunctionOP scorefxn = core::scoring::ScoreFunctionFactory::create_score_function("score3");
			if ( pose.is_fullatom() ) {
				scorefxn = core::scoring::ScoreFunctionFactory::create_score_function(core::scoring::PRE_TALARIS_2013_STANDARD_WTS);

			}
			simple_moves::SwitchChainOrderMoverOP switch_chains(utility::pointer::make_shared<simple_moves::SwitchChainOrderMover>());
			switch_chains->scorefxn(scorefxn);
			switch_chains->chain_order(new_chain_order);
			switch_chains->apply(pose);
		}
	}
	//-------make all possible loops-----------
	possibleLoops_ = create_potential_loops(pose);
	if ( !output_closed_ ) {
		for ( core::Size ii=1; ii<=possibleLoops_.size(); ii++ ) {
			possibleLoops_[ii]->generate_output_pose(false,ideal_,rmsThreshold_,allowed_loop_abegos_,closure_type_);
		}
	} else {
		if ( closure_type_ == "kic" ) {
			for ( core::Size ii=1; ii<=possibleLoops_.size(); ii++ ) {
				possibleLoops_[ii]->generate_output_pose(true,ideal_,rmsThreshold_,allowed_loop_abegos_,closure_type_);
			}
		}
		if ( closure_type_ == "lookback" ) {
			//-------check CA-CA distance viability of loop-----------
			for ( core::Size ii=1; ii<=possibleLoops_.size(); ii++ ) {
				possibleLoops_[ii]->evaluate_distance_closure();
				if ( possibleLoops_[ii]->get_below_distance_threshold() && output_closed_ ) {
					possibleLoops_[ii]->generate_stub_rmsd();
					if ( possibleLoops_[ii]->get_stubRMSD()<rmsThreshold_+ 0.10 ) {
						possibleLoops_[ii]->generate_uncached_stub_rmsd();
					}
				}
			}
			//-------sort loops by stub rmsd-----------
			std::sort(possibleLoops_.begin(), possibleLoops_.end(), StubRMSDComparator());
			//-------get final output------------
			for ( core::Size ii=1; ii<=possibleLoops_.size(); ++ii ) {
				if ( possibleLoops_[ii]->get_uncached_stubRMSD() <rmsThreshold_+0.20 ) {
					possibleLoops_[ii]->generate_output_pose(true,ideal_,rmsThreshold_,allowed_loop_abegos_,closure_type_);
				}
			}
			//debug code--------------------
			// for ( core::Size ii=1; ii<possibleLoops_.size(); ++ii ) {
			//  //std::cout <<"rmsds" << ii << "," <<  possibleLoops_[ii]->get_stubRMSD() << "," << possibleLoops_[ii]->get_uncached_stubRMSD() <<  "," << possibleLoops_[ii]->get_final_RMSD() << std::endl;
			//  //std::cout << possibleLoops_[ii]->get_description();
			//  if( possibleLoops_[ii]->get_uncached_stubRMSD() < 0.4){
			//   std::stringstream numbConvert;
			//   numbConvert << "bust_" << ii << ".pdb";
			//   std::string tmp_pdb_name= numbConvert.str();
			//   possibleLoops_[ii]->get_finalPoseOP()->dump_pdb(tmp_pdb_name);
			//  }
			// }
			//
		}
	}
	Real return_rmsd;
	core::pose::PoseOP tmpPoseOP=get_additional_output_with_rmsd(return_rmsd);
	if ( tmpPoseOP!=nullptr ) {
		pose=*tmpPoseOP;
	}
	return(return_rmsd);
	//time_t end_time = time(NULL);
	//std::cout << "total_time" << end_time-start_time << std::endl;
}

void NearNativeLoopCloser::apply(core::pose::Pose & pose) {
	close_loop(pose);
}

void NearNativeLoopCloser::combine_chains(core::pose::Pose & pose){
	if ( !(has_chain(chainBeforeLoop_,pose) && has_chain(chainAfterLoop_,pose)) ) {
		utility_exit_with_message("chains not found in pdb");
	}
	//extract chains
	core::Size chain1_id =  get_chain_id_from_chain(chainBeforeLoop_,pose);
	core::pose::PoseOP chain1 = pose.split_by_chain(chain1_id);
	core::Size chain2_id =  get_chain_id_from_chain(chainAfterLoop_,pose);
	core::pose::PoseOP chain2 = pose.split_by_chain(chain2_id);
	//assign_residue_number
	resBeforeLoop_=chain1->size();
	resAfterLoop_=chain1->size()+1;
	//Remove terminal residues & attach
	remove_upper_terminus_type_from_pose_residue(*chain1,chain1->size());
	remove_lower_terminus_type_from_pose_residue(*chain2,1);
	for ( core::Size ii=1; ii<=chain2->size(); ++ii ) {
		chain1->append_residue_by_bond(chain2->residue(ii),false,0,chain1->size());
	}
	//append chain to pose
	append_pose_to_pose(pose,*chain1,true);
	//reorder chains with the last one being first & the original chains removed.
	utility::vector1<core::Size> orig_chain_order = get_chains(pose);
	core::Size new_chain1 = orig_chain_order[orig_chain_order.size()];
	std::stringstream int_to_string;
	int_to_string << new_chain1;
	for ( core::Size ii=1; ii<orig_chain_order.size(); ++ii ) {//last elment is new pose
		if ( orig_chain_order[ii]!=chain1_id && orig_chain_order[ii]!=chain2_id ) {
			int_to_string << orig_chain_order[ii];
		}
	}
	std::string new_chain_order=int_to_string.str();
	simple_moves::SwitchChainOrderMoverOP switch_chains(utility::pointer::make_shared<simple_moves::SwitchChainOrderMover>());
	core::scoring::ScoreFunctionOP scorefxn = core::scoring::ScoreFunctionFactory::create_score_function("score3");
	if ( pose.is_fullatom() ) {
		scorefxn = core::scoring::ScoreFunctionFactory::create_score_function(core::scoring::PRE_TALARIS_2013_STANDARD_WTS);
	}
	switch_chains->scorefxn(scorefxn);
	switch_chains->chain_order(new_chain_order);
	switch_chains->apply(pose);
}

void NearNativeLoopCloser::extendRegion(bool towardCTerm, core::Size resStart, char neighborResType, core::Size numberAddRes,core::pose::PoseOP & poseOP){
	using namespace basic::options;
	using namespace basic::options::OptionKeys;
	using namespace chemical;
	Real tmpPhi =  poseOP->phi(resStart);
	Real tmpPsi =  poseOP->psi(resStart);
	Real tmpOmega = poseOP->omega(resStart);
	if ( idealExtension_ ) {
		if ( neighborResType == 'E' ) {
			tmpPhi = -135;
			tmpPsi = 135;
			tmpOmega = 180;
		}
		if ( neighborResType == 'H' ) {
			tmpPhi = -57.8;
			tmpPsi = -47.0;
			tmpOmega = 180.0;
		}
	}
	core::conformation::ResidueOP new_rsd( nullptr );
	string build_aa_type_one_letter =option[OptionKeys::remodel::generic_aa];
	string build_aa_type = name_from_aa(aa_from_oneletter_code(build_aa_type_one_letter[0]));
	debug_assert( poseOP != nullptr );
	core::chemical::ResidueTypeSetCOP rs( poseOP->residue_type_set_for_pose() );
	kinematics::FoldTree ft;
	if ( towardCTerm == true ) {
		ft = poseOP->fold_tree();
		ft.clear();
		ft.add_edge(1,resStart,core::kinematics::Edge::PEPTIDE);
		ft.add_edge(1,poseOP->size(),1); //edges need to be added in order so the fold tree is completely connected.
		ft.add_edge(poseOP->size(),resStart+1,core::kinematics::Edge::PEPTIDE);
		poseOP->fold_tree(ft);
		for ( core::Size ii=0; ii<numberAddRes; ++ii ) {
			new_rsd = core::conformation::ResidueFactory::create_residue( rs->name_map(build_aa_type) );
			poseOP->conformation().safely_append_polymer_residue_after_seqpos( *new_rsd,resStart+ii, true);
		}
	} else {
		ft = poseOP->fold_tree();
		ft.clear();
		ft.add_edge(1,resStart-1,core::kinematics::Edge::PEPTIDE);
		ft.add_edge(1,poseOP->size(),1); //edges need to be added in order so the fold tree is completely connected.
		ft.add_edge(poseOP->size(),resStart,core::kinematics::Edge::PEPTIDE);
		poseOP->fold_tree(ft);
		for ( core::Size ii=0; ii<numberAddRes; ++ii ) {
			new_rsd = core::conformation::ResidueFactory::create_residue( rs->name_map(build_aa_type) );
			poseOP->conformation().safely_prepend_polymer_residue_before_seqpos( *new_rsd,resStart, true);
		}
	}
	for ( core::Size ii=0; ii<=numberAddRes; ++ii ) {
		poseOP->set_phi(resStart+ii, tmpPhi );
		poseOP->set_psi(resStart+ii, tmpPsi );
		poseOP->set_omega(resStart+ii, tmpOmega );
	}
	ft = poseOP->fold_tree();
	ft.clear();
	ft.add_edge(1,poseOP->size(),core::kinematics::Edge::PEPTIDE);
	poseOP->fold_tree(ft);
	renumber_pdbinfo_based_on_conf_chains(*poseOP,true,false,false,false);
}

core::pose::PoseOP NearNativeLoopCloser::create_maximum_length_pose(char resTypeBeforeLoop, char resTypeAfterLoop, core::pose::Pose const pose){
	core::pose::PoseOP full_length_poseOP = pose.clone();
	if ( resBeforeLoop_+1 != resAfterLoop_ ) {
		//trim region:
		kinematics::FoldTree ft;
		ft = full_length_poseOP->fold_tree();
		ft.add_edge(1,resAfterLoop_-1,core::kinematics::Edge::PEPTIDE);
		ft.add_edge(1,full_length_poseOP->size(),1); //edges need to be added in order so the fold tree is completely connected.
		ft.add_edge(full_length_poseOP->size(),resAfterLoop_,core::kinematics::Edge::PEPTIDE);
		full_length_poseOP->fold_tree(ft);
		full_length_poseOP->conformation().delete_residue_range_slow(resBeforeLoop_+1,resAfterLoop_-1);
		//repair fold tree
		ft = full_length_poseOP->fold_tree();
		ft.clear();
		ft.add_edge(1,full_length_poseOP->total_residue(),core::kinematics::Edge::PEPTIDE);
		full_length_poseOP->fold_tree(ft);
		renumber_pdbinfo_based_on_conf_chains(*full_length_poseOP,true,false,false,false);
		resAfterLoop_ = resBeforeLoop_+1;
	}
	if ( resAdjustmentStopHigh_>0 ) {
		extendRegion(false,resAfterLoop_,resTypeAfterLoop,resAdjustmentStopHigh_,full_length_poseOP);
	}
	if ( resAdjustmentStartHigh_>0 ) {
		extendRegion(true,resBeforeLoop_,resTypeBeforeLoop,resAdjustmentStartHigh_,full_length_poseOP);
	}
	return(full_length_poseOP);
}

vector1<PossibleLoopOP> NearNativeLoopCloser::create_potential_loops(core::pose::Pose const pose){
	//create vector1 of possible loops
	vector1<PossibleLoopOP> possibleLoops;
	core::scoring::dssp::Dssp pose_dssp( pose );
	pose_dssp.dssp_reduced();
	std::string tmp_dssp = pose_dssp.get_dssp_secstruct();
	core::Size tmpResBeforeLoop = resBeforeLoop_;
	char resTypeBeforeLoop = tmp_dssp[tmpResBeforeLoop];
	//if there is a cut sometimes DSSP assigns the wrong SS
	while ( resTypeBeforeLoop == 'L' ) {
		tmpResBeforeLoop--;
		resTypeBeforeLoop = tmp_dssp[tmpResBeforeLoop];
	}
	core::Size tmpResAfterLoop = resAfterLoop_;
	char resTypeAfterLoop = tmp_dssp[tmpResAfterLoop];
	//if there is a cut sometimes DSSP assigns the wrong SS
	while ( resTypeAfterLoop == 'L' ) {
		tmpResAfterLoop--;
		resTypeAfterLoop = tmp_dssp[tmpResBeforeLoop];
	}
	if ( resTypeBeforeLoop == 'E' && resTypeAfterLoop == 'E' ) {
		//extend/trim equally
		int low_start = resAdjustmentStartLow_sheet_;
		int high_start = resAdjustmentStartHigh_sheet_;
		if ( low_start<resAdjustmentStopLow_sheet_ ) {
			low_start = resAdjustmentStopLow_sheet_;
		}
		if ( high_start>resAdjustmentStopHigh_sheet_ ) {
			high_start = resAdjustmentStopHigh_sheet_;
		}
		resAdjustmentStartHigh_=high_start; //So they are extended an equal length. This value applies to both sheet and helices. Kinda confusing I know.
		resAdjustmentStopHigh_=high_start;
		core::pose::PoseOP max_length_poseOP = create_maximum_length_pose(resTypeBeforeLoop,resTypeAfterLoop,pose);
		core::pose::PoseOP orig_atom_type_max_length_poseOP = max_length_poseOP->clone();
		if ( max_length_poseOP->is_fullatom() ) {
			core::util::switch_to_residue_type_set(*max_length_poseOP, core::chemical::CENTROID);
		}
		for ( int ii=low_start; ii<=high_start; ++ii ) {
			for ( core::Size kk=loopLengthRangeLow_; kk<=loopLengthRangeHigh_; ++kk ) {
				if ( (ii+resBeforeLoop_>=3)&&(ii+resAfterLoop_<=max_length_poseOP->total_residue()-3) ) { //ensures at least a 3 residue SS element next to loop
					PossibleLoopOP tmpLoopOP=utility::pointer::make_shared< PossibleLoop >(ii,ii,kk,resBeforeLoop_,resAfterLoop_,resTypeBeforeLoop,resTypeAfterLoop,resAdjustmentStartHigh_,resAdjustmentStopHigh_,max_length_poseOP,orig_atom_type_max_length_poseOP);
					possibleLoops.push_back(tmpLoopOP);
				}
			}
		}
	} else {
		if ( resTypeBeforeLoop=='E' ) {
			TR.Warning << "Not extending 1 side of sheet and only eating in 1 residue" <<std::endl;
			resAdjustmentStartHigh_=0;
			resAdjustmentStartLow_=resAdjustmentStopLow_sheet_;
		}
		if ( resTypeAfterLoop=='E' ) {
			TR.Warning << "Not extending 1 side of sheet and only eating in 1 residue" << std::endl;
			resAdjustmentStopHigh_=0;
			resAdjustmentStopLow_=resAdjustmentStopHigh_sheet_;
		}
		core::pose::PoseOP max_length_poseOP = create_maximum_length_pose(resTypeBeforeLoop,resTypeAfterLoop,pose);
		core::pose::PoseOP orig_atom_type_max_length_poseOP = max_length_poseOP->clone();
		if ( max_length_poseOP->is_fullatom() ) {
			core::util::switch_to_residue_type_set(*max_length_poseOP, core::chemical::CENTROID);
		}
		for ( int ii=resAdjustmentStartLow_; ii<=resAdjustmentStartHigh_; ++ii ) {
			for ( int jj=resAdjustmentStopLow_; jj<=resAdjustmentStopHigh_; ++jj ) {
				for ( core::Size kk=loopLengthRangeLow_; kk<=loopLengthRangeHigh_; ++kk ) {
					if ( (ii+resBeforeLoop_>=3)&&(jj+resAfterLoop_<=max_length_poseOP->total_residue()-3) ) {
						PossibleLoopOP tmpLoopOP=utility::pointer::make_shared< PossibleLoop >(ii,jj,kk,resBeforeLoop_,resAfterLoop_,resTypeBeforeLoop,resTypeAfterLoop,resAdjustmentStartHigh_,resAdjustmentStopHigh_,max_length_poseOP,orig_atom_type_max_length_poseOP);
						possibleLoops.push_back(tmpLoopOP);
					}
				}
			}
		}
	}
	return(possibleLoops);
}

void
NearNativeLoopCloser::parse_my_tag(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap &
){
	std::string loopLengthRange( tag->getOption< std::string >( "loopLengthRange", "1,5") );
	rmsThreshold_ = tag->getOption< core::Real >( "RMSthreshold", 0.4 );
	if ( rmsThreshold_>0.6 ) {
		TR << "********************** rmsThresholds > 0.6 sometimes produces unclosed loops ***********************";
	}
	std::string resAdjustmentRange1( tag->getOption< std::string >( "resAdjustmentRangeSide1", "-3,3") );
	std::string resAdjustmentRange2( tag->getOption< std::string >( "resAdjustmentRangeSide2","-3,3") );
	chainBeforeLoop_ = tag->getOption<char>("chain",'A');
	chainAfterLoop_ = tag->getOption<char>("chain",'A');
	chainBeforeLoop_ = tag->getOption<char>("chainBeforeLoop",'A');
	chainAfterLoop_ = tag->getOption<char>("chainAfterLoop",'A');
	idealExtension_ = tag->getOption<bool>("idealExtension",true);
	allowed_loop_abegos_ = tag->getOption< std::string >( "allowed_loop_abegos","");
	if ( !idealExtension_ ) {
		utility_exit_with_message("the ideal extension flag works but it seems to mess up the pose more than help so delete this line & recompile if you really want to use it");
	}
	max_vdw_change_ = tag->getOption<core::Real>("max_vdw_change",8.0);
	ideal_ = tag->getOption<bool>("ideal",false);
	if ( chainBeforeLoop_==chainAfterLoop_ ) {
		resBeforeLoop_ = tag->getOption<core::Size>("resBeforeLoop");
		resAfterLoop_ = tag->getOption<core::Size>("resAfterLoop");
	}
	output_closed_ = tag->getOption<bool>("close",true);
	closure_type_ = tag->getOption<std::string>("closure_type","lookback");
	if ( output_closed_ ) {
		SSHashedFragmentStore_ = protocols::indexed_structure_store::SSHashedFragmentStore::get_instance();
		SSHashedFragmentStore_->set_threshold_distance(rmsThreshold_);
		SSHashedFragmentStore_->init_SS_stub_HashedFragmentStore();
		TR << "database loaded!!" << std::endl;
	}
	output_all_= tag->getOption<bool>("output_all",false);
	top_outputed_ = false;
	utility::vector1< std::string > resAdjustmentRange1_split( utility::string_split( resAdjustmentRange1 , ',' ) );
	utility::vector1< std::string > resAdjustmentRange2_split( utility::string_split( resAdjustmentRange2 , ',' ) );
	utility::vector1< std::string > loopLengthRange_split( utility::string_split( loopLengthRange , ',' ) );
	if ( resAdjustmentRange1_split.size()==2 ) {
		resAdjustmentStartLow_ = atoi(resAdjustmentRange1_split[1].c_str());
		resAdjustmentStartHigh_ = atoi(resAdjustmentRange1_split[2].c_str());
	}
	if ( resAdjustmentRange1_split.size()==1 ) {
		resAdjustmentStartLow_= atoi(resAdjustmentRange1_split[1].c_str());
		resAdjustmentStartHigh_= atoi(resAdjustmentRange1_split[1].c_str());
	}
	if ( resAdjustmentRange2_split.size()==2 ) {
		resAdjustmentStopLow_ = atoi(resAdjustmentRange2_split[1].c_str());
		resAdjustmentStopHigh_ = atoi(resAdjustmentRange2_split[2].c_str());
	}
	if ( resAdjustmentRange2_split.size()==1 ) {
		resAdjustmentStopLow_ = atoi(resAdjustmentRange2_split[1].c_str());
		resAdjustmentStartHigh_ = atoi(resAdjustmentRange2_split[1].c_str());
	}
	if ( loopLengthRange_split.size()==2 ) {
		loopLengthRangeLow_ = atoi(loopLengthRange_split[1].c_str());
		loopLengthRangeHigh_ = atoi(loopLengthRange_split[2].c_str());
	}
	if ( loopLengthRange_split.size()==1 ) {
		loopLengthRangeLow_ = atoi(loopLengthRange_split[1].c_str());
		loopLengthRangeHigh_ = atoi(loopLengthRange_split[1].c_str());
	}
	resAdjustmentStartLow_sheet_ = resAdjustmentStartLow_;
	resAdjustmentStartHigh_sheet_ = resAdjustmentStartHigh_;
	resAdjustmentStopLow_sheet_ = resAdjustmentStopLow_;
	resAdjustmentStopHigh_sheet_ = resAdjustmentStopHigh_;
	//TR << resAdjustmentStartLow_ <<"," << resAdjustmentStartHigh_ << ",:," << resAdjustmentStopLow_ << "," << resAdjustmentStopHigh_ << ",:," << loopLengthRangeLow_ <<"," << loopLengthRangeHigh_ << std::endl;
}

core::pose::PoseOP NearNativeLoopCloser::get_additional_output(){
	Real tmp_rmsd;
	return(get_additional_output_with_rmsd(tmp_rmsd));
}

core::pose::PoseOP NearNativeLoopCloser::get_additional_output_with_rmsd(Real & return_rmsd){
	std::sort(possibleLoops_.begin(), possibleLoops_.end(), FinalRMSDComparator());
	if ( !output_all_&&top_outputed_ ) {
		set_last_move_status(protocols::moves::FAIL_DO_NOT_RETRY);
		return nullptr;
	}
	for ( core::Size ii=1; ii<=possibleLoops_.size(); ++ii ) {
		//std::cout << ii << "all final rmsd" << possibleLoops_[ii]->get_final_RMSD() << std::endl;
		//std::cout << possibleLoops_[ii]->get_description() << std::endl;
		if ( !possibleLoops_[ii]->outputed()&&(possibleLoops_[ii]->get_final_RMSD()<rmsThreshold_) ) {
			TR << "Loop outputed with " << possibleLoops_[ii]->get_final_RMSD() << " rmsd" << std::endl;
			if ( allowed_loop_abegos_!="" ) {
				utility::vector1< std::string > const abego_v = core::sequence::get_abego( *possibleLoops_[ii]->get_finalPoseOP() );
				core::sequence::ABEGOManager am;
				std::string abego = am.get_abego_string(abego_v);
				TR << "ABEGO of pose" << abego << std::endl;
			}
			possibleLoops_[ii]->outputed(true);
			top_outputed_=true;
			set_last_move_status(protocols::moves::MS_SUCCESS);
			possibleLoops_[ii]->get_finalPoseOP()->data().set(
				core::pose::datacache::CacheableDataType::JOBDIST_OUTPUT_TAG,utility::pointer::make_shared< basic::datacache::CacheableString >( pose_name_ ) );
			return_rmsd = possibleLoops_[ii]->get_final_RMSD();
			return(possibleLoops_[ii]->get_finalPoseOP());
		}
		if ( !output_closed_ && (!possibleLoops_[ii]->outputed()) ) {
			possibleLoops_[ii]->outputed(true);
			set_last_move_status(protocols::moves::MS_SUCCESS);
			return_rmsd = possibleLoops_[ii]->get_final_RMSD();
			return(possibleLoops_[ii]->get_finalPoseOP());
		}
	}
	// Real low_rmsd=999;
	// for ( core::Size ii=1; ii<possibleLoops_.size(); ++ii ) {
	//  if ( !possibleLoops_[ii]->outputed() ) {
	//   if ( possibleLoops_[ii]->get_final_RMSD()< low_rmsd ) {
	//    low_rmsd = possibleLoops_[ii]->get_final_RMSD();
	//   }
	//  }
	// }
	set_last_move_status(protocols::moves::FAIL_DO_NOT_RETRY);
	return_rmsd = 9999;
	return nullptr;
}

std::string NearNativeLoopCloser::get_name() const {
	return mover_name();
}

std::string NearNativeLoopCloser::mover_name() {
	return "NearNativeLoopCloser";
}

void NearNativeLoopCloser::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute::attribute_w_default(
		"loopLengthRange", xs_string,
		"XSD_XRW: TO DO", "1,5");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"RMSthreshold", xsct_real,
		"XSD_XRW: TO DO", "0.4");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"resAdjustmentRangeSide1", xs_string,
		"XSD_XRW: TO DO", "-3,3");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"resAdjustmentRangeSide2", xs_string,
		"XSD_XRW: TO DO", "-3,3");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"chain", xs_string,
		"XSD_XRW: TO DO", "A");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"chainBeforeLoop", xs_string,
		"XSD_XRW: TO DO", "A");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"chainAfterLoop", xs_string,
		"XSD_XRW: TO DO", "A");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"idealExtension", xsct_rosetta_bool,
		"XSD_XRW: TO DO", "true");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"max_vdw_change", xsct_real,
		"XSD_XRW: TO DO", "8.0");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"ideal", xsct_rosetta_bool,
		"XSD_XRW: TO DO", "false");
	attlist + XMLSchemaAttribute(
		"resBeforeLoop", xsct_non_negative_integer,
		"XSD_XRW: TO DO");
	attlist + XMLSchemaAttribute(
		"resAfterLoop", xsct_non_negative_integer,
		"XSD_XRW: TO DO");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"output_all", xsct_rosetta_bool,
		"XSD_XRW: TO DO", "false");
	attlist + XMLSchemaAttribute::attribute_w_default(
		"closure_type", xs_string,
		"type of closure, either kic or lookback", "lookback");
	attlist + XMLSchemaAttribute::attribute_w_default( "allowed_loop_abegos", xs_string, "comma seperated string of allowed abegos, default=empty all abegos", "" );
	protocols::moves::xsd_type_definition_w_attributes(
		xsd, mover_name(),
		"XSD_XRW: TO DO",
		attlist );
}

std::string NearNativeLoopCloserCreator::keyname() const {
	return NearNativeLoopCloser::mover_name();
}

protocols::moves::MoverOP
NearNativeLoopCloserCreator::create_mover() const {
	return utility::pointer::make_shared< NearNativeLoopCloser >();
}

void NearNativeLoopCloserCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	NearNativeLoopCloser::provide_xml_schema( xsd );
}


}//pose_length_moves
}//protocols