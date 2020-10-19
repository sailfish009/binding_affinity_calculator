import os
import subprocess


def modify_flag():
    pass


def dock():
    os.system("lib/rosetta/source/bin/docking_protocol.linuxgccrelease @flag_local_refine -overwrite")
