#include <iostream>
#include <string>

static const char b64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64Encode(const std::string &bindata) {
  using ::std::numeric_limits;
  using ::std::string;

  if (bindata.size() > (numeric_limits<string::size_type>::max() / 4u) * 3u) {
    throw ::std::length_error("Converting too large a string to base64.");
  }

  const ::std::size_t binlen = bindata.size();
  // Use = signs so the end is properly padded.
  string retval((((binlen + 2) / 3) * 4), '=');
  ::std::size_t outpos = 0;
  int bits_collected = 0;
  unsigned int accumulator = 0;
  const string::const_iterator binend = bindata.end();

  for (string::const_iterator i = bindata.begin(); i != binend; ++i) {
    accumulator = (accumulator << 8) | (*i & 0xffu);
    bits_collected += 8;
    while (bits_collected >= 6) {
      bits_collected -= 6;
      retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
    }
  }
  if (bits_collected > 0) { // Any trailing bits that are missing.
    assert(bits_collected < 6);
    accumulator <<= 6 - bits_collected;
    retval[outpos++] = b64_table[accumulator & 0x3fu];
  }
  assert(outpos >= (retval.size() - 2));
  assert(outpos <= retval.size());
  return retval;
}
