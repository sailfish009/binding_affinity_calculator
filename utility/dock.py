import os
import sys
import subprocess


def dock():
    result = subprocess.run(["lib/rosetta/source/bin/docking_protocol.linuxgccrelease",
                             "@flag_local_refine", "-overwrite"], stdout=subprocess.PIPE)
    record = result.stdout
    lines = record.splitlines()
    for i in range(len(lines), -1, -1):
        pass
    # os.system("lib/rosetta/source/bin/docking_protocol.linuxgccrelease @flag_local_refine -overwrite")
    return lines

