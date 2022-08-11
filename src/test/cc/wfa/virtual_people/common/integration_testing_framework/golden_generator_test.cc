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

#include "gmock/gmock.h"
#include "google/protobuf/text_format.h"
#include "gtest/gtest.h"
#include "wfa/virtual_people/common/config.pb.h"

namespace wfa_virtual_people {
namespace {

constexpr char bazelOut[] = "bazel-out";

TEST(GoldenGeneratorTest, EmptyConfig) {
  IntegrationTestList config;
  std::vector<std::string> execute_vector(GoldenGenerator(config));

  EXPECT_THAT(execute_vector, ::testing::IsEmpty());
}

TEST(GoldenGeneratorTest, ExpectedConfig) {
  IntegrationTestList config;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        tests {
          name: "model_compiler"
          binary: "virtual_people_training/src/main/cc/wfa/virtual_people/training/model_compiler/compiler_main"
          test_cases {
            name: "population_node"
            binary_parameters {
              name: "input_path"
              value: "src/test/cc/wfa/virtual_people/training/model_compiler/test_data/model_node_config_population_node.textproto"
            }
            binary_parameters {
              name: "output_path"
              value: "src/test/cc/wfa/virtual_people/training/model_compiler/compiled_node_for_population_node.textproto"
              golden: "src/test/cc/wfa/virtual_people/training/model_compiler/test_data/compiled_node_for_population_node.textproto"
              proto: "virtual_people_common/src/main/proto/wfa/virtual_people/common/_virtual_imports/model_proto/wfa/virtual_people/common/model.proto"
              proto_dependencies: "virtual_people_common/src/main/proto/wfa/virtual_people/common/_virtual_imports/field_filter_proto/wfa/virtual_people/common/field_filter.proto"
              proto_type: "CompiledNode"
            }
          }
          test_cases {
            name: "population_node_redistribute_probabilities_for_empty_pools"
            binary_parameters {
              name: "input_path"
              value: "src/test/cc/wfa/virtual_people/training/model_compiler/test_data/model_node_config_population_node_redistribute_probabilities_for_empty_pools.textproto"
            }
            binary_parameters {
              name: "output_path"
              value: "src/test/cc/wfa/virtual_people/training/model_compiler/compiled_node_for_population_node_redistribute_probabilities_for_empty_pools.textproto"
              golden: "src/test/cc/wfa/virtual_people/training/model_compiler/test_data/compiled_node_for_population_node_redistribute_probabilities_for_empty_pools.textproto"
              proto: "virtual_people_common/src/main/proto/wfa/virtual_people/common/_virtual_imports/model_proto/wfa/virtual_people/common/model.proto"
              proto_dependencies: "virtual_people_common/src/main/proto/wfa/virtual_people/common/_virtual_imports/field_filter_proto/wfa/virtual_people/common/field_filter.proto"
              proto_type: "CompiledNode"
            }
          }
          test_cases {
            name: "population_node_kappa_less_than_one"
            binary_parameters {
              name: "input_path"
              value: "src/test/cc/wfa/virtual_people/training/model_compiler/test_data/model_node_config_population_node_kappa_less_than_one.textproto"
            }
            binary_parameters {
              name: "output_path"
              value: "src/test/cc/wfa/virtual_people/training/model_compiler/compiled_node_for_population_node_kappa_less_than_one.textproto"
              golden: "src/test/cc/wfa/virtual_people/training/model_compiler/test_data/compiled_node_for_population_node_kappa_less_than_one.textproto"
              proto: "virtual_people_common/src/main/proto/wfa/virtual_people/common/_virtual_imports/model_proto/wfa/virtual_people/common/model.proto"
              proto_dependencies: "virtual_people_common/src/main/proto/wfa/virtual_people/common/_virtual_imports/field_filter_proto/wfa/virtual_people/common/field_filter.proto"
              proto_type: "CompiledNode"
            }
          }
          test_cases {
            name: "population_node_discretization"
            binary_parameters {
              name: "input_path"
              value: "src/test/cc/wfa/virtual_people/training/model_compiler/test_data/model_node_config_population_node_discretization.textproto"
            }
            binary_parameters {
              name: "output_path"
              value: "src/test/cc/wfa/virtual_people/training/model_compiler/compiled_node_for_population_node_discretization.textproto"
              golden: "src/test/cc/wfa/virtual_people/training/model_compiler/test_data/compiled_node_for_population_node_discretization.textproto"
              proto: "virtual_people_common/src/main/proto/wfa/virtual_people/common/_virtual_imports/model_proto/wfa/virtual_people/common/model.proto"
              proto_dependencies: "virtual_people_common/src/main/proto/wfa/virtual_people/common/_virtual_imports/field_filter_proto/wfa/virtual_people/common/field_filter.proto"
              proto_type: "CompiledNode"
            }
          }
        }
      )pb",
      &config));
  std::vector<std::string> execute_vector(GoldenGenerator(config));

  for (int i = 0; i < execute_vector.size(); i++) {
    int index = execute_vector[i].find(bazelOut);
    execute_vector[i].erase(0, index);
  }

  EXPECT_THAT(
      execute_vector,
      ::testing::ElementsAre(
          "bazel-out/k8-fastbuild/bin/src/test/cc/wfa/virtual_people/common/"
          "integration_testing_framework/golden_generator_test.runfiles/"
          "virtual_people_training/src/main/cc/wfa/virtual_people/training/"
          "model_compiler/compiler_main "
          "--input_path=src/test/cc/wfa/virtual_people/training/model_compiler/"
          "test_data/model_node_config_population_node.textproto "
          "--output_path=src/test/cc/wfa/virtual_people/training/"
          "model_compiler/test_data/"
          "compiled_node_for_population_node.textproto",
          "bazel-out/k8-fastbuild/bin/src/test/cc/wfa/virtual_people/common/"
          "integration_testing_framework/golden_generator_test.runfiles/"
          "virtual_people_training/src/main/cc/wfa/virtual_people/training/"
          "model_compiler/compiler_main "
          "--input_path=src/test/cc/wfa/virtual_people/training/model_compiler/"
          "test_data/"
          "model_node_config_population_node_redistribute_probabilities_for_"
          "empty_pools.textproto "
          "--output_path=src/test/cc/wfa/virtual_people/training/"
          "model_compiler/test_data/"
          "compiled_node_for_population_node_redistribute_probabilities_for_"
          "empty_pools.textproto",
          "bazel-out/k8-fastbuild/bin/src/test/cc/wfa/virtual_people/common/"
          "integration_testing_framework/golden_generator_test.runfiles/"
          "virtual_people_training/src/main/cc/wfa/virtual_people/training/"
          "model_compiler/compiler_main "
          "--input_path=src/test/cc/wfa/virtual_people/training/model_compiler/"
          "test_data/"
          "model_node_config_population_node_kappa_less_than_one.textproto "
          "--output_path=src/test/cc/wfa/virtual_people/training/"
          "model_compiler/test_data/"
          "compiled_node_for_population_node_kappa_less_than_one.textproto",
          "bazel-out/k8-fastbuild/bin/src/test/cc/wfa/virtual_people/common/"
          "integration_testing_framework/golden_generator_test.runfiles/"
          "virtual_people_training/src/main/cc/wfa/virtual_people/training/"
          "model_compiler/compiler_main "
          "--input_path=src/test/cc/wfa/virtual_people/training/model_compiler/"
          "test_data/"
          "model_node_config_population_node_discretization.textproto "
          "--output_path=src/test/cc/wfa/virtual_people/training/"
          "model_compiler/test_data/"
          "compiled_node_for_population_node_discretization.textproto"));
}

}  // namespace
}  // namespace wfa_virtual_people
