import sys
import subprocess
import os


def findFileLocation(file_name, expected_subdir="/usr"):
    return subprocess.check_output(
        ["find", expected_subdir, "-name", file_name, "|", "grep", "-m", "1", ".dll"])


def getDepOfLib(lib_path):
    find_op = subprocess.check_output(
        ["x86_64-w64-mingw32-objdump", "-p", lib_path, "|", "grep", "dll", "|", "awk", "'{print $3}'"])
    return find_op.split("\n")


# Expect a list of system file deps in sys.argv, like:
# libc++std.so.6 libwhatever.so.4 libanotherone.so.1

deps = sys.argv[1:]
dep_paths = []

# We'll iterate over the list of deps and find their actual file location
for dep in deps:
    dep_paths.append(findFileLocation(dep))
    for extra_dep in getDepOfLib(dep):
        if not deps.count(extra_dep):
            deps.append(extra_dep)

print " ".join(dep_paths)
