/*
 *  File    : autospace.hpp
 *  Author  : YW. Jang
 *  Date    : 2015.08.24.Mon
 *
 *  Copyright 2015, YW. Jang, All rights reserved.
 */

#ifndef AUTO_SPACE_HPP_
#define AUTO_SPACE_HPP_

#include "aquamaron/include/hiddenmarkovmodel.hpp"

#include <string>

namespace nlp { namespace jang { namespace amuthyst {

enum EmptySpaceTag
{
  AfterNonSpace,
  AfterSpace,
  BeginOrEnd,
  NumOfTags
};

class AutoSpacer : protected aquamaron::HiddenMarkovModel<std::wstring::value_type, EmptySpaceTag>
{
public:
  AutoSpacer();
  AutoSpacer(unsigned int n);
  virtual ~AutoSpacer();

public:
  bool train(const std::string& train_corpus_path);

private:
  bool assignIDs(const std::string& train_corpus_path);
  void findSpaceTag(const std::wstring& src, garnut::Ngram<std::wstring::value_type>& letters, garnut::Ngram<EmptySpaceTag>& tags);

private:
  unsigned int n_;
};

} // namespace amuthyst
} // namespace jang
} // namespace nlp

#endif