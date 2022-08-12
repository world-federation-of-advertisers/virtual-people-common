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

#include <memory>
#include <string>
#include <vector>

#include "absl/strings/str_cat.h"
#include "tools/cpp/runfiles/runfiles.h"
#include "wfa/virtual_people/common/config.pb.h"

using bazel::tools::cpp::runfiles::Runfiles;

namespace wfa_virtual_people {

// Generates CLI commands to generate golden files of binaries based on input
// config.
std::vector<std::string> GoldenGenerator(const IntegrationTestList& config) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  std::vector<std::string> execute_vector;
  std::string execute;

  for (int test_index = 0; test_index < config.tests_size(); test_index++) {
    const IntegrationTest& it = config.tests().at(test_index);
    std::string binaryPath = runfiles->Rlocation(it.binary());
    for (int test_case_index = 0; test_case_index < it.test_cases_size();
         test_case_index++) {
      const TestCase& tc = it.test_cases().at(test_case_index);
      execute = binaryPath;
      for (int binary_parameter_index = 0;
           binary_parameter_index < tc.binary_parameters_size();
           binary_parameter_index++) {
        BinaryParameter bp = tc.binary_parameters().at(binary_parameter_index);
        execute = absl::StrCat(execute, " --", bp.name(), "=");
        if (bp.golden().golden_path().empty()) {
          absl::StrAppend(&execute, bp.value());
        } else {
          absl::StrAppend(&execute, bp.golden().golden_path());
        }
      }
      execute_vector.push_back(execute);
    }
  }

  return execute_vector;
}

}  // namespace wfa_virtual_people
