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
#include "common_cpp/testing/common_matchers.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gmock/gmock.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "google/protobuf/text_format.h"
#include "gtest/gtest.h"
#include "src/test/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"

namespace wfa_virtual_people {
namespace {

using ::google::protobuf::FieldDescriptor;
using ::google::protobuf::Message;
using ::wfa::EqualsProto;
using ::wfa::IsOk;
using ::wfa::IsOkAndHolds;
using ::wfa::StatusIs;
using ::wfa_virtual_people::test::TestProto;
using ::wfa_virtual_people::test::TestProtoB;

TEST(FieldUtilTest, GetFieldAndValue) {
  TestProto test_proto_1, test_proto_2;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
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
      )pb",
      &test_proto_1));
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
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
      )pb",
      &test_proto_2));
  // Test int32.
  absl::StatusOr<std::vector<const google::protobuf::FieldDescriptor*>>
      field_descriptors =
          GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b.int32_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(GetValueFromProto<int32_t>(test_proto_2, *field_descriptors),
              IsOkAndHolds(2));
  // Test int64.
  field_descriptors =
      GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b.int64_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(GetValueFromProto<int64_t>(test_proto_2, *field_descriptors),
              IsOkAndHolds(2));
  // Test uint32.
  field_descriptors =
      GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b.uint32_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(GetValueFromProto<uint32_t>(test_proto_2, *field_descriptors),
              IsOkAndHolds(2));
  // Test uint64.
  field_descriptors =
      GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b.uint64_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(GetValueFromProto<uint64_t>(test_proto_2, *field_descriptors),
              IsOkAndHolds(2));
  // Test float.
  field_descriptors =
      GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b.float_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(GetValueFromProto<float>(test_proto_2, *field_descriptors),
              IsOkAndHolds(2.0));
  // Test double.
  field_descriptors =
      GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b.double_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(GetValueFromProto<double>(test_proto_2, *field_descriptors),
              IsOkAndHolds(2.0));
  // Test bool.
  field_descriptors =
      GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b.bool_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(GetValueFromProto<bool>(test_proto_2, *field_descriptors),
              IsOkAndHolds(false));
  // Test enum.
  field_descriptors =
      GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b.enum_value");
  EXPECT_THAT(field_descriptors, IsOk());
  absl::StatusOr<const google::protobuf::EnumValueDescriptor*> enum_value =
      GetValueFromProto<const google::protobuf::EnumValueDescriptor*>(
          test_proto_2, *field_descriptors);
  EXPECT_THAT(enum_value.status(), IsOk());
  EXPECT_EQ((*enum_value)->number(), 2);
  // Test string.
  field_descriptors =
      GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b.string_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<const std::string&>(test_proto_2, *field_descriptors),
      IsOkAndHolds("string2"));
  // Test Message.
  field_descriptors = GetFieldFromProto(test_proto_1.GetDescriptor(), "a.b");
  EXPECT_THAT(field_descriptors, IsOk());
  absl::StatusOr<const google::protobuf::Message&> message_value =
      GetValueFromProto<const google::protobuf::Message&>(
          test_proto_2, *field_descriptors);
  EXPECT_THAT(message_value.status(), IsOk());
  EXPECT_THAT(message_value, EqualsProto(test_proto_2.a().b()));
}

TEST(FieldUtilTest, GetValueForUnsetField) {
  TestProto test_proto;
  // Test int32.
  absl::StatusOr<std::vector<const google::protobuf::FieldDescriptor*>>
      field_descriptors =
          GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<int32_t>(test_proto, *field_descriptors).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Test int64.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int64_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<int64_t>(test_proto, *field_descriptors).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Test uint32.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint32_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<uint32_t>(test_proto, *field_descriptors).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Test uint64.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint64_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<uint64_t>(test_proto, *field_descriptors).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Test float.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.float_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<float>(test_proto, *field_descriptors).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Test double.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.double_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<double>(test_proto, *field_descriptors).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Test bool.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.bool_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<bool>(test_proto, *field_descriptors).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Test enum.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.enum_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<const google::protobuf::EnumValueDescriptor*>(
          test_proto, *field_descriptors).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Test string.
  field_descriptors =
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.string_value");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<const std::string&>(test_proto, *field_descriptors).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Test Message.
  field_descriptors = GetFieldFromProto(TestProto().GetDescriptor(), "a.b");
  EXPECT_THAT(field_descriptors, IsOk());
  EXPECT_THAT(
      GetValueFromProto<const google::protobuf::Message&>(
          test_proto, *field_descriptors).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(FieldUtilTest, GetFieldAndSetValue) {
  TestProtoB test_b_proto;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      int32_value: 1
                                                      int64_value: 1
                                                      uint32_value: 1
                                                      uint64_value: 1
                                                      float_value: 1.0
                                                      double_value: 1.0
                                                      bool_value: true
                                                      enum_value: TEST_ENUM_1
                                                      string_value: "string1"
                                                    )pb",
                                                    &test_b_proto));
  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors;
  // Test int32.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProtoB().GetDescriptor(), "int32_value"));
  SetValueToProto<int32_t>(test_b_proto, field_descriptors, 2);
  // Test int64.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProtoB().GetDescriptor(), "int64_value"));
  SetValueToProto<int64_t>(test_b_proto, field_descriptors, 2);
  // Test uint32.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProtoB().GetDescriptor(), "uint32_value"));
  SetValueToProto<uint32_t>(test_b_proto, field_descriptors, 2);
  // Test uint64.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProtoB().GetDescriptor(), "uint64_value"));
  SetValueToProto<uint64_t>(test_b_proto, field_descriptors, 2);
  // Test float.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProtoB().GetDescriptor(), "float_value"));
  SetValueToProto<float>(test_b_proto, field_descriptors, 2.0);
  // Test double.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProtoB().GetDescriptor(), "double_value"));
  SetValueToProto<double>(test_b_proto, field_descriptors, 2.0);
  // Test bool.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProtoB().GetDescriptor(), "bool_value"));
  SetValueToProto<bool>(test_b_proto, field_descriptors, false);
  // Test enum.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProtoB().GetDescriptor(), "enum_value"));
  SetValueToProto<const google::protobuf::EnumValueDescriptor*>(
      test_b_proto, field_descriptors,
      field_descriptors.back()->enum_type()->FindValueByNumber(2));
  // Test string.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProtoB().GetDescriptor(), "string_value"));
  SetValueToProto<const std::string&>(test_b_proto, field_descriptors,
                                      "string2");

  TestProtoB expected_test_b_proto;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      int32_value: 2
                                                      int64_value: 2
                                                      uint32_value: 2
                                                      uint64_value: 2
                                                      float_value: 2.0
                                                      double_value: 2.0
                                                      bool_value: false
                                                      enum_value: TEST_ENUM_2
                                                      string_value: "string2"
                                                    )pb",
                                                    &expected_test_b_proto));
  EXPECT_THAT(test_b_proto, EqualsProto(expected_test_b_proto));
}

TEST(FieldUtilTest, GetFieldAndSetNestedValue) {
  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
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
      )pb",
      &test_proto));
  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors;
  // Test int32.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value"));
  SetValueToProto<int32_t>(test_proto, field_descriptors, 2);
  // Test int64.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int64_value"));
  SetValueToProto<int64_t>(test_proto, field_descriptors, 2);
  // Test uint32.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint32_value"));
  SetValueToProto<uint32_t>(test_proto, field_descriptors, 2);
  // Test uint64.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint64_value"));
  SetValueToProto<uint64_t>(test_proto, field_descriptors, 2);
  // Test float.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.float_value"));
  SetValueToProto<float>(test_proto, field_descriptors, 2.0);
  // Test double.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.double_value"));
  SetValueToProto<double>(test_proto, field_descriptors, 2.0);
  // Test bool.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.bool_value"));
  SetValueToProto<bool>(test_proto, field_descriptors, false);
  // Test enum.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.enum_value"));
  SetValueToProto<const google::protobuf::EnumValueDescriptor*>(
      test_proto, field_descriptors,
      field_descriptors.back()->enum_type()->FindValueByNumber(2));
  // Test string.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.string_value"));
  SetValueToProto<const std::string&>(test_proto, field_descriptors, "string2");

  TestProto expected_test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
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
      )pb",
      &expected_test_proto));
  EXPECT_THAT(test_proto, EqualsProto(expected_test_proto));
}

TEST(FieldUtilTest, GetFieldAndSetValueForUnsetParentMessage) {
  TestProto test_proto;
  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors;
  // Test int32.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value"));
  SetValueToProto<int32_t>(test_proto, field_descriptors, 2);
  // Test int64.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int64_value"));
  SetValueToProto<int64_t>(test_proto, field_descriptors, 2);
  // Test uint32.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint32_value"));
  SetValueToProto<uint32_t>(test_proto, field_descriptors, 2);
  // Test uint64.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint64_value"));
  SetValueToProto<uint64_t>(test_proto, field_descriptors, 2);
  // Test float.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.float_value"));
  SetValueToProto<float>(test_proto, field_descriptors, 2.0);
  // Test double.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.double_value"));
  SetValueToProto<double>(test_proto, field_descriptors, 2.0);
  // Test bool.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.bool_value"));
  SetValueToProto<bool>(test_proto, field_descriptors, false);
  // Test enum.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.enum_value"));
  SetValueToProto<const google::protobuf::EnumValueDescriptor*>(
      test_proto, field_descriptors,
      field_descriptors.back()->enum_type()->FindValueByNumber(2));
  // Test string.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.string_value"));
  SetValueToProto<const std::string&>(test_proto, field_descriptors, "string2");

  TestProto expected_test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
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
      )pb",
      &expected_test_proto));
  EXPECT_THAT(test_proto, EqualsProto(expected_test_proto));
}

TEST(FieldUtilTest, InvalidFieldName) {
  TestProto test_proto;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      a { b { int64_value: 1 } }
                                                    )pb",
                                                    &test_proto));
  absl::StatusOr<std::vector<const google::protobuf::FieldDescriptor*>>
      field_descriptors = GetFieldFromProto(test_proto.GetDescriptor(), "a.c");
  EXPECT_THAT(field_descriptors.status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(FieldUtilTest, InvalidSubmessageName) {
  TestProto test_proto;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      a { b { int64_value: 1 } }
                                                    )pb",
                                                    &test_proto));
  absl::StatusOr<std::vector<const google::protobuf::FieldDescriptor*>>
      field_descriptors =
          GetFieldFromProto(test_proto.GetDescriptor(), "a.b.int64_value.c");
  EXPECT_THAT(field_descriptors.status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(FieldUtilTest, TestGetParentMessage) {
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value"));

  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
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
      )pb",
      &test_proto));
  EXPECT_THAT(GetParentMessageFromProto(test_proto, field_descriptors),
              EqualsProto(test_proto.a().b()));
}

TEST(FieldUtilTest, DisallowRepeatedField) {
  // Any repeated field in the path except the last field is disallowed.
  EXPECT_THAT(GetFieldFromProto(TestProto().GetDescriptor(),
                                "repeated_proto_a.b.int32_value")
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Any repeated field in the path except the last field is disallowed.
  EXPECT_THAT(GetFieldFromProto(TestProto().GetDescriptor(),
                                "repeated_proto_a.b.int32_value",
                                /* allow_repeated = */ true)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
  // Last field is disallowed to be repeated if @allow_repeated is set to false.
  EXPECT_THAT(GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_values")
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(FieldUtilTest, TestAllowRepeatedAndGetParentMessage) {
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_values",
                        /* allow_repeated = */ true));

  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
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
      )pb",
      &test_proto));
  EXPECT_THAT(GetParentMessageFromProto(test_proto, field_descriptors),
              EqualsProto(test_proto.a().b()));
}

TEST(FieldUtilTest, TestGetParentMessageParentNotSet) {
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value"));

  TestProto test_proto;
  EXPECT_THAT(GetParentMessageFromProto(test_proto, field_descriptors),
              EqualsProto(test_proto.a().b()));
}

TEST(FieldUtilTest, GetSizeOfRepeatedProto) {
  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { int32_values: 1 int32_values: 1 } }
      )pb",
      &test_proto));

  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_values",
                        /*allow_repeated = */ true));
  EXPECT_EQ(GetSizeOfRepeatedProto(test_proto, field_descriptors), 2);
}

TEST(FieldUtilTest, GetSizeOfRepeatedProtoEmptyField) {
  TestProto test_proto;

  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_values",
                        /*allow_repeated = */ true));
  EXPECT_EQ(GetSizeOfRepeatedProto(test_proto, field_descriptors), 0);
}

TEST(FieldUtilTest, GetRepeatedFieldAndValue) {
  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a {
          b {
            int32_values: 1
            int32_values: 2
            int64_values: 1
            int64_values: 2
            uint32_values: 1
            uint32_values: 2
            uint64_values: 1
            uint64_values: 2
            float_values: 1.0
            float_values: 2.0
            double_values: 1.0
            double_values: 2.0
            bool_values: true
            bool_values: false
            enum_values: TEST_ENUM_1
            enum_values: TEST_ENUM_2
            string_values: "string1"
            string_values: "string2"
          }
        }
        repeated_proto_a { b { int32_value: 1 } }
        repeated_proto_a { b { int32_value: 2 } }
      )pb",
      &test_proto));

  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors;

  // Test int32.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_values",
                        /*allow_repeated = */ true));
  EXPECT_EQ(
      GetValueFromRepeatedProto<int32_t>(test_proto, field_descriptors, 1), 2);

  // Test int64.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int64_values",
                        /*allow_repeated = */ true));
  EXPECT_EQ(
      GetValueFromRepeatedProto<int64_t>(test_proto, field_descriptors, 1), 2);

  // Test uint32.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint32_values",
                        /*allow_repeated = */ true));
  EXPECT_EQ(
      GetValueFromRepeatedProto<uint32_t>(test_proto, field_descriptors, 1), 2);

  // Test uint64.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint64_values",
                        /*allow_repeated = */ true));
  EXPECT_EQ(
      GetValueFromRepeatedProto<uint64_t>(test_proto, field_descriptors, 1), 2);

  // Test float.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.float_values",
                        /*allow_repeated = */ true));
  EXPECT_EQ(GetValueFromRepeatedProto<float>(test_proto, field_descriptors, 1),
            2.0);

  // Test double.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.double_values",
                        /*allow_repeated = */ true));
  EXPECT_EQ(GetValueFromRepeatedProto<double>(test_proto, field_descriptors, 1),
            2.0);

  // Test bool.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.bool_values",
                        /*allow_repeated = */ true));
  EXPECT_FALSE(
      GetValueFromRepeatedProto<bool>(test_proto, field_descriptors, 1));

  // Test enum.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.enum_values",
                        /*allow_repeated = */ true));
  EXPECT_EQ(
      GetValueFromRepeatedProto<const google::protobuf::EnumValueDescriptor*>(
          test_proto, field_descriptors, 1)
          ->number(),
      2);

  // Test string.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.string_values",
                        /*allow_repeated = */ true));
  EXPECT_EQ(GetValueFromRepeatedProto<const std::string&>(test_proto,
                                                          field_descriptors, 1),
            "string2");

  // Test Message.
  ASSERT_OK_AND_ASSIGN(
      field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "repeated_proto_a",
                        /*allow_repeated = */ true));
  EXPECT_THAT(GetValueFromRepeatedProto<const google::protobuf::Message&>(
                  test_proto, field_descriptors, 1),
              EqualsProto(test_proto.repeated_proto_a(1)));
}

}  // namespace
}  // namespace wfa_virtual_people
