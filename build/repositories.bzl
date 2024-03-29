# Copyright 2022 The Cross-Media Measurement Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Adds external repos necessary for virtual_people_common.
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def virtual_people_common_repositories():
    """Imports all direct dependencies for virtual_people_common."""
    http_archive(
        name = "wfa_common_cpp",
        sha256 = "60e9c808d55d14be65347cab008b8bd4f8e2dd8186141609995333bc75fc08ce",
        strip_prefix = "common-cpp-0.8.0",
        url = "https://github.com/world-federation-of-advertisers/common-cpp/archive/refs/tags/v0.8.0.tar.gz",
    )

    http_archive(
        name = "wfa_common_jvm",
        sha256 = "2719f6147eccd2a493e573720c64fa3a13f344840f8416060753d97fe13ab942",
        strip_prefix = "common-jvm-0.66.0",
        url = "https://github.com/world-federation-of-advertisers/common-jvm/archive/refs/tags/v0.66.0.tar.gz",
    )
