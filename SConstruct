import os
import copy
SetOption("random", 1)

def get_static_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-2]

def get_shared_library_name(node):
    return os.path.basename(str(node)[:-2])[3:-3]

env = Environment(CCFLAGS='-fpermissive -g -O2 -pthread -std=c++11', LINKFLAGS='-pthread -Wl,--start-group',
CPPPATH=["#src", "#depend/rapidjson/include", "#depend"])
cryptopp = File('#depend/cryptopp/libcryptopp.a')
env.Command(cryptopp, None, "cd depend/cryptopp && make")

libs = [
    cryptopp
]

lib_path = [
    "#build/bin"
]

env.Replace(LIBS=libs, LIBPATH=lib_path)
Export("env")
fly = SConscript("src/fly/SConscript", variant_dir="build/fly", duplicate=0)
env.Install("build/bin", fly)

env.Append(LIBS=get_static_library_name(fly))
Export("env")

test_server = SConscript("test/SConscript1", variant_dir="build/test_server", duplicate=0)
env.Install("build/bin", test_server)

test_client = SConscript("test/SConscript2", variant_dir="build/test_client", duplicate=0)
env.Install("build/bin", test_client)

test_server_wsock = SConscript("test/SConscript3", variant_dir="build/test_server_wsock", duplicate=0)
env.Install("build/bin", test_server_wsock)
