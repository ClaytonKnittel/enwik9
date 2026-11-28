#include "src/huffman.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::UnorderedElementsAre;

namespace {

MATCHER_P(EncodedLength, expected_length, "") {
  return arg.length == expected_length;
}

MATCHER_P2(HasEncodedLength, expected_char, expected_length, "") {
  return arg.first == expected_char && arg.second.length == expected_length;
}

TEST(HuffmanTest, Simple) {
  auto tree = HuffmanTree::BuildFromFrequencies({
      {Char::ascii('a'), 10},
      {Char::ascii('b'), 5},
  });

  EXPECT_THAT(tree,
              UnorderedElementsAre(HasEncodedLength(Char::ascii('a'), 1),
                                   HasEncodedLength(Char::ascii('b'), 1)));
}

TEST(HuffmanTest, Depth2) {
  auto tree = HuffmanTree::BuildFromFrequencies({
      {Char::ascii('a'), 10},
      {Char::ascii('b'), 5},
      {Char::ascii('c'), 3},
  });

  EXPECT_THAT(tree,
              UnorderedElementsAre(HasEncodedLength(Char::ascii('a'), 1),
                                   HasEncodedLength(Char::ascii('b'), 2),
                                   HasEncodedLength(Char::ascii('c'), 2)));
}

TEST(HuffmanTest, Depth3) {
  auto tree = HuffmanTree::BuildFromFrequencies({
      {Char::ascii('a'), 10},
      {Char::ascii('b'), 5},
      {Char::ascii('c'), 3},
      {Char::ascii('d'), 1},
  });

  EXPECT_THAT(tree,
              UnorderedElementsAre(HasEncodedLength(Char::ascii('a'), 1),
                                   HasEncodedLength(Char::ascii('b'), 2),
                                   HasEncodedLength(Char::ascii('c'), 3),
                                   HasEncodedLength(Char::ascii('d'), 3)));
}

TEST(HuffmanTest, ComplexTree) {
  auto tree = HuffmanTree::BuildFromFrequencies({
      {Char::ascii('a'), 10},
      {Char::ascii('b'), 11},
      {Char::ascii('c'), 12},
      {Char::ascii('d'), 13},
      {Char::ascii('e'), 14},
      {Char::ascii('f'), 15},
      {Char::ascii('g'), 40},
  });

  EXPECT_THAT(tree,
              UnorderedElementsAre(HasEncodedLength(Char::ascii('a'), 3),
                                   HasEncodedLength(Char::ascii('b'), 3),
                                   HasEncodedLength(Char::ascii('c'), 3),
                                   HasEncodedLength(Char::ascii('d'), 3),
                                   HasEncodedLength(Char::ascii('e'), 3),
                                   HasEncodedLength(Char::ascii('f'), 3),
                                   HasEncodedLength(Char::ascii('g'), 2)));
}

TEST(HuffmanTest, Unicode) {
  auto tree = HuffmanTree::BuildFromFrequencies({
      {Char::ParseCharacter("你"), 2},
      {Char::ParseCharacter("好"), 3},
      {Char::ParseCharacter("凯"), 4},
      {Char::ParseCharacter("兰"), 6},
  });

  EXPECT_THAT(tree, UnorderedElementsAre(
                        HasEncodedLength(Char::ParseCharacter("你"), 3),
                        HasEncodedLength(Char::ParseCharacter("好"), 3),
                        HasEncodedLength(Char::ParseCharacter("凯"), 2),
                        HasEncodedLength(Char::ParseCharacter("兰"), 1)));
}

TEST(HuffmanTest, EncodeSimple) {
  auto tree = HuffmanTree::BuildFromFrequencies({
      {Char::ascii('a'), 10},
      {Char::ascii('b'), 5},
  });
  auto code = HuffmanCode::Build(tree);

  EXPECT_THAT(code.code(Char::ascii('a')), EncodedLength(1));
  EXPECT_THAT(code.code(Char::ascii('b')), EncodedLength(1));

  EXPECT_THAT(code,
              UnorderedElementsAre(HasEncodedLength(Char::ascii('a'), 1),
                                   HasEncodedLength(Char::ascii('b'), 1)));
}

}  // namespace
