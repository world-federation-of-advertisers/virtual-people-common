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
import com.google.protobuf.MessageOrBuilder
import org.wfanet.virtualpeople.common.FieldFilterProto

/**
 * The implementation of [FieldFilter] when op is AND in config.
 *
 * Always use [FieldFilter.create]. Users should never construct a [AndFilter] directly.
 */
internal class AndFilter(descriptor: Descriptors.Descriptor, config: FieldFilterProto) :
  FieldFilter {

  private val subFilters: List<FieldFilter>

  init {
    if (config.op != FieldFilterProto.Op.AND) {
      error("Op must be AND. Input FieldFilterProto: $config")
    }
    if (config.subFiltersCount == 0) {
      error("sub_filters must be set when op is AND. Input FieldFilterProto: $config")
    }
    subFilters = config.subFiltersList.map { FieldFilter.create(descriptor, it) }
  }

  /** Returns true if all [subFilters] match */
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    return subFilters.all { it.matches(messageOrBuilder) }
  }
}
