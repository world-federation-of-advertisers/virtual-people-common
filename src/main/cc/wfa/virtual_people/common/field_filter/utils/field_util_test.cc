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

#include "wfa/virtual_people/common/field_filter/utils/field_util.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "gmock/gmock.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "google/protobuf/text_format.h"
#include "gtest/gtest.h"
#include "src/main/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"
#include "src/test/cc/testutil/matchers.h"
#include "src/test/cc/testutil/status_macros.h"

namespace wfa_virtual_people {
namespace {

using ::google::protobuf::FieldDescriptor;
using ::google::protobuf::Message;
using ::wfa::EqualsProto;
using ::wfa::IsOk;
using ::wfa::StatusIs;
using ::wfa_virtual_people::test::TestProto;

TEST(GetFieldFromProtoTest, GetFieldAndValue) {
  TestProto test_proto_1, test_proto_2;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"pb(
      a {
        b {
          int32_value: 1
          int64_value: 1
          uint32_value: 1
          uint64_value: 1
          float_value: 1.0
          double_value: 1.0
          bool_value: true
          enum_value: TEST_ENUM_1
          string_value: "string1"
        }
      }
  )pb", &test_proto_1));
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"pb(
      a {
        b {
          int32_value: 2
          int64_value: 2
          uint32_value: 2
          uint64_value: 2
          float_value: 2.0
          double_value: 2.0
          bool_value: false
          enum_value: TEST_ENUM_2
          string_value: "string2"
        }
      }
  )pb", &test_proto_2));
  // Test int32.
  absl::StatusOr<std::vector<const google::protobuf::FieldDescriptor*>>
        field_descriptors =
        GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b.int32_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<int32_t>(test_proto_2, *field_descriptors), 2);
  // Test int64.
  field_descriptors = GetFieldFromProto(
      test_proto_1.GetDescriptor(), "a.b.int64_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<int64_t>(test_proto_2, *field_descriptors), 2);
  // Test uint32.
  field_descriptors = GetFieldFromProto(
      test_proto_1.GetDescriptor(), "a.b.uint32_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<uint32_t>(test_proto_2, *field_descriptors), 2);
  // Test uint64.
  field_descriptors = GetFieldFromProto(
      test_proto_1.GetDescriptor(), "a.b.uint64_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<uint64_t>(test_proto_2, *field_descriptors), 2);
  // Test float.
  field_descriptors = GetFieldFromProto(
      test_proto_1.GetDescriptor(), "a.b.float_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<float>(test_proto_2, *field_descriptors), 2.0);
  // Test double.
  field_descriptors = GetFieldFromProto(
      test_proto_1.GetDescriptor(), "a.b.double_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<double>(test_proto_2, *field_descriptors), 2.0);
  // Test bool.
  field_descriptors = GetFieldFromProto(
      test_proto_1.GetDescriptor(), "a.b.bool_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_FALSE(GetValueFromProto<bool>(test_proto_2, *field_descriptors));
  // Test enum.
  field_descriptors = GetFieldFromProto(
      test_proto_1.GetDescriptor(), "a.b.enum_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(
      GetValueFromProto<const google::protobuf::EnumValueDescriptor*>(
          test_proto_2, *field_descriptors)->number(), 2);
  // Test string.
  field_descriptors = GetFieldFromProto(
      test_proto_1.GetDescriptor(), "a.b.string_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(
      GetValueFromProto<const std::string&>(test_proto_2, *field_descriptors),
      "string2");
  // Test Message.
  field_descriptors = GetFieldFromProto(
      test_proto_1.GetDescriptor(), "a.b");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<const google::protobuf::Message&>(
          test_proto_2, *field_descriptors),
      EqualsProto(test_proto_2.a().b()));
}

TEST(GetFieldFromProtoTest, GetValueForUnsetField) {
  TestProto test_proto;
  // Test int32.
  absl::StatusOr<std::vector<const google::protobuf::FieldDescriptor*>>
        field_descriptors =
        GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<int32_t>(test_proto, *field_descriptors), 0);
  // Test int64.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int64_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<int64_t>(test_proto, *field_descriptors), 0);
  // Test uint32.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint32_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<uint32_t>(test_proto, *field_descriptors), 0);
  // Test uint64.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint64_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<uint64_t>(test_proto, *field_descriptors), 0);
  // Test float.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.float_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<float>(test_proto, *field_descriptors), 0);
  // Test double.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.double_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(GetValueFromProto<double>(test_proto, *field_descriptors), 0);
  // Test bool.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.bool_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_FALSE(GetValueFromProto<bool>(test_proto, *field_descriptors));
  // Test enum.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.enum_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(
      GetValueFromProto<const google::protobuf::EnumValueDescriptor*>(
          test_proto, *field_descriptors)->number(), 0);
  // Test string.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.string_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_EQ(
      GetValueFromProto<const std::string&>(test_proto, *field_descriptors),
      "");
  // Test Message.
  field_descriptors = GetFieldFromProto(TestProto().GetDescriptor(), "a.b");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<const google::protobuf::Message&>(
          test_proto, *field_descriptors),
      EqualsProto(test_proto.a().b()));
}

TEST(GetFieldFromProtoTest, InvalidFieldName) {
  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"pb(
      a {
        b {
          int64_value: 1
        }
      }
  )pb", &test_proto));
  absl::StatusOr<std::vector<const google::protobuf::FieldDescriptor*>>
        field_descriptors =
        GetFieldFromProto(test_proto.GetDescriptor(), "a.c");
  EXPECT_THAT(
      field_descriptors.status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(GetFieldFromProtoTest, InvalidSubmessageName) {
  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"pb(
      a {
        b {
          int64_value: 1
        }
      }
  )pb", &test_proto));
  absl::StatusOr<std::vector<const google::protobuf::FieldDescriptor*>>
        field_descriptors =
        GetFieldFromProto(test_proto.GetDescriptor(), "a.b.int64_value.c");
  EXPECT_THAT(
      field_descriptors.status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(GetFieldFromProtoTest, TestGetParentMessage) {
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value"));

  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"pb(
      a {
        b {
          int32_value: 1
          int64_value: 1
          uint32_value: 1
          uint64_value: 1
          float_value: 1.0
          double_value: 1.0
          bool_value: true
          enum_value: TEST_ENUM_1
          string_value: "string1"
          int32_values: 1
          int32_values: 2
        }
      }
  )pb", &test_proto));
  EXPECT_THAT(
      GetParentMessageFromProto(test_proto, field_descriptors),
      EqualsProto(test_proto.a().b()));
}

TEST(GetFieldFromProtoTest, DisallowRepeatedField) {
  // Any repeated field in the path except the last field is disallowed.
  EXPECT_THAT(
      GetFieldFromProto(
          TestProto().GetDescriptor(),
          "repeated_proto_a.b.int32_value").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Any repeated field in the path except the last field is disallowed.
  EXPECT_THAT(
      GetFieldFromProto(
          TestProto().GetDescriptor(), "repeated_proto_a.b.int32_value",
          /* allow_repeated = */true).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Last field is disallowed to be repeated if @allow_repeated is set to false.
  EXPECT_THAT(
      GetFieldFromProto(
          TestProto().GetDescriptor(), "a.b.int32_values").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(GetFieldFromProtoTest, TestAllowRepeatedAndGetParentMessage) {
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(
          TestProto().GetDescriptor(), "a.b.int32_values",
          /* allow_repeated = */true));

  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"pb(
      a {
        b {
          int32_value: 1
          int64_value: 1
          uint32_value: 1
          uint64_value: 1
          float_value: 1.0
          double_value: 1.0
          bool_value: true
          enum_value: TEST_ENUM_1
          string_value: "string1"
          int32_values: 1
          int32_values: 2
        }
      }
  )pb", &test_proto));
  EXPECT_THAT(
      GetParentMessageFromProto(test_proto, field_descriptors),
      EqualsProto(test_proto.a().b()));
}

TEST(GetFieldFromProtoTest, TestGetParentMessageParentNotSet) {
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value"));

  TestProto test_proto;
  EXPECT_THAT(
      GetParentMessageFromProto(test_proto, field_descriptors),
      EqualsProto(test_proto.a().b()));
}

}  // namespace
}  // namespace wfa_virtual_people
