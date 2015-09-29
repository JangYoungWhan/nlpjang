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
#include <limits>

namespace nlp { namespace jang { namespace amuthyst {

AutoSpacer::AutoSpacer()
  :n_(3), HiddenMarkovModel(3)
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

  assginStartEndChar();

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
    //tags.attachTags(EmptySpaceTag::BeginOrEnd, EmptySpaceTag::BeginOrEnd, n_);
    countTransition(tags);    
  }
  trainHMM();

  return true;
}

bool AutoSpacer::test(const std::string& test_corpus_path)
{
  const std::wstring sent(L"나는학교에갔다.");
  
  garnut::Ngram<std::wstring::value_type> letters;
  garnut::Ngram<EmptySpaceTag> tags;

  garnut::SentenceDealer::convertSentenceToLetterNgram(sent, letters);
  letters.attachTags(0x01, 0x02, n_);

  viterbiSearch(letters, tags);

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

void AutoSpacer::assginStartEndChar()
{
  insertWord(0x01);
  insertWord(0x02);
}

void AutoSpacer::findSpaceTag(const std::wstring& src, garnut::Ngram<std::wstring::value_type>& letters, garnut::Ngram<EmptySpaceTag>& tags)
{
  // Insert start character and symbol.
  for (unsigned int i=0; i<n_-1; ++i)
  {
    tags.push_back(EmptySpaceTag::SentenceBegin);
    letters.push_back(0x01);
  }

  tags.push_back(EmptySpaceTag::AfterSpace);
  letters.push_back(src[0]);

  for (size_t i=1; i<src.length(); ++i)
  {
    // Skip empty space.
    if(src[i] == L' ')
      continue;

    letters.push_back(src[i]);

    if(src[i-1] == L' ')
      tags.push_back(EmptySpaceTag::AfterSpace);
    else
      tags.push_back(EmptySpaceTag::AfterNonSpace);
  }
  
  // Insert end character and symbol.
  for (unsigned int i=0; i<n_-1; ++i)
  {
    tags.push_back(EmptySpaceTag::SentenceEnd);
    letters.push_back(0x02);
  }
}

void AutoSpacer::viterbiSearch(const garnut::Ngram<std::wstring::value_type>& words,
                               garnut::Ngram<EmptySpaceTag>& hidden_state)
{
  // It stores pi values using by dynamic programming approach.
  float*** pi_table;
  pi_table = (float***) calloc (sizeof(float**), words.size());
  pi_table[0] = (float**) calloc (sizeof(float*), words.size() * num_of_state_);
  pi_table[0][0] = (float*) calloc (sizeof(float), words.size() * num_of_state_ * num_of_state_);
  for (unsigned int i=1; i<words.size(); ++i)
  {
    pi_table[i] = pi_table[i-1] + num_of_state_;

  }
  for (unsigned int j=1; j<words.size() * num_of_state_; ++j)
  {
    pi_table[0][j] = pi_table[0][j-1] + num_of_state_;
  }

  for (unsigned int i=0; i<words.size(); ++i)
  {
    for (unsigned int j=0; j<num_of_state_; ++j)
    {
      for (unsigned int k=0; k<num_of_state_; ++k)
      {
        pi_table[i][j][k] = -std::numeric_limits<float>::max();
      }
    }
  }

  // test
  /*
  for (unsigned int i=0; i<num_of_state_; ++i)
  {
    for (unsigned int j=0; j<num_of_state_; ++j)
    {
      for (unsigned int k=0; k<num_of_state_; ++k)
      {
        if (transition_lprob_[i][j][k] > -std::numeric_limits<float>::max())
          printf("q[%d][%d][%d] = %e\n", i, j, k, transition_lprob_[i][j][k]);
      }
    }
  }*/

  size_t k = n_-1;
  int w, u, v;
  w = SentenceBegin;
  u = SentenceBegin;

  pi_table[k][w][u] = 0.0f;
  for (k=k+1; k<words.size()-(n_-1); ++k)
  {
    for (v=0; v<EmptySpaceTag::NumOfTags; ++v)
    {
      float max_pi = -std::numeric_limits<float>::max();
      int max_w = 0;
      for (w=0; w<EmptySpaceTag::NumOfTags; ++w)
      {
        unsigned int x_k;
        auto find_result = word2id_.find(words[k]);
        if (find_result != word2id_.end())
        {
          x_k = find_result->second;
        }
        else
        {
          std::cout << "OOV occured!!" << std::endl;
          x_k = 0; // OOV
        }

        
        float pi = pi_table[k-1][w][u] + transition_lprob_[w][u][v] + emission_lprob_[x_k][u];
        if (pi > max_pi)
        {
          max_pi = pi;
          max_w = w;
        }

        printf("pi[%d][%d][%d] + q[%d][%d][%d] + e[%d][%d]\n", k-1,w,u,w,u,v,x_k,u);
        //printf("= %e + %e + %e\n", pi_table[k-1][w][u], transition_lprob_[w][u][v], emission_lprob_[x_k][u]);
        printf("pi=%e / max_pi=%e\n", pi, max_pi);
      }
      pi_table[k][u][v] = max_pi;
    }


  }


  free(pi_table[0][0]);
  free(pi_table[0]);
  free(pi_table);
}


} } } // nlp::jang::amuthyst
