import os
import copy
SetOption("random", 1)

def get_static_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-2]

def get_shared_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-3]

env = Environment(CCFLAGS='-g -O2 -Wall -std=c++11', CPPPATH=[
        "#src",
        "#3rd-library/include"
        ])

Export("env")
fly = SConscript("src/fly/SConscript", variant_dir="build/fly", duplicate=0)
env.Install("build/bin", fly)

libs = [
    get_static_library_name(fly)
]

lib_path = [
    "#build/bin"
]

env.Replace(LIBS=libs, LIBPATH=lib_path)
Export("env")
testserver = SConscript("test/SConscript", variant_dir="build/testserver", duplicate=0)
env.Install("build/bin", testserver)
