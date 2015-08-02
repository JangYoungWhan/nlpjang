/*
 *  File    : unittest.cpp
 *  Author  : YW. Jang
 *  Date    : 2015.08.01.Sat
 *
 *  Copyright 2015, YW. Jang, All rights reserved.
 */

#include "progressbar.hpp"
#include "encconv.hpp"

#include <string>

int main(int argc, char* argv[])
{
  int size = 4;

#if 0 // test progressbar
  for (int i=0; i<size; ++i)
    nlp::jang::garnut::ProgressBar<int>::dispalyPrgressBar(i, size);
  std::cout << std::endl;
#endif

#if 1 // 
  std::wstring src_utf8 = L"hi";
  std::wstring src_unicode;
  //src_unicode.push_back(0x0001);
  //src_unicode.push_back(0x001A);
  src_unicode.push_back(0x0061); // 1-byte
  src_unicode.push_back(0x04E8); // 2-byte
  src_unicode.push_back(0xAC00); // 3-byte

  std::string dst;

  nlp::jang::garnut::EncodingConverter::convertFromUnicodeToUtf8(src_unicode, dst);
  for (auto c=dst.begin(); c!=dst.end(); ++c)
  {
    printf("%02X\n", static_cast<unsigned char>(*c));
  }

#endif
  return 0;
}