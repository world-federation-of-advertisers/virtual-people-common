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
import org.wfanet.virtualpeople.common.FieldFilterProto
import org.wfanet.virtualpeople.common.fieldFilterProto

private val supportedType =
  listOf(
    Type.INT32,
    Type.INT64,
    Type.UINT32,
    Type.UINT64,
    Type.BOOL,
    Type.ENUM,
    Type.STRING,
    Type.MESSAGE
  )

/**
 * Convert the protobuf field in [message] represented by [fieldDescriptor] to a [FieldFilterProto],
 * which checks the equality of the given field, or any nested field of the given field.
 */
fun convertFieldToFilter(
  message: MessageOrBuilder,
  fieldDescriptor: FieldDescriptor,
): FieldFilterProto {
  if (fieldDescriptor.isRepeated) {
    error("Repeated field in message when converting to FieldFilterProto: $message")
  }

  if (fieldDescriptor.type !in supportedType) {
    error("Unsupported field type converting to FieldFilterProto from: $message")
  }
  return fieldFilterProto {
    name = fieldDescriptor.name
    op = FieldFilterProto.Op.EQUAL
    if (fieldDescriptor.type == Type.MESSAGE) {
      op = FieldFilterProto.Op.PARTIAL
      subFilters.add(convertMessageToFilter(message.getField(fieldDescriptor) as MessageOrBuilder))
    } else {
      value = message.getField(fieldDescriptor).toString()
    }
  }
}

fun convertMessageToFilter(
  message: MessageOrBuilder,
): FieldFilterProto {
  return fieldFilterProto {
    op = FieldFilterProto.Op.AND
    for (fieldDescriptor in message.allFields.keys) {
      subFilters.add(convertFieldToFilter(message, fieldDescriptor))
    }
  }
}
