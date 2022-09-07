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
import com.google.protobuf.Descriptors.FieldDescriptor
import com.google.protobuf.MessageOrBuilder
import org.wfanet.virtualpeople.common.FieldFilterProto
import org.wfanet.virtualpeople.common.fieldfilter.utils.getFieldFromProto
import org.wfanet.virtualpeople.common.fieldfilter.utils.getValueFromProto

/**
 * The implementation of [FieldFilter] when op is PARTIAL in config.
 *
 * Always use [FieldFilter.create]. Users should never construct a [PartialFilter] directly.
 */
internal class PartialFilter(descriptor: Descriptor, config: FieldFilterProto) : FieldFilter {

  private val fieldDescriptors: List<FieldDescriptor>
  private val subFilters: List<FieldFilter>

  init {
    if (config.op != FieldFilterProto.Op.PARTIAL) {
      error("Op must be PARTIAL. Input FieldFilterProto: $config")
    }
    if (!config.hasName()) {
      error("Name must be set. Input FieldFilterProto: $config")
    }
    if (config.subFiltersCount == 0) {
      error("sub_filters must be set when op is PARTIAL. Input FieldFilterProto: $config")
    }

    fieldDescriptors = getFieldFromProto(descriptor, config.name)
    if (fieldDescriptors.last().type != FieldDescriptor.Type.MESSAGE) {
      error("Name must refer to a message type field. Input FieldFilterProto: $config")
    }

    val subDescriptor: Descriptor = fieldDescriptors.last().messageType
    subFilters = config.subFiltersList.map { FieldFilter.create(subDescriptor, it) }
  }

  /** Returns true if all [subFilters] match */
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    val subMessage = getValueFromProto<MessageOrBuilder>(messageOrBuilder, fieldDescriptors)
    return subMessage.isSet && subFilters.all { it.matches(subMessage.value) }
  }
}
