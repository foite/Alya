#pragma once
#include <cmath>
#include <cstring>
#include <stdint.h>
#include <string>

#define C_VAR_SPACE_BYTES 16

namespace utils {
class vector2_t {
public:
  float m_x;
  float m_y;
  vector2_t() : m_x(0), m_y(0) {}
  vector2_t(float x, float y) : m_x(x), m_y(y) {}
  bool operator==(vector2_t &rhs) { return m_x == rhs.m_x && m_y == rhs.m_y; }
  bool operator==(const vector2_t &rhs) const {
    return m_x == rhs.m_x && m_y == rhs.m_y;
  }
  vector2_t operator+(const vector2_t &rhs) {
    return vector2_t(m_x + rhs.m_x, m_y + rhs.m_y);
  }
  vector2_t operator-(const vector2_t &rhs) {
    return vector2_t(m_x - rhs.m_x, m_y - rhs.m_y);
  }
  float distance(float x, float y) {
    float value_x = this->m_x - x;
    float value_y = this->m_y - y;
    return std::sqrt(value_x * value_x + value_y * value_y);
  }
  float distance(const vector2_t &other) {
    return distance(other.m_x, other.m_y);
  }
};

class vector2i_t {
public:
  int m_x;
  int m_y;
  vector2i_t() : m_x(0), m_y(0) {}
  vector2i_t(int x, int y) : m_x(x), m_y(y) {}
  vector2i_t(uint32_t x, uint32_t y) : m_x(x), m_y(y) {}
  bool operator==(vector2i_t &rhs) { return m_x == rhs.m_x && m_y == rhs.m_y; }
  bool operator!=(vector2i_t &rhs) { return m_x != rhs.m_x || m_y != rhs.m_y; }
  bool operator==(const vector2i_t &rhs) const {
    return m_x == rhs.m_x && m_y == rhs.m_y;
  }
  vector2i_t operator+(const vector2i_t &rhs) {
    return vector2i_t(m_x + rhs.m_x, m_y + rhs.m_y);
  }
  vector2i_t operator-(const vector2i_t &rhs) {
    return vector2i_t(m_x - rhs.m_x, m_y - rhs.m_y);
  }
  float distance(int x, int y) {
    float value_x = float(this->m_x) - x;
    float value_y = float(this->m_y) - y;
    return sqrt(value_x * value_x + value_y * value_y);
  }
};

class vector3_t {
public:
  float m_x;
  float m_y;
  float m_z;
  vector3_t() : m_x(0), m_y(0), m_z(0) {}
  vector3_t(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}
  bool operator==(vector3_t &rhs) {
    return m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z;
  }
  bool operator==(const vector3_t &rhs) const {
    return m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z;
  }
  vector3_t operator+(const vector3_t &rhs) {
    return vector3_t(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z);
  }
  vector3_t operator-(const vector3_t &rhs) {
    return vector3_t(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z);
  }
};

class rect_t {
public:
  float m_x;
  float m_y;
  float m_w;
  float m_h;
  rect_t() : m_x(0), m_y(0), m_w(0), m_h(0) {}
  rect_t(float x, float y, float w, float h) : m_x(x), m_y(y), m_w(w), m_h(h) {}
  bool operator==(rect_t &rhs) {
    return m_x == rhs.m_x && m_y == rhs.m_y && m_w == rhs.m_w && m_h == rhs.m_h;
  }
  bool operator==(const rect_t &rhs) const {
    return m_x == rhs.m_x && m_y == rhs.m_y && m_w == rhs.m_w && m_h == rhs.m_h;
  }
  rect_t operator+(const rect_t &rhs) {
    return rect_t(m_x + rhs.m_x, m_y + rhs.m_y, m_w + rhs.m_w, m_h + rhs.m_h);
  }
  rect_t operator-(const rect_t &rhs) {
    return rect_t(m_x - rhs.m_x, m_y - rhs.m_y, m_w - rhs.m_w, m_h - rhs.m_h);
  }
};

class variant_t {
public:
  enum class vartype_t {
    TYPE_UNUSED,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_VECTOR2,
    TYPE_VECTOR3,
    TYPE_UINT32,
    TYPE_ENTITY,
    TYPE_COMPONENT,
    TYPE_RECT,
    TYPE_INT32
  };
  variant_t() { set_defaults(); }
  variant_t(const variant_t &v) {
    set_defaults();
    *this = v;
  }
  variant_t(int32_t var) {
    set_defaults();
    set(var);
  }
  variant_t(uint32_t var) {
    set_defaults();
    set(var);
  }
  variant_t(float var) {
    set_defaults();
    set(var);
  }
  variant_t(float x, float y) {
    set_defaults();
    set(vector2_t(x, y));
  }
  variant_t(float x, float y, float z) {
    set_defaults();
    set(vector3_t(x, y, z));
  }
  variant_t(const vector2_t &v2) {
    set_defaults();
    set(v2);
  }
  variant_t(const vector3_t &v3) {
    set_defaults();
    set(v3);
  }
  variant_t(const rect_t &r) {
    set_defaults();
    set(r);
  }
  variant_t(const std::string &var) {
    set_defaults();
    set(var);
  }

  void set(const variant_t &v) {
    switch (v.get_type()) {
    case vartype_t::TYPE_FLOAT:
      set(v.get_float());
      break;
    case vartype_t::TYPE_STRING:
      set(v.get_string());
      break;
    case vartype_t::TYPE_VECTOR2:
      set(v.get_vector2());
      break;
    case vartype_t::TYPE_VECTOR3:
      set(v.get_vector3());
      break;
    case vartype_t::TYPE_UINT32:
      set(v.get_uint32());
      break;
    case vartype_t::TYPE_INT32:
      set(v.get_int32());
      break;
    case vartype_t::TYPE_RECT:
      set(v.get_rect());
      break;
    case vartype_t::TYPE_ENTITY:
    case vartype_t::TYPE_COMPONENT:
    default:
      break;
    }
  }
  void set(float var) {
    m_type = vartype_t::TYPE_FLOAT;
    *((float *)m_var) = var;
  }
  void set(uint32_t var) {
    m_type = vartype_t::TYPE_UINT32;
    *((uint32_t *)m_var) = var;
  }
  void set(int32_t var) {
    m_type = vartype_t::TYPE_INT32;
    *((int32_t *)m_var) = var;
  }
  void operator=(float var) { set(var); }
  void operator=(int32_t var) { set(var); }
  void operator=(uint32_t var) { set(var); }
  void operator=(std::string const &var) { set(var); }
  void set(std::string const &var) {
    m_type = vartype_t::TYPE_STRING;
    m_string = var;
  }
  void operator=(vector2_t const &var) { set(var); }
  void set(vector2_t const &var) {
    m_type = vartype_t::TYPE_VECTOR2;
    *((vector2_t *)m_var) = var;
  }
  void set(float x, float y) { set(vector2_t(x, y)); }
  void operator=(vector3_t const &var) { set(var); }
  void operator=(rect_t const &var) { set(var); }
  void set(vector3_t const &var) {
    m_type = vartype_t::TYPE_VECTOR3;
    *((vector3_t *)m_var) = var;
  }
  void set(rect_t const &var) {
    m_type = vartype_t::TYPE_RECT;
    *((rect_t *)m_var) = var;
  }
  void set(float x, float y, float z) { set(vector3_t(x, y, z)); }

  float &get_float() {
    if (m_type == vartype_t::TYPE_UNUSED)
      set(float(0));
    return *((float *)m_var);
  }
  int32_t &get_int32() {
    if (m_type == vartype_t::TYPE_UNUSED)
      set(int32_t(0));
    return *((int32_t *)m_var);
  }
  uint32_t &get_uint32() {
    if (m_type == vartype_t::TYPE_UNUSED)
      set(uint32_t(0));
    return *((uint32_t *)m_var);
  }
  std::string &get_string() { return m_string; }
  vector2_t &get_vector2() {
    if (m_type == vartype_t::TYPE_UNUSED)
      set(vector2_t(0, 0));
    return *((vector2_t *)m_var);
  }
  vector3_t &get_vector3() {
    if (m_type == vartype_t::TYPE_UNUSED)
      set(vector3_t(0, 0, 0));
    return *((vector3_t *)m_var);
  }
  rect_t &get_rect() {
    if (m_type == vartype_t::TYPE_UNUSED)
      set(rect_t(0, 0, 0, 0));
    return *((rect_t *)m_var);
  }

  const float &get_float() const { return *((float *)m_var); }
  const int32_t &get_int32() const { return *((int32_t *)m_var); }
  const uint32_t &get_uint32() const { return *((uint32_t *)m_var); }
  const std::string &get_string() const { return m_string; }
  const vector2_t &get_vector2() const { return *((vector2_t *)m_var); }
  const vector3_t &get_vector3() const { return *((vector3_t *)m_var); }
  const rect_t &get_rect() const { return *((rect_t *)m_var); }

  vartype_t get_type() const { return m_type; }

  variant_t &operator=(const variant_t &rhs) {
    m_type = rhs.m_type;
    m_pointer = rhs.m_pointer;
    memcpy(m_var, rhs.m_var, C_VAR_SPACE_BYTES);
    m_string = rhs.m_string;
    return *this;
  }
  variant_t &operator+=(const variant_t &rhs) {
    if (get_type() == rhs.get_type()) {
      switch (get_type()) {
      case vartype_t::TYPE_FLOAT:
        set(get_float() + rhs.get_float());
        break;
      case vartype_t::TYPE_STRING:
        set(get_string() + rhs.get_string());
        break;
      case vartype_t::TYPE_VECTOR2:
        set(get_vector2() + rhs.get_vector2());
        break;
      case vartype_t::TYPE_VECTOR3:
        set(get_vector3() + rhs.get_vector3());
        break;
      case vartype_t::TYPE_UINT32:
        set(get_uint32() + rhs.get_uint32());
        break;
      case vartype_t::TYPE_INT32:
        set(get_int32() + rhs.get_int32());
        break;
      default:
        break;
      }
    }
    return *this;
  }
  variant_t &operator-=(const variant_t &rhs) {
    if (get_type() == rhs.get_type()) {
      switch (get_type()) {
      case vartype_t::TYPE_FLOAT:
        set(get_float() - rhs.get_float());
        break;
      case vartype_t::TYPE_VECTOR2:
        set(get_vector2() - rhs.get_vector2());
        break;
      case vartype_t::TYPE_VECTOR3:
        set(get_vector3() - rhs.get_vector3());
        break;
      case vartype_t::TYPE_UINT32:
        set(get_uint32() - rhs.get_uint32());
        break;
      case vartype_t::TYPE_INT32:
        set(get_int32() - rhs.get_int32());
        break;
      default:
        break;
      }
    }
    return *this;
  }
  bool operator==(const variant_t &rhs) const {
    if (get_type() != rhs.get_type())
      return false;
    switch (get_type()) {
    case vartype_t::TYPE_UNUSED:
      return true;
    case vartype_t::TYPE_FLOAT:
      return get_float() == rhs.get_float();
    case vartype_t::TYPE_STRING:
      return get_string() == rhs.get_string();
    case vartype_t::TYPE_VECTOR2:
      return get_vector2() == rhs.get_vector2();
    case vartype_t::TYPE_VECTOR3:
      return get_vector3() == rhs.get_vector3();
    case vartype_t::TYPE_UINT32:
      return get_uint32() == rhs.get_uint32();
    case vartype_t::TYPE_RECT:
      return get_rect() == rhs.get_rect();
    case vartype_t::TYPE_INT32:
      return get_int32() == rhs.get_int32();
    case vartype_t::TYPE_ENTITY:
    case vartype_t::TYPE_COMPONENT:
    default:
      return false;
    }
  }
  bool operator!=(const variant_t &rhs) const { return !operator==(rhs); }
  friend class variantlist_t;

private:
  void set_defaults() { m_type = vartype_t::TYPE_UNUSED; }
  vartype_t m_type;
  void *m_pointer;
  union {
    uint8_t m_var[C_VAR_SPACE_BYTES];
    float m_as_floats[4];
    uint32_t m_as_uint32s[4];
    int32_t m_as_int32s[4];
  };
  std::string m_string;
};
inline variant_t operator+(variant_t lhs, const variant_t &rhs) {
  lhs += rhs;
  return lhs;
}
inline variant_t operator-(variant_t lhs, const variant_t &rhs) {
  lhs -= rhs;
  return lhs;
}
#define C_MAX_VARIANT_LIST_PARMS 7
class variantlist_t {
public:
  variantlist_t() = default;

  variant_t &get(int parmNum) { return m_variant[parmNum]; }
  variant_t &operator[](int num) { return m_variant[num]; }

  variantlist_t(const std::string &string) { m_variant[0] = variant_t(string); }
  variantlist_t(std::initializer_list<variant_t> variants) {
    std::copy(variants.begin(), variants.end(), m_variant);
  }

  bool serialize_from_mem(uint8_t *data, int *read = nullptr) {
    uint8_t *p = data;
    uint8_t count = *(p++);
    for (int i = 0; i < count; i++) {
      uint8_t index = *(p++);
      uint8_t type = *(p++);
      if (!deserialize_variant(p, index, variant_t::vartype_t(type))) {
        if (read)
          *read = 0;
        return false;
      }
    }
    if (read)
      *read = int(p - data);
    return true;
  }

private:
  bool deserialize_variant(uint8_t *&p, uint8_t index,
                           variant_t::vartype_t type) {
    switch (type) {
    case variant_t::vartype_t::TYPE_STRING: {
      uint32_t len;
      memcpy(&len, p, 4);
      p += 4;
      std::string v(len, '\0');
      memcpy(&v[0], p, len);
      p += len;
      m_variant[index].set(v);
      break;
    }
    case variant_t::vartype_t::TYPE_UINT32: {
      uint32_t v;
      memcpy(&v, p, sizeof(v));
      p += sizeof(v);
      m_variant[index].set(v);
      break;
    }
    case variant_t::vartype_t::TYPE_INT32: {
      int32_t v;
      memcpy(&v, p, sizeof(v));
      p += sizeof(v);
      m_variant[index].set(v);
      break;
    }
    case variant_t::vartype_t::TYPE_FLOAT: {
      float v;
      memcpy(&v, p, sizeof(v));
      p += sizeof(v);
      m_variant[index].set(v);
      break;
    }
    case variant_t::vartype_t::TYPE_VECTOR2: {
      vector2_t v;
      memcpy(&v, p, sizeof(v));
      p += sizeof(v);
      m_variant[index].set(v);
      break;
    }
    case variant_t::vartype_t::TYPE_VECTOR3: {
      vector3_t v;
      memcpy(&v, p, sizeof(v));
      p += sizeof(v);
      m_variant[index].set(v);
      break;
    }
    case variant_t::vartype_t::TYPE_RECT: {
      rect_t v;
      memcpy(&v, p, sizeof(v));
      p += sizeof(v);
      m_variant[index].set(v);
      break;
    }
    default:
      return false;
    }
    return true;
  }

  variant_t m_variant[C_MAX_VARIANT_LIST_PARMS];
};
} // namespace utils