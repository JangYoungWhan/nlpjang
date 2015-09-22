/*
 *  File    : autospace.cpp
 *  Author  : YW. Jang
 *  Date    : 2015.08.24.Mon
 *
 *  Copyright 2015, YW. Jang, All rights reserved.
 */

#include "autospace.hpp"

#include "garnut/include/encconv.hpp"
#include "garnut/include/tokenizer.hpp"
#include "garnut/include/ngram.hpp"
#include "garnut/include/sentencedealer.hpp"

#include <fstream>

namespace nlp { namespace jang { namespace amuthyst {

AutoSpacer::AutoSpacer()
{

}

AutoSpacer::AutoSpacer(unsigned int n)
  :n_(n), HiddenMarkovModel(n)
{

}

AutoSpacer::~AutoSpacer()
{

}

bool AutoSpacer::train(const std::string& train_corpus_path)
{
  if (!assignIDs(train_corpus_path))
    return false;

  prepareLogProbMatrix();

  std::ifstream ofs(train_corpus_path);

  if (ofs.fail())
  {
    return false;
  }

  std::string line;
  while (std::getline(ofs, line))
  {
    if (line.length() == 0)
      continue;

    std::wstring wline;
    garnut::EncodingConverter::convertUtf8ToUnicode(line, wline);

    // Update emission probabilities.
    garnut::Ngram<std::wstring::value_type> letters;
    garnut::Ngram<EmptySpaceTag> tags;
    findSpaceTag(wline, letters, tags);
    countEmission(letters, tags);

    // Update trasition probabilities.
    tags.attachTags(EmptySpaceTag::BeginOrEnd, EmptySpaceTag::BeginOrEnd, n_);
    countTransition(tags);

  }

  return true;
}

bool AutoSpacer::assignIDs(const std::string& train_corpus_path)
{
  std::ifstream ofs(train_corpus_path);

  if (ofs.fail())
  {
    return false;
  }

  std::string line;
  while (std::getline(ofs, line))
  {
    std::wstring wline;
    garnut::EncodingConverter::convertUtf8ToUnicode(line, wline);

    for (auto& wch : wline)
    {
      if (wch == L' ')
        continue;
      
      insertWord(wch);
    }
  }

  setNumOfState(EmptySpaceTag::NumOfTags);

  return true;
}

void AutoSpacer::findSpaceTag(const std::wstring& src, garnut::Ngram<std::wstring::value_type>& letters, garnut::Ngram<EmptySpaceTag>& tags)
{
  /*
  for (unsigned int i=0; i<n_-1; ++i)
  {
    tags.push_back(EmptySpaceTag::BeginOrEnd);
  }*/

  tags.push_back(EmptySpaceTag::AfterSpace);
  letters.push_back(src[0]);

  for (size_t i=1; i<src.length(); ++i)
  {
    // Skip empty space
    if(src[i] == L' ')
      continue;

    letters.push_back(src[i]);

    if(src[i-1] == L' ')
      tags.push_back(EmptySpaceTag::AfterSpace);
    else
      tags.push_back(EmptySpaceTag::AfterNonSpace);
  }
  /*
  for (unsigned int i=0; i<n_-1; ++i)
  {
    tags.push_back(EmptySpaceTag::BeginOrEnd);
  }*/
}


} } } // nlp::jang::amuthyst
