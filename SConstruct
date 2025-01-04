#!/usr/bin/env python
import os
import subprocess
import sys


def getSubdirs(abs_path_dir):
    lst = []
    dir_list = os.listdir(abs_path_dir)
    for name in dir_list:
        if os.path.isdir(os.path.join(abs_path_dir, name)) and name[0] != '.':
            lst = lst + (getSubdirs(os.path.join(abs_path_dir, name)))
            lst.append(os.path.join(abs_path_dir, name))
    lst.sort()
    return lst


def normalize_path(val, env):
    return val if os.path.isabs(val) else os.path.join(env.Dir("#").abspath, val)


def validate_parent_dir(key, val, env):
    if not os.path.isdir(normalize_path(os.path.dirname(val), env)):
        raise UserError("'%s' is not a directory: %s" %
                        (key, os.path.dirname(val)))


def build_rapidyaml(env):
    ryml_build_dir = os.path.join(ryml_dir, 'build')
    if not os.path.exists(ryml_build_dir):
        os.makedirs(ryml_build_dir)

    cmake_config = [
        '-DCMAKE_BUILD_TYPE=Release',
        '-DRYML_BUILD_TESTS=OFF',
        '-DCMAKE_POSITION_INDEPENDENT_CODE=ON',
    ]
    cmake_cmd = f'cd {
        ryml_build_dir} && cmake .. {" ".join(cmake_config)}'
    make_cmd = f'cd {ryml_build_dir} && make'

    env.Execute(cmake_cmd)
    env.Execute(make_cmd)

    return os.path.join(ryml_build_dir, 'libc4core.a')


def validate_library(lib_path):
    if not os.path.exists(lib_path):
        print(f"Library not found at {lib_path}")
        exit(1)


libname = "Scalad_Low_Level_Library"
projectdir = "project-gate"
ryml_dir = "extern/rapidyaml"

# install("rapidyaml-devel")
# install_dep("rapidyaml")
# validate_library("lib/ryml/libryml.a")

localEnv = Environment(tools=["default"], PLATFORM="")

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Add(
    BoolVariable(
        key="compiledb",
        help="Generate compilation DB (`compile_commands.json`) for external tools",
        default=localEnv.get("compiledb", False),
    )
)
opts.Add(
    PathVariable(
        key="compiledb_file",
        help="Path to a custom `compile_commands.json` file",
        default=localEnv.get("compiledb_file", "compile_commands.json"),
        validator=validate_parent_dir,
    )
)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()
env["compiledb"] = False

env.Tool("compilation_db")
compilation_db = env.CompilationDatabase(
    normalize_path(localEnv["compiledb_file"], localEnv)
)
env.Alias("compiledb", compilation_db)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})
# print(env.Dump())

corePath = "src/"
modules = getSubdirs(corePath)
ryml_lib = build_rapidyaml(env)
ryml_lib_dir = os.path.dirname(ryml_lib)
env.Append(CPPPATH=["src/", os.path.join(ryml_dir, 'src'),
           os.path.join(ryml_dir, 'ext/c4core/src')])
env.Append(RPATH=[ryml_lib_dir])
env.Append(LIBS=['libryml'])
print(os.path.dirname(ryml_lib))
env.Append(LIBPATH=ryml_lib_dir)

sources = Glob(os.path.join(corePath, '*.cpp'))
for module in modules:
    if (module == "rapidyaml"):
        print(os.path.join(corePath, module, '*.cpp'))
        # exec(open(os.path.join(corePath, module, 'setup.py')).read())
    else:
        sources += Glob(os.path.join(module, '*.cpp'))

env.Depends(ryml_lib, env["target"])

# sources += Glob(os.path.join(rymlPath, '*.cpp'))
# modules = getSubdirs(rymlPath)

# for module in modules:
# sources += Glob(os.path.join(module, '*.cpp'))

file = "{}{}{}".format(libname, env["suffix"], env["SHLIBSUFFIX"])

if env["platform"] == "macos":
    platlibname = "{}.{}.{}".format(
        libname, env["platform"], env["target"])
    file = "{}.framework/{}".format(env["platform"],
                                    platlibname, platlibname)

libraryfile = "bin/{}/{}".format(env["platform"], file)
library = env.SharedLibrary(
    libraryfile,
    source=sources,
)

print(library)

copy = env.InstallAs("{}/bin/{}/lib{}".format(projectdir,
                                              env["platform"], file), library)

default_args = [library, copy]
if localEnv.get("compiledb", False):
    default_args += [compilation_db]
Default(*default_args)
