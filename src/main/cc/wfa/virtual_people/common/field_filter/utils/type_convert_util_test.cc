// Copyright 2021 The Cross-Media Measurement Authors
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

#include "wfa/virtual_people/common/field_filter/utils/type_convert_util.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "gmock/gmock.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "gtest/gtest.h"
#include "src/main/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"

namespace wfa_virtual_people {
namespace {

using ::wfa_virtual_people::test::TestProtoB;

TEST(ConvertToNumericTest, TestInt32) {
  auto output = ConvertToNumeric<int32_t>("1");
  EXPECT_TRUE(output.ok());
  EXPECT_EQ(output.value(), 1);
}

TEST(ConvertToNumericTest, TestInt32Invalid) {
  auto output = ConvertToNumeric<int32_t>("a");
  EXPECT_EQ(output.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST(ConvertToNumericTest, TestInt64) {
  auto output = ConvertToNumeric<int64_t>("1");
  EXPECT_TRUE(output.ok());
  EXPECT_EQ(output.value(), 1);
}

TEST(ConvertToNumericTest, TestInt64Invalid) {
  auto output = ConvertToNumeric<int64_t>("a");
  EXPECT_EQ(output.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST(ConvertToNumericTest, TestUInt32) {
  auto output = ConvertToNumeric<uint32_t>("1");
  EXPECT_TRUE(output.ok());
  EXPECT_EQ(output.value(), 1);
}

TEST(ConvertToNumericTest, TestUInt32Invalid) {
  auto output = ConvertToNumeric<uint32_t>("a");
  EXPECT_EQ(output.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST(ConvertToNumericTest, TestUInt64) {
  auto output = ConvertToNumeric<uint64_t>("1");
  EXPECT_TRUE(output.ok());
  EXPECT_EQ(output.value(), 1);
}

TEST(ConvertToNumericTest, TestUInt64Invalid) {
  auto output = ConvertToNumeric<uint64_t>("a");
  EXPECT_EQ(output.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST(ConvertToNumericTest, TestFloat) {
  auto output = ConvertToNumeric<float>("1.1");
  EXPECT_TRUE(output.ok());
  EXPECT_EQ(output.value(), 1.1f);
}

TEST(ConvertToNumericTest, TestFloatInvalid) {
  auto output = ConvertToNumeric<float>("a");
  EXPECT_EQ(output.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST(ConvertToNumericTest, TestDouble) {
  auto output = ConvertToNumeric<double>("1.1");
  EXPECT_TRUE(output.ok());
  EXPECT_EQ(output.value(), 1.1);
}

TEST(ConvertToNumericTest, TestDoubleInvalid) {
  auto output = ConvertToNumeric<double>("a");
  EXPECT_EQ(output.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST(ConvertToNumericTest, TestBool) {
  std::vector<std::string> true_strings = {"true", "t", "yes", "y", "1"};
  for (const std::string& s : true_strings) {
    auto output = ConvertToNumeric<bool>(s);
    EXPECT_TRUE(output.ok());
    EXPECT_TRUE(output.value());
  }

  std::vector<std::string> false_strings = {"false", "f", "no", "n", "0"};
  for (const std::string& s : false_strings) {
    auto output = ConvertToNumeric<bool>(s);
    EXPECT_TRUE(output.ok());
    EXPECT_FALSE(output.value());
  }
}

TEST(ConvertToNumericTest, TestBoolInvalid) {
  auto output = ConvertToNumeric<bool>("a");
  EXPECT_EQ(output.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST(ConvertToEnumTest, TestEnumName) {
  const google::protobuf::FieldDescriptor* field_descriptor =
      TestProtoB().GetDescriptor()->FindFieldByName("enum_value");
  auto output = ConvertToEnum(field_descriptor, "TEST_ENUM_1");
  EXPECT_TRUE(output.ok());
  EXPECT_EQ(output.value()->number(), 1);
}

TEST(ConvertToEnumTest, TestEnumNumber) {
  const google::protobuf::FieldDescriptor* field_descriptor =
      TestProtoB().GetDescriptor()->FindFieldByName("enum_value");
  auto output = ConvertToEnum(field_descriptor, "1");
  EXPECT_TRUE(output.ok());
  EXPECT_EQ(output.value()->number(), 1);
}

TEST(ConvertToEnumTest, TestEnumInvalid) {
  const google::protobuf::FieldDescriptor* field_descriptor =
      TestProtoB().GetDescriptor()->FindFieldByName("enum_value");
  auto output = ConvertToEnum(field_descriptor, "a");
  EXPECT_EQ(output.status().code(), absl::StatusCode::kInvalidArgument);
}

}  // namespace
}  // namespace wfa_virtual_people
