/*
 *  File    : unittest.cpp
 *  Author  : YW. Jang
 *  Date    : 2015.08.01.Sat
 *
 *  Copyright 2015, YW. Jang, All rights reserved.
 */

#define RUN_EDIT_DISTANCE
#ifdef RUN_EDIT_DISTANCE
#include "garnut/include/editdistance.hpp"
#include <string>
#include <iostream>
#endif

#ifdef RUN_AUTO_SPACE
#include "amuthyst/include/autospace.hpp"
#include "aquamaron/include/hiddenmarkovmodel.hpp"
#endif

#ifdef RUN_SENTENCE_DEALER
#include "sentencedealer.hpp"
#endif

#ifdef RUN_NGRAM_USAGE
#include "ngram.hpp"
#endif

#ifdef RUN_PROGRESS_BAR
#include "progressbar.hpp"
#endif

#ifdef RUN_ENCODING_CONVERTER
#include "encconv.hpp"
#endif

int main(int argc, char* argv[])
{

#ifdef RUN_EDIT_DISTANCE
  std::string str1 = "i am a boy.";
  std::string str2 = "i am a bay.";

  auto ed = nlp::jang::garnut::EditDistance<std::string>::calculateLevenshteinDistance(str1, str2);
  std::cout << "E.D. = " << ed << std::endl;
#endif

#ifdef RUN_AUTO_SPACE
  nlp::jang::amuthyst::AutoSpacer autoSpacer(3);
  try
  {
    autoSpacer.train("C:/dummy/tr.txt");
    autoSpacer.test("C:/dummy/te.txt");
  }
  catch (const char * e)
  {
    std::cerr << e << std::endl;
  }
#endif

#ifdef RUN_SENTENCE_DEALER
  const std::string str("hello  world !");
  const std::wstring wstr(L"hello world !");
  const std::string tokens;
  const std::wstring wtokens;
  
  //nlp::jang::garnut::splitStringToNgram<std::string>(str, split_result, " ", false);
  //nlp::jang::garnut::Ngram<std::string> split_result;
  //nlp::jang::garnut::SentenceDealer::insertStartEndTags(str, split_result, 3);
  std::vector<nlp::jang::garnut::Ngram<std::string>> split_result;
  nlp::jang::garnut::SentenceDealer::convertSentenceToWordNgram(str, split_result, 3);
#endif

#ifdef RUN_NGRAM_USAGE
  nlp::jang::garnut::Ngram<std::string> ngram(3);

  ngram.push_back("hello");
  ngram.push_back("my");
  ngram.push_back("world");

  for (int i=0; i<ngram.getN(); ++i)
  {
    auto pos = ngram[i];
    std::cout << pos << std::endl;
  }
  std::cout << ngram << std::endl;
#endif

#ifdef RUN_PROGRESS_BAR
  int size = 4;
  for (int i=0; i<size; ++i)
    nlp::jang::garnut::ProgressBar<int>::dispalyPrgressBar(i, size);
  std::cout << std::endl;
#endif

#ifdef RUN_ENCODING_CONVERTER
  // test for unicode tu utf8 converter.
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

  nlp::jang::garnut::EncodingConverter::convertUnicodeToUtf8(src_unicode_bmp, dst);
  for (auto c=dst.begin(); c!=dst.end(); ++c)
  {
    printf("%02X ", static_cast<unsigned char>(*c));
  } puts("\n");

  dst.clear();
  nlp::jang::garnut::EncodingConverter::convertUnicodeToUtf8(src_unicode_smp, dst);
  for (auto c=dst.begin(); c!=dst.end(); ++c)
  {
    printf("%02X ", static_cast<unsigned char>(*c));
  } puts("\n");
  
  // test for utf8 to unicode converter.
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

  dst.clear();
  puts("before : ");
  for (auto c=src_utf8_bmp.begin(); c!=src_utf8_bmp.end(); ++c)
  {
    printf("%02X ", static_cast<unsigned char>(*c));
  } puts("\n");
  nlp::jang::garnut::EncodingConverter::convertUtf8ToUnicode(src_utf8_bmp, dst);
  puts("after : ");
  for (auto c=dst.begin(); c!=dst.end(); ++c)
  {
    printf("%02X ", (*c));
  } puts("\n");
#endif

  return 0;
}