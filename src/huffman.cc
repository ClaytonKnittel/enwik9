#include "src/huffman.h"

#include <istream>
#include <memory>
#include <sstream>
#include <string_view>
#include <variant>

#include "absl/container/flat_hash_map.h"
#include "absl/functional/overload.h"
#include "absl/status/statusor.h"
#include "absl/types/span.h"

#include "src/char.h"

namespace {

absl::flat_hash_map<Char, size_t> CharFrequency(std::string_view stream) {
  absl::flat_hash_map<Char, size_t> freq;
  for (Char c : Char::ParseString(stream)) {
    freq[c]++;
  }
  return freq;
}

class HuffmanCode {
 public:
  static HuffmanCode Build(const HuffmanTree& tree) {}

 private:
  struct Code {
    uint32_t code;
    uint32_t length;
  };

  explicit HuffmanCode(absl::flat_hash_map<Char, Code>&& map)
      : map_(std::move(map)) {}

  absl::flat_hash_map<Char, Code> map_;
};

}  // namespace

std::ostream& operator<<(std::ostream& out, const CharacterCode& code) {
  out << "0b";
  for (size_t i = code.length - 1; i < code.length; --i) {
    out << ((code.code >> i) & 0x1);
  }
  return out;
}

std::pair<Char, CharacterCode> HuffmanTree::Iterator::operator*() const {
  const Node* n = stack_.back();
  size_t i = stack_.size() - 1;
  auto code = CharacterCode{.code = mask_,
                            .length = static_cast<uint32_t>(stack_.size())};
  if (((mask_ >> i) & 0x1) == 0) {
    return {std::get<Char>(n->zero), code};
  } else {
    return {std::get<Char>(n->one), code};
  }
}

HuffmanTree::Iterator& HuffmanTree::Iterator::operator++() {
  uint32_t i = stack_.size() - 1;
  while (((mask_ >> i) & 0x1) != 0) {
    mask_ &= ~(1u << i);
    stack_.pop_back();
    if (i == 0) {
      return *this;
    }
    --i;
  }

  mask_ |= 1u << i;
  const auto* r = stack_[i];
  if (const auto* n = std::get_if<std::unique_ptr<Node>>(&r->one)) {
    PushNode(n->get());
  }
  return *this;
}

void HuffmanTree::Iterator::PushNode(const Node* n) {
  stack_.push_back(n);
  while (true) {
    if (std::holds_alternative<Char>(n->zero)) break;

    const Node* l = std::get<std::unique_ptr<Node>>(n->zero).get();
    stack_.push_back(l);
    n = l;
  }
}

HuffmanTree HuffmanTree::BuildFromFrequencies(
    absl::Span<const std::pair<Char, size_t>> char_frequencies) {
  std::vector<std::pair<OrChar<Node>, size_t>> nodes;
  nodes.reserve(char_frequencies.size());
  for (auto [c, count] : std::move(char_frequencies)) {
    nodes.emplace_back(c, count);
  }

  while (nodes.size() > 1) {
    absl::c_sort(nodes, [](const auto& l, const auto& r) {
      return l.second > r.second;
    });

    auto last = std::move(nodes.back());
    nodes.pop_back();
    auto second = std::move(nodes.back());
    nodes.pop_back();

    constexpr auto kToUniquePtr = [](Node&& n) {
      return std::make_unique<Node>(std::move(n));
    };
    auto zero = MapNode(std::move(second.first), kToUniquePtr);
    auto one = MapNode(std::move(last.first), kToUniquePtr);

    nodes.emplace_back(Node{.zero = std::move(zero), .one = std::move(one)},
                       last.second + second.second);
  }

  return std::visit(absl::Overload{
                        [](Char c) {
                          return HuffmanTree(Node{
                              .zero = c,
                              .one = c,
                          });
                        },
                        [](auto&& n) {
                          return HuffmanTree(std::forward<decltype(n)>(n));
                        },
                    },
                    std::move(nodes[0].first));
}

size_t HuffmanTree::NodeDepth(const OrChar<std::unique_ptr<Node>>& n) {
  return std::visit(absl::Overload{
                        [](Char) -> size_t {
                          return 0;
                        },
                        [](const std::unique_ptr<Node>& n) {
                          return DepthImpl(*n);
                        },
                    },
                    n);
}

size_t HuffmanTree::DepthImpl(const Node& node) {
  return 1 + std::max(NodeDepth(node.zero), NodeDepth(node.one));
}

void HuffmanTree::PrintImpl(std::ostream& out, const Node& node, size_t depth) {
  if (depth > 7) return;

  out << std::string(depth * 2, ' ') << "0:";

  if (const Char* c = std::get_if<Char>(&node.zero)) {
    out << " " << *c << std::endl;
  } else {
    out << std::endl;
    const Node& zero = *std::get<std::unique_ptr<Node>>(node.zero);
    PrintImpl(out, zero, depth + 1);
  }

  out << std::string(depth * 2, ' ') << "1:";
  if (const Char* c = std::get_if<Char>(&node.one)) {
    out << " " << *c << std::endl;
  } else {
    out << std::endl;
    const Node& zero = *std::get<std::unique_ptr<Node>>(node.one);
    PrintImpl(out, zero, depth + 1);
  }
}

absl::StatusOr<std::unique_ptr<std::istream>> Huffman::Compress(
    std::string input) {
  absl::flat_hash_map<Char, size_t> chars = CharFrequency(input);

  std::ostringstream out;
  out << "Total unique chars: " << chars.size() << std::endl;

  std::vector<std::pair<Char, size_t>> sorted_chars(chars.begin(), chars.end());
  absl::c_sort(sorted_chars, [](const auto& lhs, const auto& rhs) {
    return lhs.second > rhs.second;
  });
  // for (auto [c, count] : sorted_chars) {
  //   out << c << " " << count << std::endl;
  // }

  auto tree = HuffmanTree::BuildFromFrequencies(sorted_chars);
  out << "Tree depth: " << tree.Depth() << std::endl;
  out << tree;

  return std::make_unique<std::istringstream>(std::move(out).str());
}
