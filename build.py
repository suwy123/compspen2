# -*- coding: utf-8 -*-

import subprocess
import os
import sys
import json

def exec_cmd(cmd):
    """ run one command and return the output.
    """
    print("RUN: " + cmd)
    with subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True,
            encoding="utf-8") as status:
        return status.stdout.read()
    return None


def init():
    global config
    with open("config.json", "r") as fin:
        config = json.load(fin)

    config["testobjs"] = {}

    if not os.path.exists(config["build_dir"]):
        os.makedirs(config["build_dir"])

    print(config)


def compile_file(src_path, dst_path):
    """Compile file.
    Args:
        src_path: src_path
        dst_path: dst_path
    """
    inc_flags = "-I " + add_prefix(config["inc_dir"], "pro_dir") + " -I " + config["inc_z3_dir"] + " -I " + config["inc_boost_dir"]# + " -I C:/MyProgram/SEPSolver-master/include/z3" + " -I C:/MyProgram/boost/boost_1_71_0"
    cmd = [config["CC"], config["cflags"], inc_flags, src_path, 
            "-c -o", dst_path]
    cmd = " ".join(cmd)
    output = exec_cmd(cmd)
    if output:
        print(output)


def link_files():
    """Link files.
    """
    cmd = [config["CC"], config["cflags"], config["libflags"]]
    cmd.extend(config["objs"])
    cmd.append("-o")
    dst_path = add_prefix(config["dst_name"], "build_dir")
    cmd.append(dst_path)
    cmd = " ".join(cmd)
    exec_cmd(cmd)

def link_testcase(testcase):
    """Link testcase.
    """
    dst_prefix = ""
    related_objs = []
    for related_file in testcase["related_files"]:
        related_objs.append(config["testobjs"][related_file])
        if "Test" in related_file:
            dst_prefix = config["testobjs"][related_file]
            dst_prefix = dst_prefix[:dst_prefix.rfind("/")+1]

    dst_path = dst_prefix + testcase["dst_name"]


    cmd = [config["CC"], config["cflags"]]
    cmd.extend(related_objs)
    cmd.append(config["libflags"])
    cmd.append("-o")
    cmd.append(dst_path)
    cmd = " ".join(cmd)
    exec_cmd(cmd)


def link_testcases():
    for test_case in config["testcases"]:
        link_testcase(test_case)


def add_prefix(path, key):
    return config[key] + "/" + path


def get_file_name(path):
    """Get file name in path.
    """
    return path[path.rfind("/")+1:]


def filter_and_compile_test_cases(path):
    """Walk and compile testcases related files.
    """
    abs_path = add_prefix(path, "pro_dir") 
    # print(abs_path)
    if os.path.isfile(abs_path):
        # compile
        file_name = get_file_name(path)
        if file_name  in src_files: 
            dst_path = add_prefix(path, "build_dir") + ".o"
            if file_name in compile_files:
                compile_file(abs_path, dst_path)
            config["testobjs"][file_name] = dst_path
    else:
        # dir
        build_dir = add_prefix(path, "build_dir") 
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)
            print("MK dir: " + build_dir)
        # process child
        for child in os.listdir(abs_path):
            filter_and_compile_test_cases(path+"/"+child)



def walk_src(path):
    """Walk and compile src files.
    """
    abs_path = add_prefix(path, "pro_dir") 
    # print(abs_path)
    if os.path.isfile(abs_path):
        # compile
        dst_path = add_prefix(path, "build_dir") + ".o"
        compile_file(abs_path, dst_path)
        config["objs"].append(dst_path)
    else:
        # dir
        build_dir = add_prefix(path, "build_dir") 
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)
            print("MK dir: " + build_dir)
        # process child
        for child in os.listdir(abs_path):
            walk_src(path+"/"+child)


def test_exec():
    cmd = ["ls", config["build_dir"]]
    output = exec_cmd(cmd)
    if output:
        print(output)

    print("Done.")


def test_walk_src():
    src_dir = config["src_dir"]
    walk_src(src_dir)

def test_filter_test_cases():
    src_dir = config["src_dir"]
    global src_files
    global compile_files
    src_files = set()
    compile_files = set()

    for testcase in config["testcases"]:
        for c_file in testcase["compile_files"]:
            compile_files.add(c_file)
        for src_file in testcase["related_files"]:
            src_files.add(src_file)
    print(src_files) 
    print(compile_files)
    filter_and_compile_test_cases(src_dir)
    if config["linked"] == "ON":
        link_testcases()


def run():
    init()
    # test_exec()
    # test_walk_src()
    # link_files()
    test_filter_test_cases()


if __name__ == "__main__":
    run()    

