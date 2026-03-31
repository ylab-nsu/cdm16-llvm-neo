#!/usr/bin/env python3

import argparse
import os
import shutil
import subprocess
import sys
import platform

PROJECT_PATH=os.path.realpath(__file__ + "/../../../..")

def log(msg, verbose):
    if verbose:
        print(msg)

def default_cc(verbose):
    cc = "clang"
    if (platform.system() == "Windows"):
        log("Running on windows, using cl.exe as default C compiler", verbose)
        cc = "cl"
    if not (shutil.which(cc)):
        raise RuntimeError("No suitable default C compiler found")
    return cc

def default_cxx(verbose):
    cxx = "clang++"
    if (platform.system() == "Windows"):
        log("Running on windows, using cl.exe as default C++ compiler", verbose)
        cxx = "cl"
    if not (shutil.which(cxx)):
        raise RuntimeError("No suitable default C++ compiler found")
    return cxx

def run(cmd, verbose):
    if verbose:
        print("Running:", " ".join(cmd))
    subprocess.check_call(cmd)

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "build_dir",
        nargs="?",
        default=PROJECT_PATH + "/llvm/build",
        help="build directory (default: cdm16-llvm-neo/llvm/build)"
    )
    parser.add_argument("--targets", default="", help="additional targets to build (e.g. \"RISCV;X86;Mips\")"),
    parser.add_argument("--exp-targets", default="", help="additional experimental targets (besides CDM) to build (e.g. \"M68k;DirectX\")")
    parser.add_argument("--host-target", action="store_true", help="enable host target")
    parser.add_argument("--debug", action="store_true", help="build with Debug configuration")
    parser.add_argument("--build-type", default="Release", help="CMake build type [Debug, Release, RelWithDebInfo, MinSizeRel] (default: Release)")
    parser.add_argument("--cc", default="", help="host C compiler to use; support may vary, therefore default is recommended (default: clang or cl)")
    parser.add_argument("--cxx", default="", help="host C++ compiler to use, support may vary; therefore default is recommended (default: clang++ or cl)")
    parser.add_argument("--jobs", "-j", type=int, default=1, help="number of linker jobs (default: 1)")
    parser.add_argument("--generator", "-G", default="Ninja", help="Generator for build tool (default: Ninja)")
    parser.add_argument("--no-assertions", action="store_true", help="disable assertions")
    parser.add_argument("--static", action="store_true", help="build with static linking")
    parser.add_argument("--configure-flags", default="", help="additional CMake configure flags")
    parser.add_argument("--verbose", "-v", action="store_true", help="verbose output")

    args = parser.parse_args()

    llvm_src_dir = PROJECT_PATH + "/llvm"
    build_dir = os.path.abspath(args.build_dir)

    os.makedirs(build_dir, exist_ok=True)

    assertions = "OFF" if args.no_assertions else "ON"

    cc = args.cc if args.cc else default_cc(args.verbose)
    cxx = args.cxx if args.cxx else default_cxx(args.verbose)

    targets = args.targets
    if args.host_target:
        targets = "Native" + (f";{targets}" if targets else "")

    add_targets = ";" + args.exp_targets if args.exp_targets else ""

    cmake_cmd = [
        "cmake",
        "-S", llvm_src_dir,
        "-B", build_dir,
        "-G", args.generator,
        f"-DCMAKE_C_COMPILER={cc}", f"-DCMAKE_CXX_COMPILER={cxx}",
        "-DLLVM_OPTIMIZED_TABLEGEN=ON",
        f"-DLLVM_TARGETS_TO_BUILD={targets}",
        f"-DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=CDM" + add_targets,
        "-DLLVM_DEFAULT_TARGET_TRIPLE=cdm-cocas",
        f"-DCMAKE_BUILD_TYPE={args.build_type}",
        f"-DLLVM_ENABLE_ASSERTIONS={assertions}",
        "-DLLVM_ENABLE_PROJECTS=clang;lld",
        "-DLLVM_ENABLE_RUNTIMES=cdm-rt",
        "-DLLVM_INCLUDE_EXAMPLES=OFF",
        "-DLLVM_INCLUDE_BENCHMARKS=OFF",
        "-DLLVM_BUILD_DOCS=OFF",
        "-DLLVM_ENABLE_OCAMLDOC=OFF",
        "-DLLVM_ENABLE_BINDINGS=OFF",
        "-DLLVM_ENABLE_ZLIB=OFF",
        "-DLLVM_ENABLE_ZSTD=OFF",
        f"-DLLVM_PARALLEL_LINK_JOBS={args.jobs}",
    ]

    if args.static:
        cmake_cmd.extend(["-DLLVM_STATIC_LINK_CXX_STDLIB=ON", "-DLLVM_BUILD_STATIC=ON", "-DLIBCLANG_BUILD_STATIC=ON"])

    if args.configure_flags:
        cmake_cmd.extend(args.configure_flags.split())

    log("Configuring LLVM...", args.verbose)

    run(cmake_cmd, args.verbose)

    build_cmd = [
        "cmake",
        "--build", build_dir
    ]

    log("Building LLVM...", args.verbose)
    run(build_cmd, args.verbose)

    print("\nLLVM build completed successfully.")
    print(f"Build directory: {build_dir}")
    print(f"Build type: {args.build_type}")
    print(f"Assertions: {'ON' if assertions == 'ON' else 'OFF'}")
    print(f"Compilers used: {cc}, {cxx}")

if __name__ == "__main__":
    main()

