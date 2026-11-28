#pragma once

#include <istream>
#include <iterator>
#include <memory>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "src/char.h"
#include "src/compressor.h"

struct CharacterCode {
  uint32_t code;
  uint32_t length;

  friend std::ostream& operator<<(std::ostream&, const CharacterCode&);
};

class HuffmanTree {
  template <typename T>
  using OrChar = std::variant<T, Char>;

  template <typename T, typename F>
  static auto MapNode(OrChar<T>&& or_char, F&& f);

  struct Node {
    OrChar<std::unique_ptr<Node>> zero;
    OrChar<std::unique_ptr<Node>> one;
  };

  using NodePtr = std::unique_ptr<Node>;

  class Iterator {
   public:
    using iterator_concept = std::input_iterator_tag;
    using value_type = std::pair<Char, CharacterCode>;

    explicit Iterator(const Node& root) {
      PushNode(&root);
    }

    struct EndTag {};
    explicit Iterator(EndTag) {}

    value_type operator*() const;

    Iterator& operator++();

    bool operator==(const Iterator& other) {
      return mask_ == other.mask_ && stack_.size() == other.stack_.size();
    }
    bool operator!=(const Iterator& other) {
      return !(*this == other);
    }

   private:
    void PushNode(const Node* n);

    std::vector<const Node*> stack_;
    uint32_t mask_ = 0;
  };

 public:
  using value_type = Iterator::value_type;
  using const_iterator = Iterator;

  static HuffmanTree BuildFromFrequencies(
      absl::Span<const std::pair<Char, size_t>> char_frequencies);

  size_t Depth() const {
    return DepthImpl(root_);
  }

  Iterator begin() const {
    return Iterator(root_);
  }
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  Iterator end() const {
    return Iterator(Iterator::EndTag{});
  }

  friend std::ostream& operator<<(std::ostream& out, const HuffmanTree& tree) {
    PrintImpl(out, tree.root_, 0);
    return out;
  }

 private:
  explicit HuffmanTree(Node root) : root_(std::move(root)) {}

  static size_t NodeDepth(const OrChar<NodePtr>& n);
  static size_t DepthImpl(const Node& node);

  static void PrintImpl(std::ostream& out, const Node& node, size_t depth);

  Node root_;
};

class HuffmanCode {
 public:
  using value_type = std::pair<Char, CharacterCode>;
  using const_iterator =
      absl::flat_hash_map<Char, CharacterCode>::const_iterator;

  static HuffmanCode Build(const HuffmanTree& tree);

  CharacterCode code(Char c) const {
    return map_.at(c);
  }

  const_iterator begin() const {
    return map_.begin();
  }
  const_iterator end() const {
    return map_.end();
  }

 private:
  explicit HuffmanCode(absl::flat_hash_map<Char, CharacterCode>&& map)
      : map_(std::move(map)) {}

  absl::flat_hash_map<Char, CharacterCode> map_;
};

class Huffman : public Compressor {
 public:
 protected:
  absl::StatusOr<std::unique_ptr<std::istream>> Compress(
      std::string input) override;
};
