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

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gmock/gmock.h"
#include "google/protobuf/text_format.h"
#include "gtest/gtest.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "src/test/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {
namespace {

using ::wfa::StatusIs;
using ::wfa_virtual_people::test::TestProto;

// This function is required to test the FieldFilter still works when the
// FieldFilterProto is out of scope.
absl::StatusOr<std::unique_ptr<FieldFilter>> FilterFromProtoText(
    absl::string_view proto_text) {
  FieldFilterProto field_filter_proto;
  EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
      std::string(proto_text), &field_filter_proto));
  return FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto);
}

TEST(EqualFilterTest, TestNoName) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      op: EQUAL value: "1"
                                                    )pb",
                                                    &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(EqualFilterTest, TestDisallowedRepeated) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.int32_values" op: EQUAL value: "1"
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(EqualFilterTest, TestDisallowedRepeatedInThePath) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "repeated_proto_a.b.int32_value" op: EQUAL value: "1"
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(EqualFilterTest, TestNoValue) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.int32_value" op: EQUAL
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(EqualFilterTest, TestInt32) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.int32_value" op: EQUAL value: "1"
      )pb"));

  TestProto test_proto_1, test_proto_2;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      a { b { int32_value: 1 } }
                                                    )pb",
                                                    &test_proto_1));
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      a { b { int32_value: 2 } }
                                                    )pb",
                                                    &test_proto_2));
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));
}

TEST(EqualFilterTest, TestInt32NotSet) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.int32_value" op: EQUAL value: "0"
      )pb"));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

TEST(EqualFilterTest, TestInt64) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.int64_value" op: EQUAL value: "1"
      )pb"));

  TestProto test_proto_1, test_proto_2;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      a { b { int64_value: 1 } }
                                                    )pb",
                                                    &test_proto_1));
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      a { b { int64_value: 2 } }
                                                    )pb",
                                                    &test_proto_2));
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));
}

TEST(EqualFilterTest, TestInt64NotSet) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.int64_value" op: EQUAL value: "0"
      )pb"));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

TEST(EqualFilterTest, TestUInt32) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.uint32_value" op: EQUAL value: "1"
      )pb"));

  TestProto test_proto_1, test_proto_2;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { uint32_value: 1 } }
      )pb",
      &test_proto_1));
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { uint32_value: 2 } }
      )pb",
      &test_proto_2));
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));
}

TEST(EqualFilterTest, TestUInt32NotSet) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.uint32_value" op: EQUAL value: "0"
      )pb"));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

TEST(EqualFilterTest, TestUInt64) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.uint64_value" op: EQUAL value: "1"
      )pb"));

  TestProto test_proto_1, test_proto_2;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { uint64_value: 1 } }
      )pb",
      &test_proto_1));
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { uint64_value: 2 } }
      )pb",
      &test_proto_2));
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));
}

TEST(EqualFilterTest, TestUInt64NotSet) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.uint64_value" op: EQUAL value: "0"
      )pb"));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

TEST(EqualFilterTest, TestBool) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.bool_value" op: EQUAL value: "true"
      )pb"));

  TestProto test_proto_1, test_proto_2;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { bool_value: true } }
      )pb",
      &test_proto_1));
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { bool_value: false } }
      )pb",
      &test_proto_2));
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));
}

TEST(EqualFilterTest, TestBoolNotSet) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.bool_value" op: EQUAL value: "false"
      )pb"));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

TEST(EqualFilterTest, TestEnum) {
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<FieldFilter> field_filter,
                       FilterFromProtoText(R"pb(
                         name: "a.b.enum_value"
                         op: EQUAL
                         value: "TEST_ENUM_1"
                       )pb"));

  TestProto test_proto_1, test_proto_2;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { enum_value: TEST_ENUM_1 } }
      )pb",
      &test_proto_1));
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { enum_value: TEST_ENUM_2 } }
      )pb",
      &test_proto_2));
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));
}

TEST(EqualFilterTest, TestEnumNotSet) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.enum_value" op: EQUAL value: "INVALID"
      )pb"));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

TEST(EqualFilterTest, TestString) {
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<FieldFilter> field_filter,
                       FilterFromProtoText(R"pb(
                         name: "a.b.string_value"
                         op: EQUAL
                         value: "string1"
                       )pb"));

  TestProto test_proto_1, test_proto_2;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { string_value: "string1" } }
      )pb",
      &test_proto_1));
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { string_value: "string2" } }
      )pb",
      &test_proto_2));
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));
}

TEST(EqualFilterTest, TestStringNotSet) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
        name: "a.b.string_value" op: EQUAL value: ""
      )pb"));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

}  // namespace
}  // namespace wfa_virtual_people
