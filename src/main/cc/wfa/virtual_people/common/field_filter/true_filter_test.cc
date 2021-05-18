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

#include "absl/status/statusor.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "src/main/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "src/test/cc/testutil/status_macros.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {
namespace {

TEST(TrueFilterTest, TestIsMatch) {
  FieldFilterProto field_filter_proto;
  field_filter_proto.set_op(FieldFilterProto::TRUE);
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  TestProto test_proto;
  EXPECT_TRUE(field_filter->IsMatch(test_proto));
}

TEST(TrueFilterTest, TestIsMatchNotEmptyEvent) {
  FieldFilterProto field_filter_proto;
  field_filter_proto.set_op(FieldFilterProto::TRUE);
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  TestProto test_proto;
  test_proto.set_person_country_code("COUNTRY_1");
  EXPECT_TRUE(field_filter->IsMatch(test_proto));
}

TEST(TrueFilterTest, TestWithName) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"pb(
      op: TRUE
      name: "a"
  )pb", &field_filter_proto));
  auto field_filter =
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto);
  EXPECT_EQ(field_filter.status().code(),
            absl::StatusCode::kInvalidArgument);
}

TEST(TrueFilterTest, TestWithValue) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"pb(
      op: TRUE
      value: "1"
  )pb", &field_filter_proto));
  auto field_filter =
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto);
  EXPECT_EQ(field_filter.status().code(),
            absl::StatusCode::kInvalidArgument);
}

TEST(TrueFilterTest, TestWithSubFilters) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"pb(
      op: TRUE
      sub_filters {
        op: EQUAL
        name: "a"
        value: "1"
      }
  )pb", &field_filter_proto));
  auto field_filter =
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto);
  EXPECT_EQ(field_filter.status().code(),
            absl::StatusCode::kInvalidArgument);
}

}  // namespace
}  // namespace wfa_virtual_people
