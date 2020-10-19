import subprocess


def dock():
    result = subprocess.run(["lib/rosetta/source/bin/docking_protocol.linuxgccrelease",
                             "@flag_local_refine", "-overwrite"], stdout=subprocess.PIPE)
    record = result.stdout
    with open("structures/docked_intermediate/docking_record.txt", "w") as f:
        f.write(str(record))
        f.close()
    lines = record.splitlines()
    energy_dict = {}
    for i in range(len(lines)-1, -1, -1):
        if len(energy_dict) >= 3:
            break
        line = str(lines[i])
        if "energy" not in line:
            continue
        elif "interaction" in line:
            interaction_energy = get_energy_value(line)
            energy_dict["interaction"] = interaction_energy
            continue
        elif "unbound" in line:
            bounded_energy = get_energy_value(line)
            energy_dict["unbounded"] = bounded_energy
            continue
        elif "bound" in line:
            unbounded_energy = get_energy_value(line)
            energy_dict["bounded"] = unbounded_energy
            continue
    write_energy(energy_dict)
    return energy_dict


def get_energy_value(energy_line):
    components = energy_line.split(":")
    print(components[-1])
    raw_value = components[-1].strip()
    preprocessed_value = ""
    for i in range(len(raw_value)):
        if raw_value[i] in "-1234567890.":
            preprocessed_value += raw_value[i]
    energy_value = float(preprocessed_value)
    return energy_value


def write_energy(energy_dict_ref):
    with open("result/binding_energy_based_on_docking.csv", "w") as f:
        for energy_type, energy_value in energy_dict_ref.items():
            f.write("%s,%f\n" % (str(energy_type), float(energy_value)))
            print("%s,%f" % (str(energy_type), float(energy_value)))
