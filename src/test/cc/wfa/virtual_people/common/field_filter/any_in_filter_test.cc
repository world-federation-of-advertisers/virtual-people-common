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
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "src/test/cc/testutil/matchers.h"
#include "src/test/cc/testutil/status_macros.h"
#include "src/test/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"
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

TEST(AnyInFilterTest, TestNoName) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          op: ANY_IN
          value: "1,2,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(AnyInFilterTest, TestDisallowedSingularField) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.int32_value"
          op: ANY_IN
          value: "1,2,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(AnyInFilterTest, TestDisallowedRepeatedInThePath) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "repeated_proto_a.b.int32_values"
          op: ANY_IN
          value: "1,2,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(AnyInFilterTest, TestNoValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.int32_values"
          op: ANY_IN
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(AnyInFilterTest, TestInt32) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.int32_values"
          op: ANY_IN
          value: "1,2,1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_int32_values(1);
  test_proto_1.mutable_a()->mutable_b()->add_int32_values(3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_int32_values(2);
  test_proto_2.mutable_a()->mutable_b()->add_int32_values(3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->add_int32_values(3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(AnyInFilterTest, TestInt32InvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.int32_values"
          op: ANY_IN
          value: "1,a,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(AnyInFilterTest, TestInt64) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.int64_values"
          op: ANY_IN
          value: "1,2,1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_int64_values(1);
  test_proto_1.mutable_a()->mutable_b()->add_int64_values(3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_int64_values(2);
  test_proto_2.mutable_a()->mutable_b()->add_int64_values(3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->add_int64_values(3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(AnyInFilterTest, TestInt64InvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.int64_values"
          op: ANY_IN
          value: "1,a,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(AnyInFilterTest, TestUInt32) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.uint32_values"
          op: ANY_IN
          value: "1,2,1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_uint32_values(1);
  test_proto_1.mutable_a()->mutable_b()->add_uint32_values(3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_uint32_values(2);
  test_proto_2.mutable_a()->mutable_b()->add_uint32_values(3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->add_uint32_values(3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(AnyInFilterTest, TestUInt32InvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.uint32_values"
          op: ANY_IN
          value: "1,a,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(AnyInFilterTest, TestUInt64) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.uint64_values"
          op: ANY_IN
          value: "1,2,1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_uint64_values(1);
  test_proto_1.mutable_a()->mutable_b()->add_uint64_values(3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_uint64_values(2);
  test_proto_2.mutable_a()->mutable_b()->add_uint64_values(3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->add_uint64_values(3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(AnyInFilterTest, TestUInt64InvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.uint64_values"
          op: ANY_IN
          value: "1,a,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(AnyInFilterTest, TestBool) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.bool_values"
          op: ANY_IN
          value: "true,true"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_bool_values(true);
  test_proto_1.mutable_a()->mutable_b()->add_bool_values(false);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_bool_values(false);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));
}

TEST(AnyInFilterTest, TestBoolInvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.bool_values"
          op: ANY_IN
          value: "true,a"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(AnyInFilterTest, TestEnumName) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.enum_values"
          op: ANY_IN
          value: "TEST_ENUM_1,TEST_ENUM_2,TEST_ENUM_1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_1);
  test_proto_1.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_2);
  test_proto_2.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(AnyInFilterTest, TestEnumNumber) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.enum_values"
          op: ANY_IN
          value: "1,2,1"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_1);
  test_proto_1.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_2);
  test_proto_2.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(AnyInFilterTest, TestMixedEnumNameNumber) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.enum_values"
          op: ANY_IN
          value: "TEST_ENUM_1,2,1,TEST_ENUM_2"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_1);
  test_proto_1.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_2);
  test_proto_2.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_3);
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->add_enum_values(
      TestProtoB::TEST_ENUM_3);
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(AnyInFilterTest, TestEnumInvalidValue) {
  EXPECT_THAT(
      FilterFromProtoText(R"pb(
          name: "a.b.enum_values"
          op: ANY_IN
          value: "1,a,1"
      )pb").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(AnyInFilterTest, TestString) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.string_values"
          op: ANY_IN
          value: "a,b,a"
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_string_values("a");
  test_proto_1.mutable_a()->mutable_b()->add_string_values("c");
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_string_values("b");
  test_proto_2.mutable_a()->mutable_b()->add_string_values("c");
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  test_proto_3.mutable_a()->mutable_b()->add_string_values("c");
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

TEST(AnyInFilterTest, TestEmptyString) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.string_values"
          op: ANY_IN
          value: ""
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_string_values("");
  test_proto_1.mutable_a()->mutable_b()->add_string_values("a");
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_string_values("a");
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));
}

TEST(AnyInFilterTest, TestQuoteString) {
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FilterFromProtoText(R"pb(
          name: "a.b.string_values"
          op: ANY_IN
          value: "\""
      )pb"));

  TestProto test_proto_1;
  test_proto_1.mutable_a()->mutable_b()->add_string_values("\"");
  test_proto_1.mutable_a()->mutable_b()->add_string_values("a");
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  test_proto_2.mutable_a()->mutable_b()->add_string_values("a");
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));
}

}  // namespace
}  // namespace wfa_virtual_people
