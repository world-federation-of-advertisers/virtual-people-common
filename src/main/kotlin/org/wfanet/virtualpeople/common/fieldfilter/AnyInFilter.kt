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

import com.google.protobuf.Descriptors.Descriptor
import com.google.protobuf.Descriptors.EnumValueDescriptor
import com.google.protobuf.Descriptors.FieldDescriptor
import com.google.protobuf.Descriptors.FieldDescriptor.Type
import com.google.protobuf.MessageOrBuilder
import org.wfanet.virtualpeople.common.FieldFilterProto
import org.wfanet.virtualpeople.common.fieldfilter.utils.*

/**
 * The implementation of [FieldFilter] when op is ANY_IN in config.
 *
 * The supported ValueTypes are: [Int], [UInt], [Long], [ULong], [Boolean], [EnumValueDescriptor],
 * [String]
 *
 * Always use [FieldFilter.create]. Users should never construct a [AnyInFilter] directly.
 */
internal abstract class AnyInFilter(val fieldDescriptors: List<FieldDescriptor>) : FieldFilter {
  companion object {
    /**
     * Create a AnyInFilter with specific type of expected values.
     *
     * Returns error if any of the following happens:
     * 1. config.op is not ANY_IN.
     * 2. config.name is not set.
     * 3. Except the last field, any other field of the path represented by config.name is repeated
     * field.
     * 4. The last field of the path represented by config.name is not repeated field.
     * 5. config.value is not set.
     * 6. Any entry in config.value(split by comma) cannot be cast to the type of the field
     * represented by config.name.
     */
    internal fun create(descriptor: Descriptor, config: FieldFilterProto): AnyInFilter {
      if (config.op != FieldFilterProto.Op.ANY_IN) {
        error("Op must be ANY_IN. Input FieldFilterProto: $config")
      }
      if (!config.hasName()) {
        error("Name must be set. Input FieldFilterProto: $config")
      }
      if (!config.hasValue()) {
        error("Value must be set. Input FieldFilterProto: $config")
      }
      val fieldDescriptors = getFieldFromProto(descriptor, config.name, allowRepeated = true)
      if (!fieldDescriptors.last().isRepeated) {
        error("Name must represent a repeated field. Input FieldFilterProto: $config")
      }

      return when (fieldDescriptors.last().type) {
        Type.INT32 -> AnyInFilterImpl<Int>(fieldDescriptors, parseValue(config.value))
        Type.UINT32 -> AnyInFilterImpl<UInt>(fieldDescriptors, parseValue(config.value))
        Type.UINT64 -> AnyInFilterImpl<ULong>(fieldDescriptors, parseValue(config.value))
        Type.INT64 -> AnyInFilterImpl<Long>(fieldDescriptors, parseValue(config.value))
        Type.BOOL -> AnyInFilterImpl<Boolean>(fieldDescriptors, parseValue(config.value))
        Type.STRING -> AnyInFilterImpl<String>(fieldDescriptors, parseValue(config.value))
        Type.ENUM ->
          AnyInFilterImpl(
            fieldDescriptors,
            parseEnumValues(fieldDescriptors.last().enumType, config.value)
              .map { fieldDescriptors.last().enumType.findValueByNumber(it) }
              .toSet()
          )
        else -> error("Unsupported field type for ANY_IN filter. Input FieldFilterProto: $config")
      }
    }
  }
}

/** Implementation of AnyFilter for [V]. */
internal class AnyInFilterImpl<V>(
  fieldDescriptors: List<FieldDescriptor>,
  private val parsedValues: Set<V>
) : AnyInFilter(fieldDescriptors) {

  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    @Suppress("UNCHECKED_CAST")
    /** Guaranteed safe since all types come from the same fieldDescriptor.Type */
    val values =
      when (fieldDescriptors.last().type) {
        Type.INT32 -> getAllValuesFromRepeatedProto<Int>(messageOrBuilder, fieldDescriptors)
        Type.UINT32 -> getAllValuesFromRepeatedProto<UInt>(messageOrBuilder, fieldDescriptors)
        Type.UINT64 -> getAllValuesFromRepeatedProto<ULong>(messageOrBuilder, fieldDescriptors)
        Type.INT64 -> getAllValuesFromRepeatedProto<Long>(messageOrBuilder, fieldDescriptors)
        Type.BOOL -> getAllValuesFromRepeatedProto<Boolean>(messageOrBuilder, fieldDescriptors)
        Type.STRING -> getAllValuesFromRepeatedProto<String>(messageOrBuilder, fieldDescriptors)
        Type.ENUM ->
          getAllValuesFromRepeatedProto<EnumValueDescriptor>(messageOrBuilder, fieldDescriptors)
        else -> error("Unsupported field type for ANY_IN filter. ${fieldDescriptors.last().type}")
      }
        as List<V>
    return values.any { parsedValues.contains(it) }
  }
}
