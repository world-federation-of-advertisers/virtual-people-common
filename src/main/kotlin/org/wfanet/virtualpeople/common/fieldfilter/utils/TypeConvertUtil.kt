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

import com.google.protobuf.Descriptors.EnumDescriptor
import com.google.protobuf.Descriptors.EnumValueDescriptor

/**
 * Convert the string [input] to a numeric type. The allowed output types are
 *
 * [Int], [Long], [Float], [Double], [Boolean]
 *
 * For boolean, only "true' and "false" (capitalization doesn't matter) string are supported
 */
inline fun <reified ValueType> convertToNumeric(input: String): ValueType {
  return when (ValueType::class) {
    Int::class -> input.toInt() as ValueType
    Long::class -> input.toLong() as ValueType
    Float::class -> input.toFloat() as ValueType
    Double::class -> input.toDouble() as ValueType
    Boolean::class -> input.lowercase().toBooleanStrict() as ValueType
    else -> error("Unsupported data type: ${ValueType::class}")
  }
}

/** Convert the string [input] to an enum value descriptor. The enum is referred by [descriptor]. */
fun convertToEnum(descriptor: EnumDescriptor, input: String): EnumValueDescriptor {
  var enumValueDescriptor = descriptor.findValueByName(input)
  if (enumValueDescriptor == null) {
    enumValueDescriptor =
      descriptor.findValueByNumber(input.toIntOrNull() ?: error("Not a valid enum name or integer"))
  }
  if (enumValueDescriptor == null) {
    error("Input cannot be converted to enum. Input: $input")
  }
  return enumValueDescriptor
}
