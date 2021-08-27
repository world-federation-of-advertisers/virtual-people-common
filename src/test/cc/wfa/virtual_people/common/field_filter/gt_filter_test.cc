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

TEST(GtFilterTest, TestNoName) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      op: GT value: "1"
                                                    )pb",
                                                    &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(GtFilterTest, TestNotIntegerField) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.float_value" op: GT value: "1"
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(GtFilterTest, TestDisallowedRepeated) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.int32_values" op: GT value: "1"
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(GtFilterTest, TestDisallowedRepeatedInThePath) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "repeated_proto_a.b.int32_value" op: GT value: "1"
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(GtFilterTest, TestNoValue) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.int32_value" op: GT
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(GtFilterTest, TestValueNotInteger) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.int32_value" op: GT value: "10.5"
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(GtFilterTest, TestInt32) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.int32_value" op: GT value: "10"
      )pb",
      &field_filter_proto));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  TestProto test_1;
  test_1.mutable_a()->mutable_b()->set_int32_value(11);
  EXPECT_TRUE(field_filter->IsMatch(test_1));

  TestProto test_2;
  test_2.mutable_a()->mutable_b()->set_int32_value(10);
  EXPECT_FALSE(field_filter->IsMatch(test_2));

  TestProto test_3;
  test_3.mutable_a()->mutable_b()->set_int32_value(9);
  EXPECT_FALSE(field_filter->IsMatch(test_3));
}

TEST(GtFilterTest, TestInt32NotSet) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.int32_value" op: GT value: "-1"
      )pb",
      &field_filter_proto));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

TEST(GtFilterTest, TestInt64) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.int64_value" op: GT value: "10"
      )pb",
      &field_filter_proto));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  TestProto test_1;
  test_1.mutable_a()->mutable_b()->set_int64_value(11);
  EXPECT_TRUE(field_filter->IsMatch(test_1));

  TestProto test_2;
  test_2.mutable_a()->mutable_b()->set_int64_value(10);
  EXPECT_FALSE(field_filter->IsMatch(test_2));

  TestProto test_3;
  test_3.mutable_a()->mutable_b()->set_int64_value(9);
  EXPECT_FALSE(field_filter->IsMatch(test_3));
}

TEST(GtFilterTest, TestInt64NotSet) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.int64_value" op: GT value: "-1"
      )pb",
      &field_filter_proto));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

TEST(GtFilterTest, TestUInt32) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.uint32_value" op: GT value: "10"
      )pb",
      &field_filter_proto));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  TestProto test_1;
  test_1.mutable_a()->mutable_b()->set_uint32_value(11);
  EXPECT_TRUE(field_filter->IsMatch(test_1));

  TestProto test_2;
  test_2.mutable_a()->mutable_b()->set_uint32_value(10);
  EXPECT_FALSE(field_filter->IsMatch(test_2));

  TestProto test_3;
  test_3.mutable_a()->mutable_b()->set_uint32_value(9);
  EXPECT_FALSE(field_filter->IsMatch(test_3));
}

TEST(GtFilterTest, TestUInt32NotSet) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.uint32_value" op: GT value: "0"
      )pb",
      &field_filter_proto));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

TEST(GtFilterTest, TestUInt64) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.uint64_value" op: GT value: "10"
      )pb",
      &field_filter_proto));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  TestProto test_1;
  test_1.mutable_a()->mutable_b()->set_uint64_value(11);
  EXPECT_TRUE(field_filter->IsMatch(test_1));

  TestProto test_2;
  test_2.mutable_a()->mutable_b()->set_uint64_value(10);
  EXPECT_FALSE(field_filter->IsMatch(test_2));

  TestProto test_3;
  test_3.mutable_a()->mutable_b()->set_uint64_value(9);
  EXPECT_FALSE(field_filter->IsMatch(test_3));
}

TEST(GtFilterTest, TestUInt64NotSet) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.uint64_value" op: GT value: "0"
      )pb",
      &field_filter_proto));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  // Return false when the field is not set.
  TestProto test_proto;
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

}  // namespace
}  // namespace wfa_virtual_people
