#
# Copyright 2020 Xilinx Inc.
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

file=${1:-"/usr/share/somapp/movies/walking-people.nv12.30fps.1080p.h264"}
w=${2:-"1920"} h=${3:-"1080"}
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source ${DIR}/setupmipi.sh

${DIR}/somapp --file ${file} --infile-type h264 --target file  --w ${w} --h ${h} > /dev/null 2>&1 &

echo "detection on file is started."
