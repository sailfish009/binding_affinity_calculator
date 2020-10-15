# The executable file of the binding affinity tool based on the Rosetta platform.
# The estimation does not yield high level of accuracy. The API is designed for rough calculation only.
from utility.mutation import mutate_atoms
from utility.residue_matching_loader import load_residue_matching
from utility.peptide_loader import load_peptides


if __name__ == '__main__':
    # load a list of peptide sequence from input file
    peptides = load_peptides("lib/sample_peptide_sequences.txt")
    # generate mutated molecule file for further processing
    print("Starting the mutation of atoms...")
    for p in peptides:
        mutate_atoms(str(p))
    print("The mutation of atoms finished. Starting the docking process...")
    # dock the known peptide sequence and protein
