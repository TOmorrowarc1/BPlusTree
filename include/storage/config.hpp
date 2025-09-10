#pragma once
#include <stdint.h>

namespace bpt {

using page_id_t = int32_t;
using frame_id_t = int32_t;
const page_id_t INVALID_PAGE_ID = -1;
const frame_id_t INVALID_FRMAE_ID = -1;

} // namespace bpt