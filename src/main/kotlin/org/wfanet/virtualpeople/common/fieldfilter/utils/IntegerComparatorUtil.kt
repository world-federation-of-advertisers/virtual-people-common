// Copyright 2022 The Cross-Media Measurement Authors
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

package org.wfanet.virtualpeople.common.fieldfilter.utils

import com.google.protobuf.Descriptors.FieldDescriptor
import com.google.protobuf.Descriptors.FieldDescriptor.Type
import com.google.protobuf.MessageOrBuilder

enum class IntegerCompareResult {
  INVALID,
  GREATER_THAN,
  EQUAL,
  LESS_THAN,
}

/**
 * A helper class to do integer comparison between a field in a protobuf message and a given integer
 * represented by a string.
 *
 * The supported integer types are: [Int], [UInt], [Long], [ULong]
 *
 * The path represented by [fieldDescriptors] must be a valid path to an integer field in
 * [messageOrBuilder].
 *
 * Usage example: If we have protobuf message:
 * ```
 * message A {
 *   message B {
 *     optional int32 c = 1;
 *   }
 *   optional B b = 1;
 * }
 * ```
 *
 * To get the field descriptors:
 * ```
 *    fieldDescriptors = getFieldFromProto(A().GetDescriptor(), "b.c"));
 * ```
 *
 * To build an IntegerComparator which compares the field b.c in message A with value 10:
 * ```
 * comparator = IntegerComparator.new(fieldDescriptors, "10")
 * ```
 *
 * Compare examples:
 * ```
 * testProto1 = testProto { a = testProtoA { b = testProtoB { value = 11 } } }
 * IntegerCompareResult result1 = comparator.compare(testProto1); // GREATER_THAN
 *
 * testProto2 = testProto { a = testProtoA { b = testProtoB { value = 10 } } }
 * IntegerCompareResult result2 = comparator.compare(testProto2); // EQUAL
 *
 * testProto3 = testProto { a = testProtoA { b = testProtoB { value = 9 } } }
 * IntegerCompareResult result3 = comparator.compare(testProto3); // LESS_THAN
 * ```
 */
abstract class IntegerComparator(val fieldDescriptors: List<FieldDescriptor>) {
  /**
   * Compares the field represented by [fieldDescriptors] in [messageOrBuilder] and [value]. Returns
   * GREATER_THAN if the field in [messageOrBuilder] is greater than [value]. Returns EQUAL if the
   * field in [messageOrBuilder] is equal to [value]. Returns LESS_THAN if the field in
   * [messageOrBuilder] is less than [value]. Returns INVALID if the field in [messageOrBuilder] is
   * not set.
   *
   * [fieldDescriptors] must represent a valid path in [messageOrBuilder] to an integer field.
   */
  abstract fun compare(messageOrBuilder: MessageOrBuilder): IntegerCompareResult

  companion object {
    /**
     * Always use [IntegerComparator.new] to get an [IntegerComparator] object.
     *
     * Throws an error if any of the following happens:
     * 1. The last entry of [fieldDescriptors] refers to a non-integer field.
     * 2. The type of [value] does not match the type of the field represented by the last entry of
     * [fieldDescriptors].
     *
     * No repeated field is allowed in [fieldDescriptors]. This will not be caught in this class.
     * But when following the examples above, the repeated field error will be caught before calling
     * this library.
     */
    fun new(fieldDescriptors: List<FieldDescriptor>, value: String): IntegerComparator {
      return when (fieldDescriptors.last().type) {
        Type.INT32 -> IntegerComparatorImpl<Int>(fieldDescriptors, convertToNumeric(value))
        Type.UINT32 -> IntegerComparatorImpl<UInt>(fieldDescriptors, convertToNumeric(value))
        Type.INT64 -> IntegerComparatorImpl<Long>(fieldDescriptors, convertToNumeric(value))
        Type.UINT64 -> IntegerComparatorImpl<ULong>(fieldDescriptors, convertToNumeric(value))
        else -> {
          error("The given field is not integer when building IntegerComparator.")
        }
      }
    }
  }
}

private class IntegerComparatorImpl<T>(fieldDescriptors: List<FieldDescriptor>, val value: T) :
  IntegerComparator(fieldDescriptors) {

  @Suppress("UNCHECKED_CAST")
  override fun compare(messageOrBuilder: MessageOrBuilder): IntegerCompareResult {
    val fieldValue =
      when (fieldDescriptors.last().type) {
        Type.INT32,
        Type.UINT32 -> getValueFromProto<Int>(messageOrBuilder, fieldDescriptors)
        Type.INT64,
        Type.UINT64 -> getValueFromProto<Long>(messageOrBuilder, fieldDescriptors)
        else -> error("The given field is not integer.")
      }
    if (!fieldValue.isSet) {
      return IntegerCompareResult.INVALID
    }

    val actualValue =
      when (fieldDescriptors.last().type) {
        Type.INT32 -> fieldValue.value
        Type.UINT32 -> (fieldValue.value as Int).toUInt()
        Type.INT64 -> fieldValue.value
        Type.UINT64 -> (fieldValue.value as Long).toULong()
        else -> error("The given field is not integer.")
      }
        as Comparable<T>

    if (actualValue > value) {
      return IntegerCompareResult.GREATER_THAN
    }
    if (actualValue < value) {
      return IntegerCompareResult.LESS_THAN
    }
    return IntegerCompareResult.EQUAL
  }
}
