#pragma once
#include "config.hpp"
#include <cstring>

namespace bpt {

#define TEMPLATE                                                               \
  template <typename KeyType, typename ValueType, typename KeyComparator>
#define LEAF_PAGE_TYPE LeafPage<KeyType, ValueType, KeyComparator>
#define INTERNAL_PAGE_TYPE InternalPage<KeyType, ValueType, KeyComparator>
#define PAGE_MAX_SIZE (4096 - 16) / (sizeof(KeyType) + sizeof(ValueType))

class TreePage {
public:
  enum class PageType { LEAF = false, INTERNAL };

private:
  int32_t size_;
  int32_t max_size_;
  PageType page_type_;

public:
  TreePage() : size_(0), max_size_(0), page_type_(PageType::LEAF) {}
  TreePage(int32_t maxsize, PageType page_type)
      : size_(0), max_size_(maxsize), page_type_(page_type) {}
  TreePage(const TreePage &other) = delete;
  ~TreePage() = default;

  auto IsLeafPage() const -> bool { return page_type_ == PageType::LEAF; }
  auto GetPageType() const -> PageType { return page_type_; }
  void SetPageType(PageType page_type) { page_type_ = page_type; }

  auto GetSize() const -> int32_t { return size_; }
  void SetSize(int32_t size) { size_ = size; }

  auto GetMaxSize() const -> int32_t { return max_size_; }
  void SetMaxSize(int32_t size) { max_size_ = size; }
  auto GetMinSize() const -> int32_t { return max_size_ >> 1; }
};
/*————————————————————————————————————————————————————————————————————————————————————————————————————————*/
TEMPLATE
class LeafPage : public TreePage {
private:
  KeyType key_array_[PAGE_MAX_SIZE];
  ValueType value_array_[PAGE_MAX_SIZE];
  page_id_t next_page_id_;

public:
  LeafPage() = delete;
  LeafPage(int32_t maxsize = PAGE_MAX_SIZE)
      : TreePage(maxsize, PageType::LEAF), next_page_id_(INVALID_PAGE_ID) {}
  LeafPage(const LeafPage &other) = delete;

  auto GetNextPageId() const -> page_id_t { return next_page_id_; };
  void SetNextPageId(page_id_t next_page_id) { next_page_id_ = next_page_id; }

  auto KeyAt(int32_t index) const -> KeyType { return key_array_[index]; }
  auto ValueAt(int32_t index) const -> ValueType { return value_array_[index]; }
  auto KeyIndex(const KeyType &target) const -> int32_t;

  auto InsertInPage(int32_t index, const KeyType &target_key,
                    const ValueType &target_value) -> int32_t;
  auto SplitPage(LeafPage *empty) -> KeyType;

  auto DeleteInPage(int32_t index) -> int32_t;
  auto BorrowFromPage(LeafPage *sibling, bool sibling_right) -> KeyType;
  void MergePage(LeafPage *sibling, bool sibling_right);
};

TEMPLATE
auto LEAF_PAGE_TYPE::KeyIndex(const KeyType &target) const -> int32_t {
  int32_t left = 0;
  int32_t right = GetSize();
  int32_t middle = 0;
  while (left < right) {
    middle = (left + right) >> 1;
    if (KeyComparator{}(key_array_[middle], target) < 0) {
      left = middle + 1;
    } else {
      right = middle;
    }
  }
  return left;
}

TEMPLATE
auto LEAF_PAGE_TYPE::InsertInPage(int32_t index, const KeyType &target_key,
                                  const ValueType &target_value) -> int32_t {
  std::memmove(key_array_ + index + 1, key_array_ + index,
               (GetSize() - index) * sizeof(KeyType));
  key_array_[index] = target_key;
  std::memmove(value_array_ + index + 1, value_array_ + index,
               (GetSize() - index) * sizeof(ValueType));
  value_array_[index] = target_value;
  SetSize(GetSize() + 1);
  return GetSize();
}

TEMPLATE
auto LEAF_PAGE_TYPE::SplitPage(LeafPage *empty) -> KeyType {
  new (empty) LeafPage(GetMaxSize());
  empty->SetPageType(GetPageType());
  empty->SetSize(GetSize() >> 1);
  SetSize(GetSize() - empty->GetSize());
  std::memcpy(empty->key_array_, key_array_ + GetSize(),
              empty->GetSize() * sizeof(KeyType));
  std::memcpy(empty->value_array_, value_array_ + GetSize(),
              empty->GetSize() * sizeof(ValueType));
  return empty->key_array_[0];
}

TEMPLATE
auto LEAF_PAGE_TYPE::DeleteInPage(int32_t index) -> int32_t {
  std::memmove(key_array_ + index, key_array_ + index + 1,
               (GetSize() - index - 1) * sizeof(KeyType));
  std::memmove(value_array_ + index, value_array_ + index + 1,
               (GetSize() - index - 1) * sizeof(ValueType));
  SetSize(GetSize() - 1);
  return GetSize();
}

TEMPLATE
auto LEAF_PAGE_TYPE::BorrowFromPage(LeafPage *sibling, bool sibling_right)
    -> KeyType {
  sibling->SetSize(sibling->GetSize() - 1);
  SetSize(GetSize() + 1);
  if (sibling_right) {
    key_array_[GetSize() - 1] = sibling->key_array_[0];
    value_array_[GetSize() - 1] = sibling->value_array_[0];
    std::memmove(sibling->key_array_, sibling->key_array_ + 1,
                 sibling->GetSize() * sizeof(KeyType));
    std::memmove(sibling->value_array_, sibling->value_array_ + 1,
                 sibling->GetSize() * sizeof(ValueType));
    return sibling->key_array_[0];
  }
  std::memmove(key_array_ + 1, key_array_, (GetSize() - 1) * sizeof(KeyType));
  std::memmove(value_array_ + 1, value_array_,
               (GetSize() - 1) * sizeof(ValueType));
  key_array_[0] = sibling->key_array_[sibling->GetSize()];
  value_array_[0] = sibling->value_array_[sibling->GetSize()];
  return key_array_[0];
}

TEMPLATE
void LEAF_PAGE_TYPE::MergePage(LeafPage *sibling, bool sibling_right) {
  if (sibling_right) {
    std::memmove(key_array_ + GetSize(), sibling->key_array_,
                 sibling->GetSize() * sizeof(KeyType));
    std::memmove(value_array_ + GetSize(), sibling->value_array_,
                 sibling->GetSize() * sizeof(ValueType));
    SetSize(GetSize() + sibling->GetSize());
    SetNextPageId(sibling->GetNextPageId());
    sibling->SetSize(0);
  } else {
    std::memmove(sibling->key_array_ + sibling->GetSize(), key_array_,
                 GetSize() * sizeof(KeyType));
    std::memmove(sibling->value_array_ + sibling->GetSize(), value_array_,
                 GetSize() * sizeof(ValueType));
    sibling->SetSize(sibling->GetSize() + GetSize());
    sibling->SetNextPageId(GetNextPageId());
    SetSize(0);
  }
};
/*————————————————————————————————————————————————————————————————————————————————————————————————————————*/
TEMPLATE
class InternalPage : public TreePage {
private:
  KeyType key_array_[PAGE_MAX_SIZE];
  ValueType value_array_[PAGE_MAX_SIZE];

public:
  InternalPage() = delete;
  InternalPage(int32_t max_size) : TreePage(max_size, PageType::INTERNAL){};
  InternalPage(const InternalPage &other) = delete;

  auto KeyAt(int32_t index) const -> KeyType { return key_array_[index]; }
  void SetKeyAt(int32_t index, const KeyType &key) { key_array_[index] = key; }
  auto ValueAt(int32_t index) const -> ValueType { return value_array_[index]; }
  auto KeyIndex(const KeyType &key) const -> int32_t;

  auto InsertInPage(int32_t index, const KeyType &target_key,
                    const ValueType &page_id) -> int32_t;
  auto InsertInPage(int32_t index, const ValueType &page_id) -> int32_t;
  auto SplitPage(InternalPage *empty) -> KeyType;

  auto DeleteInPage(int32_t index) -> int32_t;
  auto BorrowFromPage(InternalPage *sibling, const KeyType &middle,
                      bool sibling_right) -> KeyType;
  void MergePage(InternalPage *sibling, const KeyType &middle,
                 bool sibling_right);
};

TEMPLATE
auto INTERNAL_PAGE_TYPE::KeyIndex(const KeyType &key) const -> int32_t {
  int32_t left = 1;
  int32_t right = GetSize();
  int32_t middle = 0;
  while (left < right) {
    middle = (left + right) >> 1;
    if (KeyComparator{}(key_array_[middle], key) <= 0) {
      left = middle + 1;
    } else {
      right = middle;
    }
  }
  return left;
}

TEMPLATE
auto INTERNAL_PAGE_TYPE::InsertInPage(int32_t index, const KeyType &target_key,
                                      const ValueType &page_id) -> int32_t {
  std::memmove(key_array_ + index + 1, key_array_ + index,
               (GetSize() - index) * sizeof(KeyType));
  key_array_[index] = target_key;
  std::memmove(value_array_ + index + 1, value_array_ + index,
               (GetSize() - index) * sizeof(ValueType));
  value_array_[index] = page_id;
  SetSize(GetSize() + 1);
  return GetSize();
}
TEMPLATE
auto INTERNAL_PAGE_TYPE::InsertInPage(int32_t index, const ValueType &page_id)
    -> int32_t {
  value_array_[0] = page_id;
  SetSize(GetSize() + 1);
  return GetSize();
}

TEMPLATE
auto INTERNAL_PAGE_TYPE::SplitPage(InternalPage *empty) -> KeyType {
  new (empty) InternalPage(GetMaxSize());
  empty->SetPageType(GetPageType());
  empty->SetSize(GetSize() >> 1);
  SetSize(GetSize() - empty->GetSize());
  std::memcpy(empty->key_array_ + 1, key_array_ + GetSize() + 1,
              (empty->GetSize() - 1) * sizeof(KeyType));
  std::memcpy(empty->value_array_, value_array_ + GetSize(),
              empty->GetSize() * sizeof(ValueType));
  return key_array_[GetSize()];
}

TEMPLATE
auto INTERNAL_PAGE_TYPE::DeleteInPage(int32_t index) -> int32_t {
  std::memmove(key_array_ + index, key_array_ + index + 1,
               (GetSize() - index - 1) * sizeof(KeyType));
  std::memmove(value_array_ + index, value_array_ + index + 1,
               (GetSize() - index - 1) * sizeof(ValueType));
  SetSize(GetSize() - 1);
  return GetSize();
}

TEMPLATE
auto INTERNAL_PAGE_TYPE::BorrowFromPage(InternalPage *sibling,
                                        const KeyType &middle,
                                        bool sibling_right) -> KeyType {
  SetSize(GetSize() + 1);
  sibling->SetSize(sibling->GetSize() - 1);
  if (sibling_right) {
    key_array_[GetSize() - 1] = middle;
    value_array_[GetSize() - 1] = sibling->value_array_[0];
    KeyType temp = sibling->key_array_[1];
    std::memmove(sibling->key_array_ + 1, sibling->key_array_ + 2,
                 (sibling->GetSize() - 1) * sizeof(KeyType));
    std::memmove(sibling->value_array_, sibling->value_array_ + 1,
                 sibling->GetSize() * sizeof(ValueType));
    return temp;
  }
  std::memmove(key_array_ + 2, key_array_ + 1,
               (GetSize() - 2) * sizeof(KeyType));
  std::memmove(value_array_ + 1, value_array_,
               (GetSize() - 1) * sizeof(ValueType));
  key_array_[1] = middle;
  value_array_[0] = sibling->value_array_[sibling->GetSize()];
  return sibling->key_array_[sibling->GetSize()];
}

TEMPLATE
void INTERNAL_PAGE_TYPE::MergePage(InternalPage *sibling, const KeyType &middle,
                                   bool sibling_right) {
  if (sibling_right) {
    key_array_[GetSize()] = middle;
    std::memcpy(key_array_ + GetSize() + 1, sibling->key_array_ + 1,
                (sibling->GetSize() - 1) * sizeof(KeyType));
    std::memcpy(value_array_ + GetSize(), sibling->value_array_,
                sibling->GetSize() * sizeof(ValueType));
    SetSize(GetSize() + sibling->GetSize());
    sibling->SetSize(0);
  } else {
    sibling->key_array_[sibling->GetSize()] = middle;
    std::memcpy(sibling->key_array_ + sibling->GetSize() + 1, key_array_ + 1,
                (GetSize() - 1) * sizeof(KeyType));
    std::memcpy(sibling->value_array_ + sibling->GetSize(), value_array_,
                GetSize() * sizeof(ValueType));
    sibling->SetSize(sibling->GetSize() + GetSize());
    SetSize(0);
  }
}

} // namespace bpt