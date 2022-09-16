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

import com.google.protobuf.Descriptors.Descriptor
import com.google.protobuf.Descriptors.FieldDescriptor
import com.google.protobuf.Message.Builder
import com.google.protobuf.MessageOrBuilder

/**
 * A container for value of protobuf field.
 * @param isSet whether the field is set.
 * @param value the return of the getter of protobuf reflection. Will be the default value of the
 * field when the field is not set.
 */
data class ProtoFieldValue<V>(val isSet: Boolean, val value: V)

/**
 * In the protobuf message represented by [descriptor], get the field descriptors of the field, the
 * path of which is represented by [fullFieldName].
 *
 * @param descriptor descriptor of the proto.
 * @param fullFieldName field name separated by ".".
 * @param allowRepeated If true, the last field in the path represented by [fullFieldName] is
 * allowed to be a repeated field. If false, no repeated field is allowed in the path.
 *
 * @return a list of FieldDescriptors in the order of the field name to access the nested field.
 */
fun getFieldFromProto(
  descriptor: Descriptor,
  fullFieldName: String,
  allowRepeated: Boolean = false
): List<FieldDescriptor> {
  val result: MutableList<FieldDescriptor> = mutableListOf()

  var currentDescriptor = descriptor
  val fieldNames = fullFieldName.split('.')
  for (i in fieldNames.indices) {
    val fieldDescriptor: FieldDescriptor =
      currentDescriptor.findFieldByName((fieldNames[i]))
        ?: error("The field name is invalid: $fullFieldName")
    result.add(fieldDescriptor)
    if (i != fieldNames.size - 1) {
      /** Can't call .messageType on the last field, since it could be a non-Message. */
      currentDescriptor = fieldDescriptor.messageType
    }
  }

  if (result.isEmpty()) {
    /** This should never happen. */
    error("Get empty field descriptor from $fullFieldName")
  }
  for (fieldDescriptor in result.take(result.size - 1)) {
    if (fieldDescriptor.isRepeated) {
      error("Repeated field is not allowed in the path: $fullFieldName")
    }
  }
  if (!allowRepeated && result.last().isRepeated) {
    error("Repeated field is not allowed in the path: $fullFieldName")
  }
  return result
}

/**
 * Gets the parent message of the field represented by [fieldDescriptors] from the [message].
 *
 * Unset parent message is equivalent to an empty message.
 *
 * All elements except the last one in [fieldDescriptors] must refer to a protobuf Message. The
 * first element in [fieldDescriptors] must refer to a field in [message], Each of the rest elements
 * must refer to a field in the message referred by the previous element.
 *
 * The typical usage is to first call GetFieldFromProto(see above), to get fieldDescriptors for the
 * target field in [message], then call this function to get the parent message of the target field.
 *
 * Example: If we have a protobuf
 * ```
 * message MsgA {
 *     message MsgB {
 *       optional int32 c = 1;
 *     }
 *     optional MsgB b = 1;
 * }
 * ```
 * To get the field descriptors of MsgA.b.c:
 * ```
 *    fieldDescriptors = getFieldFromProto(MsgA().GetDescriptor(), "b.c"));
 * ```
 * And if there is an MsgA object obj_a, to get the message obj_a.b:
 * ```
 *    output = getParentMessageFromProto(obj_a, fieldDescriptors);
 * ```
 */
fun getParentMessageFromProto(
  message: MessageOrBuilder,
  fieldDescriptors: List<FieldDescriptor>
): MessageOrBuilder {
  /**
   * [fieldDescriptors] refers to a field in [message]. To get the parent message of the field, the
   * last entry in [fieldDescriptors] is unused.
   */
  var result = message
  for (fieldDescriptor in fieldDescriptors.take(fieldDescriptors.size - 1)) {
    val temp =
      if (result is Builder) {
        result.getFieldBuilder(fieldDescriptor)
      } else {
        result.getField(fieldDescriptor)
      }
        ?: error("Message doesn't contain field ${fieldDescriptor.name}")
    if (temp is MessageOrBuilder) {
      result = temp
    } else {
      error("${fieldDescriptor.name} is not a proto MessageOrBuilder")
    }
  }
  return result
}

/**
 * Gets the value from the [message], with field name represented by [fieldDescriptor].
 *
 * When called with unset field, returns default value.
 *
 * The field must be an immediate field of [message]. The corresponding C++ type of the field must
 * be [V].
 */
inline fun <reified V> getImmediateValueFromProtoOrDefault(
  message: MessageOrBuilder,
  fieldDescriptor: FieldDescriptor
): V {
  val result = message.getField(fieldDescriptor)
  return if (result is V) {
    result
  } else if (result is Int && V::class == UInt::class) {
    result.toUInt() as V
  } else if (result is Long && V::class == ULong::class) {
    result.toULong() as V
  } else {
    error("The type of the field is not compatible to the target")
  }
}

/**
 * Same as GetImmediateValueFromProtoOrDefault, except that returns a [ProtoFieldValue], which also
 * indicates whether the field is set.
 */
inline fun <reified V> getImmediateValueFromProto(
  message: MessageOrBuilder,
  fieldDescriptor: FieldDescriptor
): ProtoFieldValue<V> {
  return ProtoFieldValue(
    message.hasField(fieldDescriptor),
    getImmediateValueFromProtoOrDefault(message, fieldDescriptor)
  )
}

/**
 * Gets the value from the [message], with field path represented by [fieldDescriptors].
 *
 * isSet of the return indicates whether the field is set. value of the return is the value of the
 * field, and will be the default value when isSet is false.
 *
 * All elements except the last one in [fieldDescriptors] must refer to a protobuf Message. The last
 * one in [fieldDescriptors] must refer to a field with [V]. The first element in [fieldDescriptors]
 * must refer to a field in [message], Each of the rest elements must refer to a field in the
 * message referred by the previous element.
 */
inline fun <reified V> getValueFromProto(
  message: MessageOrBuilder,
  fieldDescriptors: List<FieldDescriptor>
): ProtoFieldValue<V> {
  return getImmediateValueFromProto(
    getParentMessageFromProto(message, fieldDescriptors),
    fieldDescriptors.last()
  )
}

/**
 * Sets the value to the [builder], with field name represented by [fieldDescriptor].
 *
 * The field must be an immediate field of [builder]. The corresponding Kotlin type of the field
 * must be [V].
 */
inline fun <reified V> setImmediateValueToProtoBuilder(
  builder: Builder,
  fieldDescriptor: FieldDescriptor,
  value: V
) {
  val fieldObject = builder.getField(fieldDescriptor)
  if (fieldObject is V) {
    builder.setField(fieldDescriptor, value)
  } else if (fieldObject is Int && V::class == UInt::class) {
    builder.setField(fieldDescriptor, (value as UInt).toInt())
  } else if (fieldObject is Long && V::class == ULong::class) {
    builder.setField(fieldDescriptor, (value as ULong).toLong())
  } else {
    error(
      "The type of the field is not compatible to the target. ${fieldObject.javaClass} vs  ${V::class}"
    )
  }
}

/**
 * Sets the value to the [builder], with field path represented by [fieldDescriptors].
 *
 * All elements except the last one in [fieldDescriptors] must refer to a protobuf Message. The last
 * one in [fieldDescriptors] must refer to a field with [V]. The first element in [fieldDescriptors]
 * must refer to a field in [builder], Each of the rest elements must refer to a field in the
 * message referred by the previous element.
 *
 * The typical usage is to first call getFieldFromProto(see above), to get [fieldDescriptors] for
 * the target field in [builder], then call this function to set the value of the target field.
 * Example: If we have a protobuf
 * ```
 *  message MsgA {
 *    message MsgB {
 *      optional int32 c = 1;
 *    }
 *    optional MsgB b = 1;
 *  }
 * ```
 * To get the field descriptors of MsgA.b.c:
 * ```
 *   val fieldDescriptors = getFieldFromProto(MsgA().GetDescriptor(), "b.c"));
 * ```
 * And if there is a MsgA object obj_a, to set the value of obj_a.b.c to 10:
 * ```
 *   setValueToProto(obj_a, fieldDescriptors, 10);
 * ```
 */
inline fun <reified V> setValueToProtoBuilder(
  builder: Builder,
  fieldDescriptors: List<FieldDescriptor>,
  value: V
) {
  val parentBuilder = getParentMessageFromProto(builder, fieldDescriptors)
  if (parentBuilder !is Builder) {
    /** this should never happen */
    error("expecting a Builder but get a Message")
  }
  setImmediateValueToProtoBuilder(parentBuilder, fieldDescriptors.last(), value)
}

/** Gets the size of the repeated field represented by [fieldDescriptors] in [message]. */
fun getSizeOfRepeatedProto(
  message: MessageOrBuilder,
  fieldDescriptors: List<FieldDescriptor>
): Int {
  return getParentMessageFromProto(message, fieldDescriptors)
    .getRepeatedFieldCount(fieldDescriptors.last())
}

/**
 * Gets the value from the [message], with repeated field name represented by [fieldDescriptor], and
 * the index represented by [index].
 *
 * [index] must be in the boundary of the repeated field.
 *
 * The field must be an immediate field of [message]. The corresponding type of the field must be
 * [V].
 */
inline fun <reified V> getImmediateValueFromRepeatedProto(
  message: MessageOrBuilder,
  fieldDescriptor: FieldDescriptor,
  index: Int
): V {
  val result = message.getRepeatedField(fieldDescriptor, index)
  return if (result is V) {
    result
  } else if (result is Int && V::class == UInt::class) {
    result.toUInt() as V
  } else if (result is Long && V::class == ULong::class) {
    result.toULong() as V
  } else {
    error("The type of the field is not compatible to the target: $result")
  }
}

/**
 * Gets the value from the [message], with repeated field path represented by [fieldDescriptors],
 * and the index represented by [index].
 *
 * [index] must be in the boundary of the repeated field.
 *
 * All entries except the last one in [fieldDescriptors] must refer to a singular protobuf Message
 * field. The last entry in [fieldDescriptors] must refer to a repeated field with [V]. The first
 * element in [fieldDescriptors] must refer to a field in [message], Each of the rest elements must
 * refer to a field in the message referred by the previous element.
 */
inline fun <reified V> getValueFromRepeatedProto(
  message: MessageOrBuilder,
  fieldDescriptors: List<FieldDescriptor>,
  index: Int
): V {
  return getImmediateValueFromRepeatedProto(
    getParentMessageFromProto(message, fieldDescriptors),
    fieldDescriptors.last(),
    index
  )
}

/**
 * Gets all values from the [message], with repeated field name represented by [fieldDescriptor]
 *
 * The field must be an immediate field of [message]. The corresponding type of the field must be
 * [V].
 */
inline fun <reified V> getAllImmediateValuesFromRepeatedProto(
  message: MessageOrBuilder,
  fieldDescriptor: FieldDescriptor,
): List<V> {
  return (0 until message.getRepeatedFieldCount(fieldDescriptor))
    .map {
      val value = message.getRepeatedField(fieldDescriptor, it)
      if (value is V) {
        value
      } else if (value is Int && V::class == UInt::class) {
        value.toUInt() as V
      } else if (value is Long && V::class == ULong::class) {
        value.toULong() as V
      } else {
        error("The type of the field is not compatible to the target: $value")
      }
    }
    .toList()
}

/**
 * Gets all values from the [message], with repeated field path represented by [fieldDescriptors]
 *
 * All entries except the last one in [fieldDescriptors] must refer to a singular protobuf Message
 * field. The last entry in [fieldDescriptors] must refer to a repeated field with [V]. The first
 * element in [fieldDescriptors] must refer to a field in [message], Each of the rest elements must
 * refer to a field in the message referred by the previous element.
 */
inline fun <reified V> getAllValuesFromRepeatedProto(
  message: MessageOrBuilder,
  fieldDescriptors: List<FieldDescriptor>,
): List<V> {
  return getAllImmediateValuesFromRepeatedProto(
    getParentMessageFromProto(message, fieldDescriptors),
    fieldDescriptors.last()
  )
}
