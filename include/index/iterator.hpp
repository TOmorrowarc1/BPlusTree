#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include "b_plus_tree_page.hpp"
#include "buffer_pool_manager.hpp"

namespace bpt {

TEMPLATE
class IndexIterator {
  using Leaf = LeafPage<KeyType, ValueType, KeyComparator>;

private:
  BufferPoolManager *bpm_;
  Leaf *page_;
  int place_;

public:
  IndexIterator() = delete;
  IndexIterator(BufferPoolManager *bpm, Leaf *page, int place = 0)
      : bpm_(bpm), page_(page), place_(place){};
  ~IndexIterator() = default;

  auto IsEnd() -> bool { return page_ == nullptr && place_ == 0; };

  auto operator*() -> std::pair<const KeyType &, const ValueType &> {
    return {page_->KeyAt(place_), page_->ValueAt(place_)};
  }

  auto operator++() -> IndexIterator & {
    ++place_;
    if (place_ >= page_->GetSize()) {
      page_id_t next_page = page_->GetNextPageId();
      if (next_page == -1) {
        page_ = nullptr;
      } else {
        page_ =bpm_->VisitPage(page_->GetNextPageId(), false).template AsMut<Leaf>();
      }
      place_ = 0;
    }
    return *this;
  }

  auto operator==(const IndexIterator &itr) const -> bool {
    return (page_ == itr.page_) && (place_ == itr.place_);
  }

  auto operator!=(const IndexIterator &itr) const -> bool {
    return (page_ != itr.page_) || (place_ != itr.place_);
  }

  // A helper function print out the statement of the iterator.
  auto Getinfo() const -> std::pair<page_id_t, int> {
    return {page_.GetPageId(), place_};
  }
};
} // namespace bpt

#endif