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

/**
 * A helper function to parse and store [valuesString] as a set of V. [valuesString] is a string
 * represents a list of [V] entities separated by comma.
 *
 * The supported [V]s are: [Int], [UInt], [Long], [ULong], [Boolean], [String].
 */
inline fun <reified V> parseValue(valuesString: String): Set<V> {
  return valuesString
    .split(',')
    .map { value ->
      when (V::class) {
        String::class -> value as V
        Int::class,
        UInt::class,
        Long::class,
        ULong::class,
        Boolean::class -> convertToNumeric(value)
        else ->
          error("Only Int, UInt, Long, ULong, Boolean and String are supported, get ${V::class}")
      }
    }
    .toSet()
}

/**
 * A helper function to parse and store [valuesString] as a set of protobuf enums. [valuesString] is
 * a string represents a list of protobuf enum values (enum number or enum name) separated by comma.
 */
fun parseEnumValues(descriptor: EnumDescriptor, valuesString: String): Set<Int> {
  return valuesString.split(',').map { value -> convertToEnum(descriptor, value).index }.toSet()
}
