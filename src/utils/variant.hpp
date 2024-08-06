#pragma once
#include <cstring>
#include <endian.h>
#include <magic_enum.hpp>
#include <string>
#include <variant>
#include <vector>

namespace utils {
enum class VariantType : uint8_t {
  Unknown = 0,
  Float = 1,
  String = 2,
  Vec2 = 3,
  Vec3 = 4,
  Unsigned = 5,
  Signed = 9
};

class Variant {
public:
  using Vec2 = std::pair<float, float>;
  using Vec3 = std::tuple<float, float, float>;
  using VariantValue = std::variant<float, std::string, Vec2, Vec3, uint32_t,
                                    int32_t, std::monostate>;

  Variant() : value_(std::monostate{}) {}
  Variant(float v) : value_(v) {}
  Variant(const std::string &v) : value_(v) {}
  Variant(const Vec2 &v) : value_(v) {}
  Variant(const Vec3 &v) : value_(v) {}
  Variant(uint32_t v) : value_(v) {}
  Variant(int32_t v) : value_(v) {}

  std::string as_string() const {
    if (std::holds_alternative<float>(value_)) {
      return std::to_string(std::get<float>(value_));
    } else if (std::holds_alternative<std::string>(value_)) {
      return std::get<std::string>(value_);
    } else if (std::holds_alternative<Vec2>(value_)) {
      auto [x, y] = std::get<Vec2>(value_);
      return std::to_string(x) + ", " + std::to_string(y);
    } else if (std::holds_alternative<Vec3>(value_)) {
      auto [x, y, z] = std::get<Vec3>(value_);
      return std::to_string(x) + ", " + std::to_string(y) + ", " +
             std::to_string(z);
    } else if (std::holds_alternative<uint32_t>(value_)) {
      return std::to_string(std::get<uint32_t>(value_));
    } else if (std::holds_alternative<int32_t>(value_)) {
      return std::to_string(std::get<int32_t>(value_));
    } else {
      return "Unknown";
    }
  }

  int32_t as_int32() const {
    if (std::holds_alternative<int32_t>(value_)) {
      return std::get<int32_t>(value_);
    }
    return 0;
  }

  Vec2 as_vec2() const {
    if (std::holds_alternative<Vec2>(value_)) {
      return std::get<Vec2>(value_);
    }
    return {0.0f, 0.0f};
  }

private:
  VariantValue value_;
};

class VariantList {
public:
  static VariantList deserialize(const uint8_t *data) {
    VariantList list;
    size_t offset = 0;
    uint8_t numVariants = data[offset++];
    list.variants_.reserve(numVariants);

    for (uint8_t i = 0; i < numVariants; ++i) {
      uint8_t index = data[offset++];
      VariantType var_type = magic_enum::enum_cast<VariantType>(data[offset++])
                                 .value_or(VariantType::Unknown);

      switch (var_type) {
      case VariantType::Float: {
        float value;
        std::memcpy(&value, &data[offset], sizeof(float));
        value = le32toh(value);
        offset += sizeof(float);
        list.variants_.emplace_back(value);
        break;
      }
      case VariantType::String: {
        uint32_t len;
        std::memcpy(&len, &data[offset], sizeof(uint32_t));
        len = le32toh(len);
        offset += sizeof(uint32_t);
        std::string value(reinterpret_cast<const char *>(&data[offset]), len);
        offset += len;
        list.variants_.emplace_back(value);
        break;
      }
      case VariantType::Vec2: {
        float x, y;
        std::memcpy(&x, &data[offset], sizeof(float));
        x = le32toh(x);
        offset += sizeof(float);
        std::memcpy(&y, &data[offset], sizeof(float));
        y = le32toh(y);
        offset += sizeof(float);
        list.variants_.emplace_back(Variant::Vec2{x, y});
        break;
      }
      case VariantType::Vec3: {
        float x, y, z;
        std::memcpy(&x, &data[offset], sizeof(float));
        x = le32toh(x);
        offset += sizeof(float);
        std::memcpy(&y, &data[offset], sizeof(float));
        y = le32toh(y);
        offset += sizeof(float);
        std::memcpy(&z, &data[offset], sizeof(float));
        z = le32toh(z);
        offset += sizeof(float);
        list.variants_.emplace_back(Variant::Vec3{x, y, z});
        break;
      }
      case VariantType::Unsigned: {
        uint32_t value;
        std::memcpy(&value, &data[offset], sizeof(uint32_t));
        value = le32toh(value);
        offset += sizeof(uint32_t);
        list.variants_.emplace_back(value);
        break;
      }
      case VariantType::Signed: {
        int32_t value;
        std::memcpy(&value, &data[offset], sizeof(int32_t));
        value = le32toh(value);
        offset += sizeof(int32_t);
        list.variants_.emplace_back(value);
        break;
      }
      case VariantType::Unknown:
      default:
        list.variants_.emplace_back();
        break;
      }
    }

    return list;
  }

  const Variant *get(size_t index) const {
    if (index < variants_.size()) {
      return &variants_[index];
    }
    return nullptr;
  }

private:
  std::vector<Variant> variants_;
};
} // namespace utils