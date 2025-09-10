#include "index/b_plus_tree.hpp"
#include <cstring>
#include <iostream>
#include <string>
class MyString {
private:
  char content[65] = {0};

public:
  MyString() = default;
  MyString(const std::string &);
  ~MyString() = default;
  MyString &operator=(const MyString &);
  MyString &operator=(const std::string &);
  std::string return_content();
  char &operator[](int);
  bool operator>(const MyString &) const;
  bool operator==(const MyString &) const;
  bool operator!=(const MyString &) const;
  bool operator>=(const MyString &) const;
  bool operator<(const MyString &) const;
  bool operator<=(const MyString &) const;
  friend std::ostream &operator<<(std::ostream &output, const MyString &object);
  friend std::istream &operator>>(std::istream &output, MyString &object);
};
MyString::MyString(const std::string &target) {
  std::strcpy(content, &target[0]);
}
std::string MyString::return_content() { return std::string(content); }
char &MyString::operator[](int place) { return content[place]; }
bool MyString::operator>(const MyString &B) const {
  return std::strcmp((*this).content, B.content) > 0;
}
bool MyString::operator==(const MyString &B) const {
  return std::strcmp((*this).content, B.content) == 0;
}
bool MyString::operator!=(const MyString &B) const {
  return std::strcmp((*this).content, B.content) != 0;
}
bool MyString::operator>=(const MyString &B) const {
  return std::strcmp((*this).content, B.content) >= 0;
}
bool MyString::operator<(const MyString &B) const {
  return std::strcmp((*this).content, B.content) < 0;
}
bool MyString::operator<=(const MyString &B) const {
  return std::strcmp((*this).content, B.content) <= 0;
}
MyString &MyString::operator=(const MyString &B) {
  if (&B != this) {
    std::strcpy(content, B.content);
  }
  return *this;
}
MyString &MyString::operator=(const std::string &B) {
  std::strcpy(content, &B[0]);
  return *this;
}
std::ostream &operator<<(std::ostream &output, const MyString &object) {
  return output << object.content;
}
std::istream &operator>>(std::istream &input, MyString &object) {
  std::string temp;
  input >> temp;
  object = temp;
  return input;
}

struct Key {
  MyString key;
  int value;
};
struct KeyComparator {
  auto operator()(const Key &lhs, const Key &rhs) -> int {
    if (lhs.key > rhs.key) {
      return 1;
    } else if (rhs.key > lhs.key) {
      return -1;
    } else {
      if (lhs.value > rhs.value) {
        return 1;
      } else if (rhs.value > lhs.value) {
        return -1;
      }
    }
    return 0;
  }
};

int main() {
  int operation_num = 0, value = 0;
  std::cin >> operation_num;
  MyString operation, index;
  std::string insert = "insert", del = "delete", find = "find";
  bpt::BufferPoolManager bpm(50, 4096, "data_file", "disk_file");
  bpm.NewPage();
  bpt::BPlusTree<Key, int, KeyComparator> storage(0, &bpm, 2, 3);
  Key key;
  /*for (int i = 999; i > 0; --i) {
    key.key = "Amiya";
    key.value = value = i;
    storage.Insert(key, value);
    assert(!storage.Insert(key, value));
  }
  int count = 0;
  key.key = "Amiya";
  key.value = (1 << 31);
  auto iter = storage.KeyBegin(key);
  key.value = ~key.value;
  while (!iter.IsEnd() && KeyComparator{}((*iter).first, key) <= 0) {
    ++count;
    std::cout << (*iter).second << ' ';
    ++iter;
  }
  if (count == 0) {
    std::cout << "null";
  }
  std::cout << '\n';
  std::cout << "Checkpoint 1" << '\n';
  for (int i = 0; i < 500; ++i) {
    key.value = i;
    storage.Remove(key, i);
  }
  count = 0;
  key.key = "Amiya";
  key.value = (1 << 31);
  iter = storage.KeyBegin(key);
  key.value = ~key.value;
  while (!iter.IsEnd() && KeyComparator{}((*iter).first, key) <= 0) {
    ++count;
    std::cout << (*iter).second << ' ';
    ++iter;
  }
  if (count == 0) {
    std::cout << "null";
  }
  std::cout << '\n';
  std::cout << "Checkpoint 2" << '\n';*/
  std::vector<int> test;
  for (int i = 0; i < operation_num; ++i) {
    std::cin >> operation >> key.key;
    if (operation == insert) {
      std::cin >> key.value;
      value = key.value;
      storage.Insert(key, value);
      assert(storage.GetValue(key, &test));
    } else if (operation == del) {
      std::cin >> key.value;
      value = key.value;
      storage.Remove(key, value);
      assert(!storage.GetValue(key, &test));
    } else {
      int count = 0;
      key.value = (1 << 31);
      auto iter = storage.KeyBegin(key);
      key.value = ~key.value;
      while (!iter.IsEnd() && KeyComparator{}((*iter).first, key) <= 0) {
        ++count;
        std::cout << (*iter).second << ' ';
        ++iter;
      }
      if (count == 0) {
        std::cout << "null";
      }
      std::cout << '\n';
    }
  }
  return 0;
}
