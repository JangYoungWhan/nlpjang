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
#include <vector>

int main(int argc, char* argv[])
{
  int size = 4;

#if 0 // test progressbar
  for (int i=0; i<size; ++i)
    nlp::jang::garnut::ProgressBar<int>::dispalyPrgressBar(i, size);
  std::cout << std::endl;
#endif

#if 0 // test for unicode to utf8 converter.
  std::wstring src_utf8 = L"hi";
  std::wstring src_unicode_bmp;
  src_unicode_bmp.push_back(0x0061); // 1-byte
  src_unicode_bmp.push_back(0x04E8); // 2-byte
  src_unicode_bmp.push_back(0xAC00); // 3-byte
  std::vector<unsigned int> src_unicode_smp;
  src_unicode_smp.push_back(0x0061); // 1-byte
  src_unicode_smp.push_back(0x04E8); // 2-byte
  src_unicode_smp.push_back(0xAC00); // 3-byte
  src_unicode_smp.push_back(0x0010AC00); // 4-byte

  std::string dst;

  nlp::jang::garnut::EncodingConverter::convertFromUnicodeToUtf8(src_unicode_bmp, dst);
  for (auto c=dst.begin(); c!=dst.end(); ++c)
  {
    printf("%02X ", static_cast<unsigned char>(*c));
  } puts("\n");

  dst.clear();
  nlp::jang::garnut::EncodingConverter::convertFromUnicodeToUtf8(src_unicode_smp, dst);
  for (auto c=dst.begin(); c!=dst.end(); ++c)
  {
    printf("%02X ", static_cast<unsigned char>(*c));
  } puts("\n");
#endif

#if 1 // test for utf8 to unicode converter.
  std::string utf8_1byte = "hello world!";
  std::string utf8_2byte;
  std::string utf8_3byte;
  std::string utf8_41byte;

  std::wstring dst;

  nlp::jang::garnut::EncodingConverter::convertFromUtf8ToUnicode(utf8_1byte, dst);
  for (auto c=dst.begin(); c!=dst.end(); ++c)
  {
    printf("%02X ", static_cast<unsigned char>(*c));
  } puts("\n");
#endif

  return 0;
}