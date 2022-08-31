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

package org.wfanet.virtualpeople.common.fieldfilter

import com.google.protobuf.Descriptors
import com.google.protobuf.Descriptors.EnumValueDescriptor
import com.google.protobuf.Descriptors.FieldDescriptor
import com.google.protobuf.Descriptors.FieldDescriptor.Type
import com.google.protobuf.MessageOrBuilder
import org.wfanet.virtualpeople.common.FieldFilterProto
import org.wfanet.virtualpeople.common.fieldfilter.utils.*

/**
 * The implementation of [FieldFilter] when op is EQUAL in config.
 *
 * The supported ValueTypes are: [Int], [Long], [Boolean], [EnumValueDescriptor], [String]
 *
 * Always use [FieldFilter.create] Users should never construct a [EqualFilter] directly.
 */
internal class EqualFilter(descriptor: Descriptors.Descriptor, config: FieldFilterProto) :
  FieldFilter {

  private val fieldDescriptors: List<FieldDescriptor>
  private val expectedValue: String

  private val supportedType =
    listOf(Type.INT32, Type.UINT32, Type.UINT64, Type.INT64, Type.BOOL, Type.ENUM, Type.STRING)

  init {
    if (config.op != FieldFilterProto.Op.EQUAL) {
      error("Op must be EQUAL. Input FieldFilterProto: $config")
    }
    if (!config.hasName()) {
      error("Name must be set. Input FieldFilterProto: $config")
    }
    if (!config.hasValue()) {
      error("Value must be set. Input FieldFilterProto: $config")
    }
    expectedValue = config.value
    fieldDescriptors = getFieldFromProto(descriptor, config.name, allowRepeated = false)
    if (fieldDescriptors.last().type !in supportedType) {
      error("Unsupported field type for EQUAL filter. Input FieldFilterProto: $config")
    }
  }

  /**
   * Returns true if any of the following is true. 1. The field is not repeated, and is set in
   * [messageOrBuilder]. 2. The field is repeated, and is not empty in [messageOrBuilder].
   */
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    return when (fieldDescriptors.last().type) {
      Type.INT32,
      Type.UINT32 -> matchesInternal<Int>(messageOrBuilder)
      Type.UINT64,
      Type.INT64 -> matchesInternal<Long>(messageOrBuilder)
      Type.BOOL -> matchesInternal<Boolean>(messageOrBuilder)
      Type.STRING -> matchesInternal<String>(messageOrBuilder)
      Type.ENUM -> matchesInternal<EnumValueDescriptor>(messageOrBuilder)
      else ->
        /** This should never happen. */
        error("Invalid field type: ${fieldDescriptors.last().type}")
    }
  }

  private inline fun <reified ValueType> matchesInternal(
    messageOrBuilder: MessageOrBuilder
  ): Boolean {
    val actualValue = getValueFromProto<ValueType>(messageOrBuilder, fieldDescriptors)
    return when (fieldDescriptors.last().type) {
      Type.ENUM ->
        actualValue.isSet &&
          actualValue.value == convertToEnum(fieldDescriptors.last().enumType, expectedValue)
      Type.STRING -> actualValue.isSet && actualValue.value == expectedValue
      else -> actualValue.isSet && actualValue.value == convertToNumeric<ValueType>(expectedValue)
    }
  }
}
