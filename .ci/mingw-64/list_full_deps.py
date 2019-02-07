import sys
import subprocess
import os


def findFileLocation(file_name, expected_subdir="/usr"):
    proc = subprocess.Popen(
        ["find", expected_subdir, "-name", file_name], stdout=subprocess.PIPE)
    for loc_path in proc.communicate()[0]:
        if loc_path.count(".dll"):
            return loc_path


def getDepOfLib(lib_path):
    proc_objdump = subprocess.Popen(
        ["x86_64-w64-mingw32-objdump", "-p", lib_path], stdout=subprocess.PIPE, shell=False)
    proc_grep = subprocess.Popen(
        ["grep", "dll"], stdin=proc_objdump.stdout, stdout=subprocess.PIPE, shell=False)
    proc_awk = subprocess.Popen(
        ["awk", "'{print $3}'"], stdin=proc_grep.stdout, stdout=subprocess.PIPE, shell=False)

    proc_objdump.stdout.close()
    proc_grep.stdout.close()
    return proc_awk.communicate()[0].split("\n")


# Expect a list of system file deps in sys.argv, like:
# libc++std.so.6 libwhatever.so.4 libanotherone.so.1,

# Actually no, expect the path to an executable

exec_path = sys.argv[1]
deps = []
dep_paths = []

# get the direct deps of the exec
deps.extend(getDepOfLib(exec_path))
# We'll iterate over the list of deps and find their actual file location
for dep in deps:
    dep_paths.append(findFileLocation(dep))
    for extra_dep in getDepOfLib(dep):
        if not deps.count(extra_dep):
            deps.append(extra_dep)

print " ".join(dep_paths)
