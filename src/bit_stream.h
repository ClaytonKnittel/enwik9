#pragma once

#include <compare>
#include <limits>
#include <vector>

class BitStream {
  static constexpr size_t kBitWidth = std::numeric_limits<uint64_t>::digits;

  class BitIterator {
   public:
    BitIterator(const uint64_t* buffer, size_t pos)
        : buffer_(buffer), pos_(pos) {}

    BitIterator& operator++() {
      pos_++;
      return *this;
    }
    BitIterator operator++(int) {
      auto copy = *this;
      ++*this;
      return copy;
    }

    bool operator*() const {
      size_t i = pos_ / kBitWidth;
      size_t b = pos_ % kBitWidth;
      return ((buffer_[i] >> b) & 0x1) != 0;
    }

    friend std::strong_ordering operator<=>(const BitIterator& l,
                                            const BitIterator& r) {
      return l.pos_ <=> r.pos_;
    }

   private:
    const uint64_t* buffer_;
    size_t pos_;
  };

 public:
  void Put(bool bit) {
    if (pos_ % kBitWidth == 0) {
      buffer_.push_back(static_cast<uint64_t>(bit));
    } else {
      buffer_.back() |= uint64_t{1} << (pos_ % kBitWidth);
    }
    pos_++;
  }

  BitIterator begin() const {
    return BitIterator(buffer_.data(), 0);
  }
  BitIterator end() const {
    return BitIterator(buffer_.data(), pos_);
  }

 private:
  std::vector<uint64_t> buffer_;
  size_t pos_ = 0;
};
