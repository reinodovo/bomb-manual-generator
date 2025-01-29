#include <iostream>
#include <string>

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

std::string base64Encode(const std::string &input) {
  std::string encoded;
  int val = 0, valb = -6;
  for (unsigned char c : input) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6) {
    encoded.push_back(base64_chars[((val << (6 + valb)) & 0x3F)]);
  }
  while (encoded.size() % 4) {
    encoded.push_back('='); // Padding
  }
  return encoded;
}
