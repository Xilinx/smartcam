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

for media in $(ls /dev/media*); do
	if [[ $(media-ctl -d ${media} -p | grep driver | grep xilinx-video | wc -l) -eq 1 ]]; then
		echo "xilinx mipi:" ${media}
		video=$(media-ctl -p -d ${media} | grep "/dev/video"| sed 's/.*\/dev\/video\(.\)/\1/')
		findmipi=1
	fi
done

if [[ ${findmipi} -ne 1 ]]; then
	echo "not find mipi"
fi

media=${media/\/dev\/media/}
