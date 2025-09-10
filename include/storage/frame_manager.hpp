#pragma once
#include "config.hpp"
#include <cassert>
#include <vector>

namespace bpt {

struct FrameHeader {
  page_id_t page_in_;
  int32_t pin_count_;
  int32_t history_;
  bool is_dirty_;
};

class FrameManager {
private:
  std::vector<FrameHeader> frame_info_;
  int32_t current_time_stamp_;

public:
  FrameManager() = delete;
  FrameManager(int32_t size);
  ~FrameManager();

  auto EvictFrame() -> std::pair<frame_id_t, page_id_t>;
  void ConnectPage(frame_id_t target_frame, page_id_t target_page);
  void Pin(frame_id_t target_frame, bool is_read);
  void Unpin(frame_id_t target_frame);
  void Erase(frame_id_t target_frame);
  auto IsDirty(frame_id_t target_frame) -> bool;
};
} // namespace bpt