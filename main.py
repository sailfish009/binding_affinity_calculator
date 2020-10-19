# The executable file of the binding affinity tool based on the Rosetta platform.
# The estimation does not yield high level of accuracy. The API is designed for rough calculation only.
from utility.dock import dock
import os


if __name__ == '__main__':
    os.system("python3 generate_mutation.py")
    print("The mutation of atoms finished. Starting the docking process...")
    dock()
