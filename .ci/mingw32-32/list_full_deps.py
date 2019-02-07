import sys
import subprocess
import os


def findFileLocation(file_name, expected_subdir="/usr"):
    proc = subprocess.Popen(
        ["find", expected_subdir, "-name", file_name], stdout=subprocess.PIPE)
    # for loc_path in proc.communicate()[0]:
    loc_path = proc.communicate()[0]

    for p in loc_path.split("\n"):
        if p.count(".dll"):
            return p


def getDepsOfLib(lib_path):
    #print "Getting deps of", lib_path
    proc_objdump = subprocess.Popen(
        ["i686-w64-mingw32-objdump", "-p", lib_path], stdout=subprocess.PIPE, shell=False)
    proc_grep = subprocess.Popen(
        ["grep", "\\.dll"], stdin=proc_objdump.stdout, stdout=subprocess.PIPE, shell=False)
    proc_awk = subprocess.Popen(
        ["awk", "{print $3}"], stdin=proc_grep.stdout, stdout=subprocess.PIPE, shell=False)

    proc_objdump.stdout.close()
    proc_grep.stdout.close()
    return proc_awk.communicate()[0].split("\n")


# Expect a list of system file deps in sys.argv, like:
# libc++std.so.6 libwhatever.so.4 libanotherone.so.1,

# Actually no, expect the path to an executable

#exec_path = sys.argv[1:]
deps = sys.argv[1:]
dep_paths = []

# get the direct deps of the exec
# deps.extend(getDepOfLib(exec_path))
# We'll iterate over the list of deps and find their actual file location
for dep in deps:
    dep_location = findFileLocation(dep)

    if dep_location:
        #print "Adding", dep_location, "to dep_paths"
        dep_paths.append(dep_location)
    for dep_path in dep_paths:
        extra_deps = getDepsOfLib(dep_path)
        #print "Found extra deps:", extra_deps
        for extra_dep in extra_deps:
            if not deps.count(extra_dep):
                #print extra_dep, "is not already in deps, adding"
                deps.append(extra_dep)


print " ".join(dep_paths)
