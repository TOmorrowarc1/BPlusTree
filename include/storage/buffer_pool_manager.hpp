#pragma once
#include "disk_manager.hpp"
#include "frame_manager.hpp"
#include <unordered_map>
#include <utility>

namespace bpt {
class PageGuard {
private:
  char *pointer_;
  FrameManager *frame_manager_;
  page_id_t page_in_;
  frame_id_t frame_in_;
  bool is_valid_;

public:
  PageGuard();
  PageGuard(char *pointer, page_id_t page_in, frame_id_t frame_in,
            FrameManager *frame_manager);
  PageGuard(const PageGuard &other) = delete;
  PageGuard(PageGuard &&other) noexcept;
  auto operator=(PageGuard &&other) -> PageGuard &;
  ~PageGuard();

  auto GetPageID() -> page_id_t;
  template <typename T> auto As() -> const T *;
  template <typename T> auto AsMut() -> T *;
};

class BufferPoolManager {
private:
  std::fstream data_file_;
  std::unordered_map<page_id_t, frame_id_t> page_table_;
  DiskManager *disk_manager_;
  FrameManager *frame_manager_;
  int32_t cache_size_;
  int32_t page_size_;
  char **cache_;

  void InitPage(page_id_t target_page);
  void FetchPage(page_id_t target_page, frame_id_t target_frame);
  void FlushPage(page_id_t target_page, frame_id_t target_frame);

public:
  BufferPoolManager() = delete;
  BufferPoolManager(int32_t cache_size, int32_t page_size,
                    const std::string &data_file,
                    const std::string &disk_manager_file);
  ~BufferPoolManager();

  auto NewPage() -> page_id_t;
  auto DeletePage(page_id_t target_page) -> bool;
  auto VisitPage(page_id_t target_page, bool read) -> PageGuard;
};

} // namespace bpt