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

#include "src/bit_stream.h"
#include "src/char.h"

namespace {

absl::flat_hash_map<Char, size_t> CharFrequency(std::string_view stream) {
  absl::flat_hash_map<Char, size_t> freq;
  for (Char c : Char::ParseString(stream)) {
    freq[c]++;
  }
  return freq;
}

class HuffmanTree {
  template <typename T>
  using OrChar = std::variant<T, Char>;

  template <typename T, typename F>
  static decltype(auto) MapNode(OrChar<T>&& or_char, F&& f) {
    using U = decltype(f(std::declval<T&&>()));
    return std::visit(absl::Overload{
                          [](Char c) -> OrChar<U> {
                            return c;
                          },
                          [&f](T&& t) -> OrChar<U> {
                            return std::forward<F>(f)(std::move(t));
                          },
                      },
                      std::move(or_char));
  }

  struct Node {
    OrChar<std::unique_ptr<Node>> zero;
    OrChar<std::unique_ptr<Node>> one;
  };

 public:
  static HuffmanTree BuildFromFrequencies(
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

  void Write(Char c, BitStream& bit_stream) const {}

  size_t Depth() const {
    return DepthImpl(root_);
  }

  friend std::ostream& operator<<(std::ostream& out, const HuffmanTree& tree) {
    PrintImpl(out, tree.root_, 0);
    return out;
  }

 private:
  explicit HuffmanTree(Node root) : root_(std::move(root)) {}

  static size_t NodeDepth(const OrChar<std::unique_ptr<Node>>& n) {
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

  static size_t DepthImpl(const Node& node) {
    return 1 + std::max(NodeDepth(node.zero), NodeDepth(node.one));
  }

  static void PrintImpl(std::ostream& out, const Node& node, size_t depth) {
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

  Node root_;
};

}  // namespace

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
