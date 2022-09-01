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
 * The supported ValueTypes are: [Int], [Long], [Boolean], [EnumValueDescriptor], [String]
 *
 * Always use [FieldFilter.create]. Users should never construct a [AnyInFilter] directly.
 */
internal abstract class AnyInFilter : FieldFilter {
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
        Type.INT32,
        Type.UINT32 -> AnyInFilterInt(fieldDescriptors, parseValue(config.value))
        Type.UINT64,
        Type.INT64 -> AnyInFilterLong(fieldDescriptors, parseValue(config.value))
        Type.BOOL -> AnyInFilterBoolean(fieldDescriptors, parseValue(config.value))
        Type.STRING -> AnyInFilterString(fieldDescriptors, parseValue(config.value))
        Type.ENUM ->
          AnyInFilterEnum(
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

/** Implementation of AnyFilter for Int values. */
internal class AnyInFilterInt(
  private val fieldDescriptors: List<FieldDescriptor>,
  private val parsedValues: Set<Int>
) : AnyInFilter() {
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    getAllValuesFromRepeatedProto<Int>(messageOrBuilder, fieldDescriptors).forEach {
      if (parsedValues.contains(it)) {
        return true
      }
    }
    return false
  }
}

/** Implementation of AnyFilter for Long values. */
internal class AnyInFilterLong(
  private val fieldDescriptors: List<FieldDescriptor>,
  private val parsedValues: Set<Long>
) : AnyInFilter() {
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    getAllValuesFromRepeatedProto<Long>(messageOrBuilder, fieldDescriptors).forEach {
      if (parsedValues.contains(it)) {
        return true
      }
    }
    return false
  }
}

/** Implementation of AnyFilter for Boolean values. */
internal class AnyInFilterBoolean(
  private val fieldDescriptors: List<FieldDescriptor>,
  private val parsedValues: Set<Boolean>
) : AnyInFilter() {
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    getAllValuesFromRepeatedProto<Boolean>(messageOrBuilder, fieldDescriptors).forEach {
      if (parsedValues.contains(it)) {
        return true
      }
    }
    return false
  }
}

/** Implementation of AnyFilter for String values. */
internal class AnyInFilterString(
  private val fieldDescriptors: List<FieldDescriptor>,
  private val parsedValues: Set<String>
) : AnyInFilter() {
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    getAllValuesFromRepeatedProto<String>(messageOrBuilder, fieldDescriptors).forEach {
      if (parsedValues.contains(it)) {
        return true
      }
    }
    return false
  }
}

/** Implementation of AnyFilter for Enum values. */
internal class AnyInFilterEnum(
  private val fieldDescriptors: List<FieldDescriptor>,
  private val parsedValues: Set<EnumValueDescriptor>
) : AnyInFilter() {
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    getAllValuesFromRepeatedProto<EnumValueDescriptor>(messageOrBuilder, fieldDescriptors).forEach {
      if (parsedValues.contains(it)) {
        return true
      }
    }
    return false
  }
}
