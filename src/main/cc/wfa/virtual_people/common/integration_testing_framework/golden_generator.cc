// Copyright 2022 The Cross-Media Measurement Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "wfa/virtual_people/common/integration_testing_framework/golden_generator.h"

#include <string>
#include <vector>

#include "tools/cpp/runfiles/runfiles.h"
#include "wfa/virtual_people/common/config.pb.h"

using bazel::tools::cpp::runfiles::Runfiles;

namespace wfa_virtual_people {

// Generates CLI commands to generate golden files of binaries based on input
// config.
std::vector<std::string> GoldenGenerator(const IntegrationTestList config) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  std::vector<std::string> executeVector;
  std::string execute;

  for (int testIndex = 0; testIndex < config.tests_size(); testIndex++) {
    IntegrationTest it = config.tests().at(testIndex);
    std::string binaryPath = runfiles->Rlocation(it.binary());
    for (int testCaseIndex = 0; testCaseIndex < it.test_cases_size();
         testCaseIndex++) {
      TestCase tc = it.test_cases().at(testCaseIndex);
      execute = binaryPath;
      for (int binaryParameterIndex = 0;
           binaryParameterIndex < tc.binary_parameters_size();
           binaryParameterIndex++) {
        BinaryParameter bp = tc.binary_parameters().at(binaryParameterIndex);
        execute += " --" + bp.name() + "=";
        if (bp.golden().empty()) {
          execute += bp.value();
        } else {
          execute += bp.golden();
        }
      }
      executeVector.push_back(execute);
    }
  }

  return executeVector;
}

}  // namespace wfa_virtual_people
