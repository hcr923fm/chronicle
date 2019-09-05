import sys
import subprocess
import os
import re

try:
    HOST_TRIPLET = os.environ["COMPILER_TRIPLET"]
except:
    HOST_TRIPLET = ""


def findFileLocation(file_name, expected_subdir):
    proc = subprocess.Popen(
        ["find", expected_subdir, "-name", file_name], stdout=subprocess.PIPE)
    loc_path = proc.communicate()[0]

    for p in loc_path.split("\n"):
        if re.match(".*\.(dll|so|a)$", p) and p.count(HOST_TRIPLET):
            return p


def getDepsOfLib(lib_path):
    # print "Getting deps of", lib_path
    objdump_path = "objdump" if not HOST_TRIPLET else "{0}-objdump".format(
        HOST_TRIPLET)

    proc_objdump = subprocess.Popen(
        [objdump_path, "-p", lib_path], stdout=subprocess.PIPE, shell=False)
    proc_grep = subprocess.Popen(
        ["egrep", "lib.*\.(dll|so|a)$", "-o"], stdin=proc_objdump.stdout, stdout=subprocess.PIPE, shell=False)
    proc_objdump.stdout.close()
    return proc_grep.communicate()[0].split("\n")

# Expect the path to an executable


deps = getDepsOfLib(sys.argv[1])
dep_paths = []

# get the direct deps of the exec
# deps.extend(getDepOfLib(exec_path))
# We'll iterate over the list of deps and find their actual file location
for dep in deps:
    # print "Getting deps of", dep
    dep_location = findFileLocation(
        dep, "/usr/%s" % HOST_TRIPLET if HOST_TRIPLET else "/usr/")

    if dep_location:
        # print "Adding", dep_location, "to dep_paths"
        dep_paths.append(dep_location)
    for dep_path in dep_paths:
        extra_deps = getDepsOfLib(dep_path)
        # print "Found extra deps:", extra_deps
        for extra_dep in extra_deps:
            if not deps.count(extra_dep):
                # print extra_dep, "is not already in deps, adding"
                deps.append(extra_dep)


print " ".join(dep_paths)
