# The executable file of the binding affinity tool based on the Rosetta platform.
# The estimation does not yield high level of accuracy. The API is designed for rough calculation only.
from utility.dock import dock
import os


if __name__ == '__main__':
    print("Starting the mutation of atoms...")
    os.system("python3 generate_mutation.py")
    print("The mutation of atoms finished. Starting the docking process...")
    print("Some side product of the docking: ")
    binding_energy_docking = dock()
    print("The docking process completed. Starting the calculation of binding affinity...")
