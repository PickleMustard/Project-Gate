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


def validate_library(lib_path):
    if not os.path.exists(lib_path):
        print(f"Library not found at {lib_path}")
        exit(1)


# def install(package):
    # subprocess.check_call(["sudo", "dnf", "install", package])

# def install_dep(package):
    # print(sys.executable)
    # subprocess.check_call([sys.executable, "-m", "pip", "install", package])


libname = "Scalad_Low_Level_Library"
projectdir = "project-gate"

# install("rapidyaml-devel")
# install_dep("rapidyaml")
validate_library("lib/ryml/libryml.a")

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
print(env.Dump())

corePath = "src/"
modules = getSubdirs(corePath)
Library('libryml', ['lib/ryml/libryml'])
env.Append(CPPPATH=["src/", 'lib/ryml/include/'])
env.Append(LIBS=['libryml'])
env.Append(LIBPATH='./lib/ryml/lib')


sources = Glob(os.path.join(corePath, '*.cpp'))
for module in modules:
    if (module == "rapidyaml"):
        print(os.path.join(corePath, module, '*.cpp'))
        # exec(open(os.path.join(corePath, module, 'setup.py')).read())
    else:
        sources += Glob(os.path.join(module, '*.cpp'))

# sources += Glob(os.path.join(rymlPath, '*.cpp'))
# modules = getSubdirs(rymlPath)

# for module in modules:
    # sources += Glob(os.path.join(module, '*.cpp'))


file = "{}{}{}".format(libname, env["suffix"], env["SHLIBSUFFIX"])

if env["platform"] == "macos":
    platlibname = "{}.{}.{}".format(libname, env["platform"], env["target"])
    file = "{}.framework/{}".format(env["platform"], platlibname, platlibname)

libraryfile = "bin/{}/{}".format(env["platform"], file)
library = env.SharedLibrary(
    libraryfile,
    source=sources,
)

copy = env.InstallAs("{}/bin/{}/lib{}".format(projectdir,
                     env["platform"], file), library)

default_args = [library, copy]
if localEnv.get("compiledb", False):
    default_args += [compilation_db]
Default(*default_args)
