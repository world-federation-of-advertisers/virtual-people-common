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
#include "gmock/gmock.h"
#include "google/protobuf/text_format.h"
#include "gtest/gtest.h"
#include "src/main/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "src/test/cc/testutil/matchers.h"
#include "src/test/cc/testutil/status_macros.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {
namespace {

using ::wfa::StatusIs;
using ::wfa_virtual_people::test::TestProto;
using ::wfa_virtual_people::test::TestProtoB;

// This function is required to test the FieldFilter still works when the
// FieldFilterProto is out of scope.
absl::StatusOr<std::unique_ptr<FieldFilter>> FilterFromProtoText(
    absl::string_view proto_text) {
  FieldFilterProto field_filter_proto;
  EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
      std::string(proto_text), &field_filter_proto));
  return FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto);
}

TEST(InFilterTest, TestNoName) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          op: IN
          value: "1,2,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(InFilterTest, TestDisallowedRepeated) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.int32_values"
          op: IN
          value: "1,2,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(InFilterTest, TestDisallowedRepeatedInThePath) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "repeated_proto_a.b.int32_value"
          op: IN
          value: "1,2,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(InFilterTest, TestNoValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.int32_value"
          op: IN
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(InFilterTest, TestInt32) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.int32_value"
          op: IN
          value: "1,2,1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->set_int32_value(1);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->set_int32_value(2);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->set_int32_value(3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(InFilterTest, TestInt32InvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.int32_value"
          op: IN
          value: "1,a,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(InFilterTest, TestInt64) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.int64_value"
          op: IN
          value: "1,2,1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->set_int64_value(1);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->set_int64_value(2);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->set_int64_value(3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(InFilterTest, TestInt64InvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.int64_value"
          op: IN
          value: "1,a,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(InFilterTest, TestUInt32) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.uint32_value"
          op: IN
          value: "1,2,1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->set_uint32_value(1);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->set_uint32_value(2);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->set_uint32_value(3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(InFilterTest, TestUInt32InvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.uint32_value"
          op: IN
          value: "1,a,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(InFilterTest, TestUInt64) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.uint64_value"
          op: IN
          value: "1,2,1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->set_uint64_value(1);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->set_uint64_value(2);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->set_uint64_value(3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(InFilterTest, TestUInt64InvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.uint64_value"
          op: IN
          value: "1,a,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(InFilterTest, TestBool) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.bool_value"
          op: IN
          value: "true,true"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->set_bool_value(true);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->set_bool_value(false);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));
}

TEST(InFilterTest, TestBoolInvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.bool_value"
          op: IN
          value: "true,a"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(InFilterTest, TestEnumName) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.enum_value"
          op: IN
          value: "TEST_ENUM_1,TEST_ENUM_2,TEST_ENUM_1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->set_enum_value(
      TestProtoB::TEST_ENUM_1);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->set_enum_value(
      TestProtoB::TEST_ENUM_2);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->set_enum_value(
      TestProtoB::TEST_ENUM_3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(InFilterTest, TestEnumNumber) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.enum_value"
          op: IN
          value: "1,2,1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->set_enum_value(
      TestProtoB::TEST_ENUM_1);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->set_enum_value(
      TestProtoB::TEST_ENUM_2);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->set_enum_value(
      TestProtoB::TEST_ENUM_3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(InFilterTest, TestMixedEnumNameNumber) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.enum_value"
          op: IN
          value: "TEST_ENUM_1,2,1,TEST_ENUM_2"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->set_enum_value(
      TestProtoB::TEST_ENUM_1);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->set_enum_value(
      TestProtoB::TEST_ENUM_2);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->set_enum_value(
      TestProtoB::TEST_ENUM_3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(InFilterTest, TestEnumInvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.enum_value"
          op: IN
          value: "1,a,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(InFilterTest, TestString) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.string_value"
          op: IN
          value: "a,b,a"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->set_string_value("a");
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->set_string_value("b");
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->set_string_value("c");
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(InFilterTest, TestQuoteString) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.string_value"
          op: IN
          value: "\""
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->set_string_value("\"");
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->set_string_value("a");
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));
}

}  // namespace
}  // namespace wfa_virtual_people
