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
import com.google.protobuf.Descriptors.Descriptor
import com.google.protobuf.Descriptors.EnumValueDescriptor
import com.google.protobuf.MessageOrBuilder
import org.wfanet.virtualpeople.common.FieldFilterProto
import org.wfanet.virtualpeople.common.fieldfilter.utils.*

/**
 * The implementation of [FieldFilter] when op is IN in config.
 *
 * The supported ValueTypes are: [Int], [Long], [Boolean], [EnumValueDescriptor], [String]
 *
 * Always use [FieldFilter.create]. Users should never construct a [InFilter] directly.
 */
internal abstract class InFilter(val fieldDescriptors: List<Descriptors.FieldDescriptor>) :
  FieldFilter {
  companion object {
    /**
     * Returns error if any of the following happens:
     * 1. [config.op] is not IN.
     * 2. [config.name] is not set.
     * 3. Any field of the path represented by [config.name] is repeated field.
     * 4. [config.value] is not set.
     * 5. Any entry in [config.value] (split by comma) cannot be cast to the type of the field
     * represented by [config.name].
     */
    internal fun create(descriptor: Descriptor, config: FieldFilterProto): InFilter {

      if (config.op != FieldFilterProto.Op.IN) {
        error("Op must be IN. Input FieldFilterProto: $config")
      }
      if (!config.hasName()) {
        error("Name must be set. Input FieldFilterProto: $config")
      }
      if (!config.hasValue()) {
        error("Value must be set. Input FieldFilterProto: $config")
      }
      val fieldDescriptors = getFieldFromProto(descriptor, config.name)

      return when (fieldDescriptors.last().type) {
        Descriptors.FieldDescriptor.Type.INT32,
        Descriptors.FieldDescriptor.Type.UINT32 ->
          InFilterImpl<Int>(fieldDescriptors, parseValue(config.value))
        Descriptors.FieldDescriptor.Type.UINT64,
        Descriptors.FieldDescriptor.Type.INT64 ->
          InFilterImpl<Long>(fieldDescriptors, parseValue(config.value))
        Descriptors.FieldDescriptor.Type.BOOL ->
          InFilterImpl<Boolean>(fieldDescriptors, parseValue(config.value))
        Descriptors.FieldDescriptor.Type.STRING ->
          InFilterImpl<String>(fieldDescriptors, parseValue(config.value))
        Descriptors.FieldDescriptor.Type.ENUM ->
          InFilterImpl(
            fieldDescriptors,
            parseEnumValues(fieldDescriptors.last().enumType, config.value)
              .map { fieldDescriptors.last().enumType.findValueByNumber(it) }
              .toSet()
          )
        else -> error("Unsupported field type for IN filter. Input FieldFilterProto: $config")
      }
    }
  }
}

/** Implementation of AnyFilter for [T]. */
internal class InFilterImpl<T>(
  fieldDescriptors: List<Descriptors.FieldDescriptor>,
  private val parsedValues: Set<T>
) : InFilter(fieldDescriptors) {

  /**
   * Returns true when the value of the field represented by [config.name] in [messageOrBuilder]
   * equals to any value in [config.value]. Otherwise, returns false. Returns false if the field is
   * not set.
   */
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    val protoFieldValue =
      when (fieldDescriptors.last().type) {
        Descriptors.FieldDescriptor.Type.INT32,
        Descriptors.FieldDescriptor.Type.UINT32 ->
          getValueFromProto<Int>(messageOrBuilder, fieldDescriptors)
        Descriptors.FieldDescriptor.Type.UINT64,
        Descriptors.FieldDescriptor.Type.INT64 ->
          getValueFromProto<Long>(messageOrBuilder, fieldDescriptors)
        Descriptors.FieldDescriptor.Type.BOOL ->
          getValueFromProto<Boolean>(messageOrBuilder, fieldDescriptors)
        Descriptors.FieldDescriptor.Type.STRING ->
          getValueFromProto<String>(messageOrBuilder, fieldDescriptors)
        Descriptors.FieldDescriptor.Type.ENUM ->
          getValueFromProto<EnumValueDescriptor>(messageOrBuilder, fieldDescriptors)
        else -> error("Unsupported field type for IN filter. ${fieldDescriptors.last().type}")
      }
    println("value :$protoFieldValue")
    println("parsedValues :$parsedValues")
    @Suppress("UNCHECKED_CAST")
    /** Guaranteed safe since all types come from the same fieldDescriptor.Type */
    return protoFieldValue.isSet && parsedValues.contains(protoFieldValue.value as T)
  }
}
