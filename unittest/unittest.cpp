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
  std::string src_utf8_bmp = "hi"; // 1-byte

  src_utf8_bmp.push_back(0xD0); // 2-byte
  src_utf8_bmp.push_back(0x96);

  src_utf8_bmp.push_back(0xED); // 3-byte
  src_utf8_bmp.push_back(0x95);
  src_utf8_bmp.push_back(0x9C);
  src_utf8_bmp.push_back(0xEA);
  src_utf8_bmp.push_back(0xB8);
  src_utf8_bmp.push_back(0x80);

  src_utf8_bmp.push_back(0xF4); // 4-byte
  src_utf8_bmp.push_back(0x8A);
  src_utf8_bmp.push_back(0xB0);
  src_utf8_bmp.push_back(0x80);

  std::string utf8_41byte;

  std::wstring dst;
  puts("before : ");
  for (auto c=src_utf8_bmp.begin(); c!=src_utf8_bmp.end(); ++c)
  {
    printf("%02X ", static_cast<unsigned char>(*c));
  } puts("\n");
  nlp::jang::garnut::EncodingConverter::convertFromUtf8ToUnicode(src_utf8_bmp, dst);
  puts("after : ");
  for (auto c=dst.begin(); c!=dst.end(); ++c)
  {
    printf("%02X ", (*c));
  } puts("\n");
#endif

  return 0;
}