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

//#define REFINE_SENTENCE

namespace nlp { namespace jang { namespace amuthyst {

enum EmptySpaceTag
{
  SentenceBegin,
  SentenceEnd,
  WordBegin,
  WordIng,
  WordEnd,
  //SpaceBegin,
  //SpaceIng,  
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
  bool test(const std::string& test_corpus_path);

private:
  bool assignIDs(const std::string& train_corpus_path);
  void refineSentence(std::wstring& target) const;
  void assginStartEndChar();
  void findSpaceTag(const std::wstring& src,
                    garnut::Ngram<std::wstring::value_type>& letters,
                    garnut::Ngram<EmptySpaceTag>& tags);
  void viterbiSearch(const garnut::Ngram<std::wstring::value_type>& words,
                     garnut::Ngram<EmptySpaceTag>& hidden_state);

private:
  float transition_lprob(int v, int w, int u) const;
  float emission_lprob(int x_k, int v) const;

private:
  unsigned int n_;
};

} // namespace amuthyst
} // namespace jang
} // namespace nlp

#endif