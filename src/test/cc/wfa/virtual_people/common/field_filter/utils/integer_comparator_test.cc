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

#include "wfa/virtual_people/common/field_filter/utils/integer_comparator.h"

#include "absl/status/status.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gmock/gmock.h"
#include "google/protobuf/descriptor.h"
#include "gtest/gtest.h"
#include "src/test/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"
#include "wfa/virtual_people/common/field_filter/utils/field_util.h"

namespace wfa_virtual_people {
namespace {

using ::wfa::StatusIs;
using ::wfa_virtual_people::test::TestProto;

TEST(IntegerComparatorTest, FieldNotInteger) {
  // This is a float field.
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.float_value"));
  EXPECT_THAT(
      IntegerComparator::New(std::move(field_descriptors), "1").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(IntegerComparatorTest, InvalidValue) {
  // "a" is not a valid integer.
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value"));
  EXPECT_THAT(
      IntegerComparator::New(std::move(field_descriptors), "a").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(IntegerComparatorTest, ValueTypeNotMatch) {
  // "-1" is not a valid uint32.
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint32_value"));
  EXPECT_THAT(
      IntegerComparator::New(std::move(field_descriptors), "-1").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(IntegerComparatorTest, FloatValue) {
  // "10.5" is not a valid uint32.
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value"));
  EXPECT_THAT(
      IntegerComparator::New(std::move(field_descriptors), "10.5").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(IntegerComparatorTest, TestInt32) {
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int32_value"));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<IntegerComparator> comparator,
      IntegerComparator::New(std::move(field_descriptors), "10"));

  TestProto test_1;
  test_1.mutable_a()->mutable_b()->set_int32_value(11);
  EXPECT_EQ(comparator->Compare(test_1), IntegerCompareResult::GREATER_THAN);

  TestProto test_2;
  test_2.mutable_a()->mutable_b()->set_int32_value(10);
  EXPECT_EQ(comparator->Compare(test_2), IntegerCompareResult::EQUAL);

  TestProto test_3;
  test_3.mutable_a()->mutable_b()->set_int32_value(9);
  EXPECT_EQ(comparator->Compare(test_3), IntegerCompareResult::LESS_THAN);

  // Return INVALID when the field is not set.
  TestProto test_4;
  EXPECT_EQ(comparator->Compare(test_4), IntegerCompareResult::INVALID);
}

TEST(IntegerComparatorTest, TestInt64) {
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.int64_value"));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<IntegerComparator> comparator,
      IntegerComparator::New(std::move(field_descriptors), "10"));

  TestProto test_1;
  test_1.mutable_a()->mutable_b()->set_int64_value(11);
  EXPECT_EQ(comparator->Compare(test_1), IntegerCompareResult::GREATER_THAN);

  TestProto test_2;
  test_2.mutable_a()->mutable_b()->set_int64_value(10);
  EXPECT_EQ(comparator->Compare(test_2), IntegerCompareResult::EQUAL);

  TestProto test_3;
  test_3.mutable_a()->mutable_b()->set_int64_value(9);
  EXPECT_EQ(comparator->Compare(test_3), IntegerCompareResult::LESS_THAN);

  // Return INVALID when the field is not set.
  TestProto test_4;
  EXPECT_EQ(comparator->Compare(test_4), IntegerCompareResult::INVALID);
}

TEST(IntegerComparatorTest, TestUInt32) {
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint32_value"));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<IntegerComparator> comparator,
      IntegerComparator::New(std::move(field_descriptors), "10"));

  TestProto test_1;
  test_1.mutable_a()->mutable_b()->set_uint32_value(11);
  EXPECT_EQ(comparator->Compare(test_1), IntegerCompareResult::GREATER_THAN);

  TestProto test_2;
  test_2.mutable_a()->mutable_b()->set_uint32_value(10);
  EXPECT_EQ(comparator->Compare(test_2), IntegerCompareResult::EQUAL);

  TestProto test_3;
  test_3.mutable_a()->mutable_b()->set_uint32_value(9);
  EXPECT_EQ(comparator->Compare(test_3), IntegerCompareResult::LESS_THAN);

  // Return INVALID when the field is not set.
  TestProto test_4;
  EXPECT_EQ(comparator->Compare(test_4), IntegerCompareResult::INVALID);
}

TEST(IntegerComparatorTest, TestUInt64) {
  ASSERT_OK_AND_ASSIGN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(TestProto().GetDescriptor(), "a.b.uint64_value"));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<IntegerComparator> comparator,
      IntegerComparator::New(std::move(field_descriptors), "10"));

  TestProto test_1;
  test_1.mutable_a()->mutable_b()->set_uint64_value(11);
  EXPECT_EQ(comparator->Compare(test_1), IntegerCompareResult::GREATER_THAN);

  TestProto test_2;
  test_2.mutable_a()->mutable_b()->set_uint64_value(10);
  EXPECT_EQ(comparator->Compare(test_2), IntegerCompareResult::EQUAL);

  TestProto test_3;
  test_3.mutable_a()->mutable_b()->set_uint64_value(9);
  EXPECT_EQ(comparator->Compare(test_3), IntegerCompareResult::LESS_THAN);

  // Return INVALID when the field is not set.
  TestProto test_4;
  EXPECT_EQ(comparator->Compare(test_4), IntegerCompareResult::INVALID);
}

}  // namespace
}  // namespace wfa_virtual_people
