#!/usr/bin/env python
import os
import subprocess
import sys

def getSubdirs(abs_path_dir) :
    lst = [ name for name in os.listdir(abs_path_dir) if os.path.isdir(os.path.join(abs_path_dir, name)) and name[0] != '.' ]
    lst.sort()
    return lst

def normalize_path(val, env):
    return val if os.path.isabs(val) else os.path.join(env.Dir("#").abspath, val)


def validate_parent_dir(key, val, env):
    if not os.path.isdir(normalize_path(os.path.dirname(val), env)):
        raise UserError("'%s' is not a directory: %s" % (key, os.path.dirname(val)))

def install(package) :
    subprocess.check_call(["sudo", "dnf", "install", package])


libname = "Scalad_Low_Level_Library"
projectdir = "project-gate"

install("rapidyaml-devel")

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

corePath = "src/"
modules = getSubdirs(corePath)

env.Append(CPPPATH=["src/"])
env.Append(LIBS=["libryml"])

sources = Glob(os.path.join(corePath, '*.cpp'))
for module in modules:
    if(module == "rapidyaml") :
        print(os.path.join(corePath, module, '*.cpp'))
        #exec(open(os.path.join(corePath, module, 'setup.py')).read())
    else :
        sources += Glob(os.path.join(corePath, module, '*.cpp'))


file = "{}{}{}".format(libname, env["suffix"], env["SHLIBSUFFIX"])

if env["platform"] == "macos":
    platlibname = "{}.{}.{}".format(libname, env["platform"], env["target"])
    file = "{}.framework/{}".format(env["platform"], platlibname, platlibname)

libraryfile = "bin/{}/{}".format(env["platform"], file)
library = env.SharedLibrary(
    libraryfile,
    source=sources,
)

copy = env.InstallAs("{}/bin/{}/lib{}".format(projectdir, env["platform"], file), library)

default_args = [library, copy]
if localEnv.get("compiledb", False):
    default_args += [compilation_db]
Default(*default_args)

