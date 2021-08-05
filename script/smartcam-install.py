#!/usr/bin/python3
#
# Copyright 2021 Xilinx Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import argparse
import pathlib
import os
import sys
import shutil

def main(appname):
    note_dir="/opt/xilinx/share/notebooks/"+appname
    parser = argparse.ArgumentParser(prog=appname+'-install',
                                 formatter_class=argparse.RawDescriptionHelpFormatter,
                                 description='Script to copy {} Jupyter notebook to user directory'.format(appname))
    parser.add_argument('-d', '--dir', type=pathlib.Path, help='Install the Jupyter notebook to the specified directory.', default=os.path.join("/home/petalinux/notebooks", "./{}".format(appname)) )
    parser.add_argument('-f', '--force', action='store_true', help='Force to install the Jupyter notebook even if the destination directory exists.')
    args = parser.parse_args()
    destdir = os.path.abspath(args.dir)

    if os.path.exists(destdir):
        if os.path.isfile(destdir):
                sys.exit("Error: Destination directory {} is an existing file.".format(destdir))
        else:
            if not args.force:
                sys.exit("Error: Destination directory {} already exists, please use another dirname or use '--force' option.".format(destdir))
            else:
                print("Info: Destination directory "+destdir+" already exists, force removing it.")
                shutil.rmtree(destdir)

    ignore = shutil.ignore_patterns('.ipynb_checkpoints','*.pyc','*~')
    srcdir = os.path.abspath(note_dir)

    if os.path.isdir(srcdir):
        try:
            shutil.copytree(srcdir, destdir, ignore=ignore, symlinks=True)
            print("Info: Notebook files under {} are copied to {}.".format(srcdir, destdir))
        except BaseException as error:
            sys.exit("Error: An exception occurred: {}".format(error))
    else:
        sys.exit("Error: Predefined system notebook directory %s doesn't exist." % srcdir)

if __name__ == "__main__":
    main("smartcam")
