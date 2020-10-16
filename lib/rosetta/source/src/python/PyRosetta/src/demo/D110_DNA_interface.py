#!usr/bin/env python

from __future__ import print_function

################################################################################
# A GENERAL EXPLANATION

"""
dna_interface.py

This script performs interface structure prediction (high-resolution docking)
on a DNA-protein complex. This sample script is very similar to docking.py
without interface location prediction (the low-resolution (centroid) docking
stages). The high-resolution (fullatom) docking stage consists of small
rigid-body perturbations, sidechain packing, and minimization. Without
interface location prediction, this sample script cannot perform full DNA-
protein docking. The "dna" scoring function is optimized for
DNA-protein docking.

Instructions:

1) ensure that your PDB file is in the current directory
2) run the script:
    from commandline                        >python D110_DNA_interface.py

    from within python/ipython              [1]: run D110_DNA_interface.py

Author: Evan H. Baugh
    based on an original script by Sid Chaudhury
    revised and motivated by Robert Schleif

Updated by Boon Uranukul, 6/9/12
Simplified special constant seed initialization ~ Labonte

References:
    J. J. Gray, "High-resolution protein-protein docking," Curr. Opinions in
        Struct. Bio. 16 (2) 183-193 (2006).

"""

################################################################################
# THE BASIC PROTOCOL, sample_dna_interface

"""
This sample script is setup for usage with
    commandline arguments,
    default running within a python interpreter,
    or for import within a python interpreter,
        (exposing the sample_dna_interface method)

The method sample_dna_interface:
1.  creates a pose from the desired PDB file
2.  sets up the pose FoldTree for docking
3.  creates a copy of the pose to be modified
4.  creates a ScoreFunctions for scoring DNA-protein complexes
5.  sets up the DockMCMProtocol object for fullatom docking
6.  creates a (Py)JobDistributor for managing multiple trajectories
7.  create a PyMOL_Observer for viewing intermediate output
8.  exports the original structure to PyMOL
9.  perform protein-protein docking:
        a. set necessary variables for the new trajectory
            -reset the pose structure to the input conformation
            -change the pose's PDDInfo.name, for exporting to PyMOL
        b. perform Rosetta high-resolution docking
        c. output the decoy structure
            -to PyMOL using the PyMOL_Observer.pymol.apply
            -to a PDB file using the PyJobDistributor.output_decoy

"""

import optparse    # for sorting options

import pyrosetta
import pyrosetta.rosetta as rosetta

from pyrosetta import init, Pose, pose_from_file, Vector1, create_score_function, PyJobDistributor
from pyrosetta.rosetta import core, protocols

init(extra_options = "-constant_seed")
# normally, init() works fine
# for this sample script, we want to ease comparison by making sure all random
#    variables generated by Rosetta in this instance of PyRosetta start from a
#    constant seed
# here we provide the additional argument "-constant_seed" which sets all the
#    random variables generated by Rosetta from a constant seed (google random
#    seed for more information)
# some options can be set after initialization, please see PyRosetta.org FAQs
#    for more information
# WARNING: option '-constant_seed' is for testing only! MAKE SURE TO REMOVE IT IN PRODUCTION RUNS!!!!!

import os; os.chdir('.test.output')


#########
# Methods

def sample_dna_interface(pdb_filename, partners,
        jobs = 1, job_output = 'dna_output'):
    """
    Performs DNA-protein docking using Rosetta fullatom docking (DockingHighRes)
        on the DNA-protein complex in  <pdb_filename>  using the relative chain
        <partners>  .
        <jobs>  trajectories are performed with output structures named
        <job_output>_(job#).pdb.

    """
    # 1. creates a pose from the desired PDB file
    pose = Pose()
    pose_from_file(pose, pdb_filename)

    # 2. setup the docking FoldTree
    # using this method, the jump number 1 is automatically set to be the
    #    inter-body jump
    dock_jump = 1
    # the exposed method setup_foldtree takes an input pose and sets its
    #    FoldTree to have jump 1 represent the relation between the two docking
    #    partners, the jump points are the residues closest to the centers of
    #    geometry for each partner with a cutpoint at the end of the chain,
    # the second argument is a string specifying the relative chain orientation
    #    such as "A_B" of "LH_A", ONLY TWO BODY DOCKING is supported and the
    #    partners MUST have different chain IDs and be in the same pose (the
    #    same PDB), additional chains can be grouped with one of the partners,
    #    the "_" character specifies which bodies are separated
    # the third argument...is currently unsupported but must be set (it is
    #    supposed to specify which jumps are movable, to support multibody
    #    docking...but Rosetta doesn't currently)
    # the FoldTrees setup by this method are for TWO BODY docking ONLY!
    protocols.docking.setup_foldtree(pose, partners, Vector1( [dock_jump]))

    # 3. create a copy of the pose for testing
    test_pose = Pose()
    test_pose.assign(pose)

    # 4. create ScoreFunctions for centroid and fullatom docking
    scorefxn = create_score_function('dna')
    scorefxn.set_weight(core.scoring.fa_elec , 1)    # an "electrostatic" term

    #### global docking, a problem solved by the Rosetta DockingProtocol,
    ####    requires interface detection and refinement
    #### as with other protocols, these tasks are split into centroid (interface
    ####    detection) and high-resolution (interface refinement) methods
    #### without a centroid representation, low-resolution DNA-protein
    ####    prediction is not possible and as such, only the high-resolution
    ####    DNA-protein interface refinement is available
    #### WARNING: if you add a perturbation or randomization step, the
    ####    high-resolution stages may fail (see Changing DNA Docking
    ####    Sampling below)
    #### a perturbation step CAN make this a global docking algorithm however
    ####    the rigid-body sampling preceding refinement will require EXTENSIVE
    ####    sampling to produce accurate results and this algorithm spends most
    ####    of its effort in refinement (which may be useless for the predicted
    ####    interface)

    # 5. setup the high resolution (fullatom) docking protocol (DockMCMProtocol)
    # ...as should be obvious by now, Rosetta applications have no central
    #    standardization, the DockingProtocol object can be created and
    #    applied to perform Rosetta docking, many of its options and settings
    #    can be set using the DockingProtocol setter methods
    # as there is currently no centroid representation of DNA in the chemical
    #    database, the low-resolution docking stages are not useful for
    #    DNA docking
    # instead, create an instance of just the high-resolution docking stages
    docking = protocols.docking.DockMCMProtocol()
    docking.set_scorefxn(scorefxn)

    # 6. setup the PyJobDistributor
    jd = PyJobDistributor( job_output , jobs , scorefxn )

    # 7. setup a PyMOL_Observer (optional)
    # the PyMOL_Observer object owns a PyMOLMover and monitors pose objects for
    #    structural changes, when changes are detected the new structure is
    #    sent to PyMOL
    # fortunately, this allows investigation of full protocols since
    #    intermediate changes are displayed, it also eliminates the need to
    #    manually apply the PyMOLMover during a custom protocol
    # unfortunately, this can make the output difficult to interpret (since you
    #    aren't explicitly telling it when to export) and can significantly slow
    #    down protocols since many structures are output (PyMOL can also slow
    #    down if too many structures are provided and a fast machine may
    #    generate structures too quickly for PyMOL to read, the
    #    "Buffer clean up" message
    # uncomment the line below to use the PyMOL_Observer
##    AddPyMOLObserver(test_pose, True)

    # 8. perform protein-protein docking
    counter = 0    # for pretty output to PyMOL
    while not jd.job_complete:
        # a. set necessary variables for this trajectory
        # -reset the test pose to original (centroid) structure
        test_pose.assign(pose)
        # -change the pose name, for pretty output to PyMOL
        counter += 1
        test_pose.pdb_info().name(job_output + '_' + str(counter))

        # b. perform docking
        docking.apply(test_pose)

        # c. output the decoy structure:
        # to PyMOL
        test_pose.pdb_info().name(job_output + '_' + str(counter) + '_fa')
        # to a PDB file
        jd.output_decoy(test_pose)

################################################################################
# INTERPRETING RESULTS

"""
The (Py)JobDistributor will output the lowest scoring pose for each trajectory
(as a PDB file), recording the score in <job_output>.fasc. Generally,
the decoy generated with the lowest score contains the best prediction
for the protein conformation. PDB files produced from docking will contain
both docking partners in their predicted conformation. When inspecting these
PDB files (or the PyMOL_Observer output) be aware that PyMOL can introduce or
predict bonds that do not exist, particularly for close atoms. This rarely
occurs when using the PyMOLMover.keep_history feature (since PyRosetta will
sample some conformation space that has clashes).

The PyMOL_Observer will output a series of structures directly produced by the
DockingProtocol. Unfortunately, this may include intermediate structures that
do not yield any insight into the protocol performance. A LARGE number of
structures are output to PyMOL and your machine may have difficulty
loading all of these structures. If this occurs, try changing the
PyMOL_Observer keep_history to False or running the protocol without the
PyMOL_Observer.

Interface structure prediction is useful for considering what physical
properties are important in the binding event and what conformational changes
occur. Once experienced using PyRosetta, you can easily write scripts to
investigate the Rosetta score terms and structural characteristics. There is no
general interpretation of DNA-binding results. Although Rosetta score does not
translate directly to physical meaning (it is not physical energy), splitting
the docked partners and comparing the scores (after packing or refinement) can
indicate the strength of the bonding interaction.

"""

################################################################################
# COMMANDLINE COMPATIBILITY

# everything below is added to provide commandline usage,
#   the available options are specified below
# this method:
#    1. defines the available options
#    2. loads in the commandline or default values
#    3. calls dna_sample_dna_interface with these values

# parser object for managing input options
# all defaults are for the example using "test_dna.pdb" with reduced
#    cycles/jobs to provide results quickly
parser = optparse.OptionParser()
parser.add_option('--pdb_filename', dest = 'pdb_filename',
    default = '../test/data/dna_test.pdb',    # default example PDB
    help = 'the PDB file containing the DNA and protein to dock')
# for more information on "partners", see sample_docking step 2.
parser.add_option( '--partners', dest = 'partners',
    default = 'A_CD',    # default for the example dna_dock.pdb
    help = 'the relative chain partners for docking')
# PyJobDistributor options
parser.add_option('--jobs', dest='jobs',
    default = '1',    # default to single trajectory for speed
    help = 'the number of jobs (trajectories) to perform')
parser.add_option('--job_output' , dest = 'job_output' ,
    default = 'dna_output' ,    # if a specific output name is desired
    help = 'the name preceding all output, output PDB files and .fasc')
(options,args) = parser.parse_args()

# PDB file option
pdb_filename = options.pdb_filename
partners = options.partners

# JobDistributor options
jobs = int(options.jobs)
job_output = options.job_output

sample_dna_interface(pdb_filename, partners, jobs, job_output)

################################################################################
# ALTERNATE SCENARIOS

#######################################
# Obtaining and Preparing DNA PDB files
"""
PDB files are the keys to structural Bioinformatics and structure prediction.
PDB files are most easily obtained from the RCSB website but may contain
variability which makes them uncompatible with PyRosetta. DNA in particular must
exist in a specific format for PyRosetta to read it properly. The (PDB format)
residue characters must be right-justified single DNA letters (i.e. if the raw
download contains "  DG" change this to "   G"). For PDB files containing both
protein and nucleic acids, all protein chains must be listed first (PyRosetta
will still load these PDBs, but the docking FoldTree will be wrong*).
To obtain a new PDB file:

1) locate your protein of interest at http://www.pdb.org/
2) download the PDB file, using a browser this includes:
    a. clicking "Download Files" on the upper right
    b. clicking "PDB File (text)", the second option
3) Manually edit the file to remove lines which may hinder PyRosetta
    (use PyMOL, grep, awk, Python, Biopython, or whatever technique you prefer)
4) Manually edit the file to contain proper DNA residue names and chain order
    (use PyMOL, grep, awk, Python, Biopython, or whatever technique you prefer)


When preparing PDB files for docking, remember that the two chains to dock must
be part of the same Pose object. This is easily attained by creating a PDB file
which includes both partners. If only interface structure prediction (high-
resolution) is used, the PDB file MUST contain the molecules ORIENTED properly
for the interface or the sampling will rarely find a proper structure.

Methods for downloading and generically "cleaning" PDB files should accompany
future PyRosetta releases.

*This error occurs because the methods were designed for usage with two-body
docking problems and there is a hard-coded definition of "upstream" and
"downstream" for docking partners, the position of the upstream docking partner
is held constant while the downstream partner is altered by rigid-body
perturbations, this does NOT affect the accuracy of predictions but can be an
annoyance since the protein coordinates can significantly change though its
conformation will not

"""

################
# A Real Example
"""
All of the default variables and parameters used above are specific to
the example with "test_dna.pdb", which is supposed to be simple,
straightforward, and speedy. Here is a more practical example:

The TATA Box is very important in understanding DNA-protein interactions and
gene expression. Suppose you are curious about these protein binding regions
and want to predict conformational variability using PyRosetta.

1. Download a copy of RCSB PDB file 1VTL (remove waters and any other HETATM)
2. Edit the PDB file such that only one docked DNA-protein remains (1VTL is a
        dimer crystal structure and we intend to investigate one interaction)
3. Edit the PDB file such that any deoxynucleic acid is listed in Rosetta's
        required format (i.e. "  DG" -> "   G") and chain E comes first
4. Make a directory containing:
        -the PDB file for 1VTL (cleaned of HETATMs and waters)
            lets name it "1VTL.clean.pdb" here
        -this sample script (technically not required, but otherwise the
            commands in 4. would change since docking.py wouldn't be here)
5. Run the script from the commandline with appropriate arguments:

>python dna_interface.py --pdb_filename 1VTL.clean.pdb --partners E_AB --jobs 400 --job_output 1VTL_docking_output

        -The partners option, "E_AB" is PDB specific, if you chose to retain
            different chains (in step 2.) or otherwise change the chain IDs
            in 1VTL, make sure this string matches the desired chain interaction
        -400 trajectories is low, sampling docking conformations is difficult,
            typically thousands of (800-1000) trajectories are attempted
        -This script features the PyMOL_Observer (may be commented out to avoid
            using it), Monte Carlo simulations are not expected to produce
            kinetically meaningful results and as such, viewing the intermediates
            is only useful when understanding a protocol and rarely produces
            insight beyond the final output

5. Wait for output, this will take a while (performing 400 trajectories
        of the DockingHighRes is intensive)
6. Analyze the results (see INTERPRETING RESULTS above)

Note: this is a direct port of the Rosetta docking protocol and provides example
syntax for using this method within Python. This script also provides example
code for PyRosetta. A priori, a large number of jobs (~1000 or more) is required
to achieve useful results. The best protocols are somewhat protein-specific and
the scoring and sampling methods here may be customized to produce better
results on your complex.

"""

##############################
# Changing DNA Docking Sampling
"""
This script performs DNA-protein interface structure prediction but does NOT
perform global DNA-protein docking. Since there is no generic interface
detection, the input PDB file must have the DNA placed near the interface
that will be refined. If the DockMCMProtocol is applied to a pose
without placement near the interface, then the refinement may:
    -waste steps sampling the wrong interface
    -fail by predicting an incorrect interface very far from the true interface
    -fail by separating the DNA from the protein (usually due to a clash)
DockMCMProtocol does not require an independent randomization or perturbation
step to "seed" its prediction.

Additional refinement steps may increase the accuracy of the predicted
conformation (see refinement.py). Drastic moves (large conformational changes)
should be avoided; if they precede the protocol, the problems above may occur,
if they succeed the protocol, the protocol results may be lost. Please read
"Changing Docking Sampling" in docking.py and "Custom Docking" below to better
understand the explicit moves performed during docking if you wish to
create your own protocol.

Many other approaches to docking exist however PyRosetta exposes a myriad of
tools for constructing novel Monte Carlo algorithms for docking prediction.

Please try alternate sampling methods to better understand how these
algorithms perform and to find what moves best suite your problem.

"""

#############################
# Changing DNA Docking Scoring
"""
The "ligand" ScoreFunction is optimized for ligand-protein interface prediction and
is very similar to the "standard" ScoreFunction with modified weights. The
score term "fa_elec" is activated to (further) penalize structures with poor
electrostatic interactions. The adjusted (and activated) weights in the "ligand"
ScoreFunction exist to emphasize interactions between the ligand and protein.
Many score terms representing enthaplic bonuses (hydrogen bond formation,
salt-bridge formation, etc.) have higher weights.

The dominant interactions in ligand-protein binding are not necessarily
general or specific. Many score terms in Rosetta may improve the accuracy of
ligand-protein interface structure prediction. Please try alternate scoring
functions or unique selection methods to better understand which scoring terms
contribute to performance and to find what scoring best suites your problem.

"""

################
# Custom Docking
"""
If you wish to develop a custom high-resolution docking protocol, PyRosetta
exposes a number of tools. The sequence of moves presented below is very
similar to the docking performed by DockMCMProtocol.apply. This function
packs the input protein, minimizes using DockMinMover, and performs several
cycles of docking with DockMCMCycle (combination of small rigid-body moves,
packing, and minimization). The syntax below is complete...but does not work.
There is some problem using DockMCMCycle but it IS avoidable (I just don't
know how so I've presented my findings here instead of above).

# the first argument id the Vector1 of movable jumps (see the script step 2.
#    above), the second argument is the ScoreFunction to use in docking, and
#    the third argument is the ScoreFunction to use in packing
dock = DockMCMCycle( Vector1( [dock_jump] ) , scorefxn , scorefxn )

# for initial packing
task = standard_packer_task( test_pose )
task.restrict_to_repacking()
packer = PackRotamersMover( scorefxn , task )

# for initial minimization
minmover = DockMinMover( Vector1( [dock_jump] ) , scorefxn , dock.get_mc() )

cycles = 49    # if using this, make it an argument
dockmover = RepeatMover( dock , cycles )

# create a SequenceMover of these moves
docking = protocols.moves.SequenceMover()
docking.add_mover( packer )
docking.add_mover( minmover )
docking.add_mover( dockmover )

# make sure to use dock.get_mc().recover_low() or
#    dock.get_mc().lowest_score_pose()at the end of the protocol

"""

###############
# Other Docking
"""
Binding interaction drive MANY biological processes. While protein-protein
docking addresses many possibilities, there are other compounds which a user
may wish to investigate. Rosetta (and thus the core methods exposed in
PyRosetta) provides protocols for predicting docking with other chemical
entities such as DNA, RNA (see dna_docking.py), and small ligands (see
ligand_docking.py). Rosetta provides symmetry tools for predicting symmetric
protein complexes (another kind of protein-protein interaction) but these
methods are not currently supported in PyRosetta. Peptide (or other flexible
molecule) docking does not currently have a fine tuned protocol however
PyRosetta provides all the tools necessary for such algorithms. When binding
a highly flexible molecule, the sampling space becomes much larger,
compounding the already large search space of docking and conformation
prediction. Such algorithms are anticipated to require extremely intensive
sampling to achieve useful results. Antibody docking is supported by Rosetta
is likewise confounded by a very large search space. Small molecule docking
covers a large variety of biologically relevant interactions including
(potentially) the placement of structurally important water molecules.

Problem                    Rosetta tool        In PyRosetta?
protein-protein docking    DockingProtocol     yes
small molecule docking     LigandDockProtocol  components, use DockMCMProtocol
nucleic acid interface     DNAInterfaceFinder  components, use DockMCMProtocol
symmetric complexes        SymDockProtocol     no
flexible polymer docking   -                   no
antibody docking           AntibodyModeler     no
water placement            -                   tools, no direct protocol

"""
