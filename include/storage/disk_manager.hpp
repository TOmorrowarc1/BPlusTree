#pragma once
#include "config.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace bpt {

class DiskManager {
private:
  std::fstream file_;
  page_id_t next_page_;
  std::vector<page_id_t> empty_page_;

public:
  DiskManager() = delete;
  DiskManager(const std::string &file_name);
  ~DiskManager();

  auto NewPage() -> std::pair<page_id_t, bool>;
  void DeletePage(page_id_t target_page);
};

} // namespace bpt