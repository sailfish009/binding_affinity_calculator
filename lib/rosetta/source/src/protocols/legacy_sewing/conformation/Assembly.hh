// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/legacy_sewing/conformation/Assembly.hh
///
/// @brief A set of simple container classes used by the SewingProtocol. These classes are used during the Hashing of coordinates, as well
/// as during assembly of low-resolution assemblies.
///
/// @author Tim Jacobs

#ifndef INCLUDED_protocols_legacy_sewing_conformation_Assembly_hh
#define INCLUDED_protocols_legacy_sewing_conformation_Assembly_hh

//Package headers
#include <protocols/legacy_sewing/sampling/SewGraph.hh>
#include <protocols/legacy_sewing/util/io.hh>

//Unit headers
#include <protocols/legacy_sewing/conformation/Assembly.fwd.hh>

//Protocol headers
#include <protocols/legacy_sewing/conformation/Model.hh>
#include <core/pack/task/TaskFactory.fwd.hh>
#include <protocols/loops/Loop.fwd.hh>

//Utility headers
#include <utility/pointer/owning_ptr.functions.hh>
#include <utility/VirtualBase.hh>
#include <utility/vector1.hh>

//Core headers
#include <utility/graph/Graph.hh>
#include <core/pose/Pose.hh>

//C++ headers
#include <set>
#include <map>

//External headers
#include <boost/tuple/tuple.hpp>

namespace protocols {
namespace legacy_sewing  {

///@brief An Assembly is a collection of SewSegments. Assemblies are created using the
///geometric "compatibility" data generated by the Hasher.
class Assembly : public utility::VirtualBase {

public:

	Assembly();

	virtual
	AssemblyOP
	clone() = 0;

	///@brief Add a model to the Assembly. Only used for first node
	///and special cases.
	void
	add_model(
		SewGraphCOP graph,
		Model const & model,
		bool available = true
	);

	void
	update_coords_from_pose(
		core::pose::Pose const & pose
	);

	std::map<SewSegment,SewSegment>
	get_matching_model_segments(
		Model const & model,
		ScoreResult const & edge_score
	) const;

	ScoreResult
	get_score_result(
		ModelNode const * model_1_node,
		HashEdge const * const cur_edge,
		SewGraphCOP graph
	) const;

	ModelNode const *
	starting_node() const;

	ModelNode const *
	ending_node() const;

	utility::vector1< boost::tuple<ModelNode const *, ModelNode const *, HashEdge const *> >
	edges() const;

	///TODO remove logic for picking nodes and just return available nodes!
	///we are convoluting sampling and the conformation itself by not doing that

	///@brief Pick which model to use as the reference model for the next
	///node additon
	core::Size
	get_next_reference_node(
		SewGraphOP graph
	) const;

	utility::vector1<SewSegment>
	get_chimera_segments(
		std::map<SewSegment, SewSegment> const & matching_segments,
		std::map<SegmentPair, AtomMap> const & segment_matches,
		Model const & mobile_model
	);

	SewSegment
	create_chimera_segment(
		SewSegment const & reference_segment,
		SewSegment const & mobile_segment,
		AtomMap const & atom_map,
		bool reference_is_nter
	) const;

	utility::vector1<SewSegment>
	segments() const;

	utility::vector1< utility::vector1<SewSegment> >
	all_segments() const;

	utility::vector1<SewSegment>
	get_model_segments(
		int model_id
	) const;

	std::set<core::Size>
	model_ids() const;

	Model
	regenerate_model(
		int model_id
	) const;

	///@brief append the given model to the end of the assembly
	virtual void
	append_model(
		Model const & model,
		ScoreResult const & edge_score
	)=0 ;

	void
	add_loop_segment(
		core::pose::Pose const & pose,
		protocols::loops::Loop loop,
		core::Size segment
	);

	AtomMap
	atom_map_from_score_result(
		ScoreResult const & alignment_scores
	) const;

	void
	follow_edge(
		SewGraphCOP graph,
		HashEdge const * const edge,
		core::Size source_index
	);

	void
	align_model(
		Model & mobile_model,
		AtomMap const & atom_alignments,
		int reference_model_id
	) const;

	///@brief create mappings between SewResidues in the Assembly
	///that map to the same pose number
	void
	map_residues(
		int reference_model_id,
		Model mobile_model,
		AtomMap const & atom_alignments
	);

	NativeRotamersMap
	generate_native_rotamers_map()
	const;

	///@brief Update the task factory such that
	///the 'native' rotamers are added to the rotamer
	///set, and the native residues are favored
	void
	prepare_for_packing(
		core::pose::Pose & pose,
		core::pack::task::TaskFactoryOP task_factory,
		core::Real base_native_bonus,
		core::Size neighbor_cutoff
	) const;

	core::pose::Pose
	to_pose(
		std::string residue_type_set,
		bool create_cuts = true
	) const;

	core::pose::Pose
	to_multichain_pose (
		std::string residue_type_set
	) const;

	///@brief reorder segments in a random fashion that satisfies the maximum loop distance cutoff
	///If this is not possible then return false, otherwise true
	bool
	reorder_randomly(
		core::Real max_loop_distance
	);

	void
	reorder(
		utility::vector1<core::Size> new_order
	);

	utility::vector1< utility::vector1<core::Size> >
	find_possible_orders(
		core::Real max_loop_distance
	) const;

	core::Size
	pose_num(
		int model_id,
		core::Size resnum
	) const;

	core::Size
	total_residue()
	const;

	utility::vector1<core::Size>
	pose_loop_anchors() const;

	utility::vector1<core::Size>
	disconnected_segments() const;

	std::set<core::Size>
	native_positions(
		core::pose::Pose const & pose
	) const;

	///@brief count the amount of 'native' residues
	///retained in the pose
	core::Real
	percent_native(
		core::pose::Pose const & pose
	) const;


	///@brief print pymol selection of "native" positions
	std::string
	natives_select(
		core::pose::Pose const & pose,
		std::string object_name = ""
	) const;

	utility::vector1< std::pair<core::Size, core::Size> >
	path() const;

	std::string
	string_path() const;

	std::string
	string_blosum(
		utility::vector1< std::pair<core::Real, core::Size> > blosum_history
	) const;

	void
	set_partner(
		core::pose::PoseOP partner_pose
	);

	core::pose::PoseOP
	get_partner() const;

	///@brief append the segments to the given assembly. This should
	///be used very infrequently. The normal way to add segments is to
	///follow an edge
	void
	append_segments(
		utility::vector1<SewSegment> const & segments
	);

	///@brief delete segments at the given index. This should
	///be used very infrequently
	void
	delete_segments(
		core::Size seg_index
	);

	friend
	std::ostream &
	operator<< ( std::ostream & out, Assembly const & atom);

	////////////////////////////////////////////////////////////
	/////////////////   Iterator functions   ///////////////////
	////////////////////////////////////////////////////////////

	ModelConstIterator<SewSegment>
	assembly_begin() const;

	ModelIterator<SewSegment>
	assembly_begin();

	ModelConstIterator<SewSegment>
	assembly_end() const;

	ModelIterator<SewSegment>
	assembly_end();


	//Blosum Score and Number of Residues for each edge
	utility::vector1< std::pair<core::Real, core::Size> > blosum_history_;

protected:

	SegmentGraph segments_;
	core::pose::PoseOP partner_pose_;

	//A list of all nodes in the assembly that have not been built off of
	std::set<core::Size> available_nodes_;

	//Doesn't trim segments, and therefore has a complete history of assembly,
	//also used for alignments (to ensure we don't try to align to non-existent segments)
	utility::vector1< utility::vector1<SewSegment> > all_segments_;

	utility::vector1< boost::tuple<ModelNode const *, ModelNode const *, HashEdge const *> > edge_path_;
	utility::vector1< std::pair<core::Size, core::Size> > path_;

};


} //legacy_sewing namespace
} //protocols namespace

#endif
