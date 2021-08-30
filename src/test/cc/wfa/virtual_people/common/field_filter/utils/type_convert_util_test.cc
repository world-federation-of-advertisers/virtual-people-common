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
#include "common_cpp/testing/status_matchers.h"
#include "gmock/gmock.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "gtest/gtest.h"
#include "wfa/virtual_people/common/field_filter/test/test.pb.h"

namespace wfa_virtual_people {
namespace {

using ::wfa::IsOk;
using ::wfa::IsOkAndHolds;
using ::wfa::StatusIs;
using ::wfa_virtual_people::test::TestProtoB;

TEST(ConvertToNumericTest, TestInt32) {
  EXPECT_THAT(ConvertToNumeric<int32_t>("1"), IsOkAndHolds(1));
}

TEST(ConvertToNumericTest, TestInt32Invalid) {
  EXPECT_THAT(ConvertToNumeric<int32_t>("a").status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ConvertToNumericTest, TestInt64) {
  EXPECT_THAT(ConvertToNumeric<int64_t>("1"), IsOkAndHolds(1));
}

TEST(ConvertToNumericTest, TestInt64Invalid) {
  EXPECT_THAT(ConvertToNumeric<int64_t>("a").status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ConvertToNumericTest, TestUInt32) {
  EXPECT_THAT(ConvertToNumeric<uint32_t>("1"), IsOkAndHolds(1));
}

TEST(ConvertToNumericTest, TestUInt32Invalid) {
  EXPECT_THAT(ConvertToNumeric<uint32_t>("a").status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ConvertToNumericTest, TestUInt64) {
  EXPECT_THAT(ConvertToNumeric<uint64_t>("1"), IsOkAndHolds(1));
}

TEST(ConvertToNumericTest, TestUInt64Invalid) {
  EXPECT_THAT(ConvertToNumeric<uint64_t>("a").status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ConvertToNumericTest, TestFloat) {
  EXPECT_THAT(ConvertToNumeric<float>("1.1"), IsOkAndHolds(1.1f));
}

TEST(ConvertToNumericTest, TestFloatInvalid) {
  EXPECT_THAT(ConvertToNumeric<float>("a").status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ConvertToNumericTest, TestDouble) {
  EXPECT_THAT(ConvertToNumeric<double>("1.1"), IsOkAndHolds(1.1));
}

TEST(ConvertToNumericTest, TestDoubleInvalid) {
  EXPECT_THAT(ConvertToNumeric<double>("a").status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ConvertToNumericTest, TestBool) {
  std::vector<std::string> true_strings = {"true", "t", "yes", "y", "1"};
  for (const std::string& s : true_strings) {
    EXPECT_THAT(ConvertToNumeric<bool>(s), IsOkAndHolds(true));
  }

  std::vector<std::string> false_strings = {"false", "f", "no", "n", "0"};
  for (const std::string& s : false_strings) {
    EXPECT_THAT(ConvertToNumeric<bool>(s), IsOkAndHolds(false));
  }
}

TEST(ConvertToNumericTest, TestBoolInvalid) {
  EXPECT_THAT(ConvertToNumeric<bool>("a").status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ConvertToEnumTest, TestEnumName) {
  const google::protobuf::FieldDescriptor* field_descriptor =
      TestProtoB().GetDescriptor()->FindFieldByName("enum_value");
  auto output = ConvertToEnum(field_descriptor->enum_type(), "TEST_ENUM_1");
  EXPECT_THAT(output, IsOk());
  EXPECT_EQ((*output)->number(), 1);
}

TEST(ConvertToEnumTest, TestEnumNumber) {
  const google::protobuf::FieldDescriptor* field_descriptor =
      TestProtoB().GetDescriptor()->FindFieldByName("enum_value");
  auto output = ConvertToEnum(field_descriptor->enum_type(), "1");
  EXPECT_THAT(output, IsOk());
  EXPECT_EQ((*output)->number(), 1);
}

TEST(ConvertToEnumTest, TestEnumInvalid) {
  const google::protobuf::FieldDescriptor* field_descriptor =
      TestProtoB().GetDescriptor()->FindFieldByName("enum_value");
  EXPECT_THAT(ConvertToEnum(field_descriptor->enum_type(), "a").status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

}  // namespace
}  // namespace wfa_virtual_people
