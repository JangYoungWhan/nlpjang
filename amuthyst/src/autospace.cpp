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
#include <list>
#include <set>

#ifdef REFINE_SENTENCE
#include <cctype> // using for ispunct
#endif

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
  printf("Training... : %s\n\n", train_corpus_path.c_str());
  if (!assignIDs(train_corpus_path))
    return false;

  assginStartEndChar();

  prepareLogProbMatrix();

  std::ifstream ifs(train_corpus_path);

  if (ifs.fail())
  {
    return false;
  }

  std::string line;
  while (std::getline(ifs, line))
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

  ifs.close();

  return true;
}

void makeTestInput(const std::wstring& src, std::wstring& dst)
{
  for (auto& wch : src)
  {
    if (wch == L' ')
      continue;

    dst.push_back(wch);
  }
}

void makeTestOutput(const std::wstring& src, garnut::Ngram<EmptySpaceTag>& state,
                    std::wstring& dst)
{
  dst.push_back(src[0]);
  for (size_t i=1; i<state.size(); ++i)
  {
    switch (state[i])
    {
    case EmptySpaceTag::WordBegin:
      dst.push_back(L' ');
      dst.push_back(src[i]);
      break;
    case EmptySpaceTag::WordIng:
      dst.push_back(src[i]);
      break;
    case EmptySpaceTag::WordEnd:
      dst.push_back(src[i]);
      dst.push_back(L' ');
      break;
    default:
      break;
    }
  }
}

bool AutoSpacer::test(const std::string& test_corpus_path)
{
  printf("Testing... : %s\n\n", test_corpus_path.c_str());
  std::ifstream ifs(test_corpus_path);

  if (ifs.fail())
  {
    return false;
  }
  std::ofstream ofs("C:/dummy/as.txt");
  int total_syllable = 0, total_sentence = 0, correct_syllable = 0, correct_sentence = 0;
  std::string line;
  while (std::getline(ifs, line))
  {
    std::wstring wline, test_input;
    garnut::EncodingConverter::convertUtf8ToUnicode(line, wline);

#ifdef REFINE_SENTENCE
    std::wstring origin_test_input;
    makeTestInput(wline, origin_test_input);

    // Replace insignificant letters.
    refineSentence(wline);
#endif

    // Remove spaces,
    makeTestInput(wline, test_input);

    garnut::Ngram<std::wstring::value_type> letters;
    garnut::Ngram<EmptySpaceTag> tags;

    garnut::SentenceDealer::convertSentenceToLetterNgram(test_input, letters);
    letters.attachTags(0x01, 0x02, n_);
    viterbiSearch(letters, tags);

    std::wstring test_output;
#ifdef REFINE_SENTENCE
    makeTestOutput(origin_test_input, tags, test_output);
#else
    makeTestOutput(test_input, tags, test_output);
#endif

    std::vector<std::wstring> tokens, result;
    garnut::splitStringToNgram<std::wstring>(wline, tokens, L" ");
    garnut::splitStringToNgram<std::wstring>(test_output, result, L" ");
    std::set<std::wstring> token_set(tokens.begin(), tokens.end());

    for (auto& r : result)
    {
      if (token_set.find(r) != token_set.end())
        correct_syllable++;
    }
    total_syllable += tokens.size();

    if (test_input == test_output)
      correct_sentence++;
    total_sentence++;
    line.clear();
    garnut::EncodingConverter::convertUnicodeToUtf8(test_output, line);
    ofs << line << std::endl;
  }  

  ofs.close();
  ifs.close();  

  printf("syllable : correct:%d / total:%d\naccuracy:%f%\n", correct_syllable, total_syllable, ((float)correct_syllable/total_syllable)*100);
  printf("sentence : correct:%d / total:%d\naccuracy:%f%\n", correct_sentence, total_sentence, ((float)correct_sentence/total_sentence)*100);

  return true;
}

bool AutoSpacer::assignIDs(const std::string& train_corpus_path)
{
  std::ifstream ifs(train_corpus_path);

  if (ifs.fail())
  {
    return false;
  }

  std::string line;
  while (std::getline(ifs, line))
  {
    std::wstring wline;
    garnut::EncodingConverter::convertUtf8ToUnicode(line, wline);
#ifdef REFINE_SENTENCE
    refineSentence(wline);
#endif

    for (auto& wch : wline)
    {
      if (wch == L' ')
        continue;
      
      insertWord(wch);
    }
  }

  setNumOfWord(word2id_.size());
  setNumOfState(EmptySpaceTag::NumOfTags);

  return true;
}

void AutoSpacer::refineSentence(std::wstring& target) const
{
  for (auto it=target.begin(); it!=target.end(); ++it)
  {
    if (L'0' <= *it && *it <= L'9')
      *it = L'0';
    else if (ispunct(static_cast<int>(*it)))
      *it = L'*';
  }
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

  tags.push_back(EmptySpaceTag::WordBegin);
  letters.push_back(src[0]);

  for (size_t i=1; i<src.length()-1; ++i)
  {
    // Skip empty space.
    if(src[i] == L' ')
      continue;

    letters.push_back(src[i]);

    if(src[i-1] == L' ')
      tags.push_back(EmptySpaceTag::WordBegin);
    else if (src[i+1] == L' ')
      tags.push_back(EmptySpaceTag::WordEnd);
    else
      tags.push_back(EmptySpaceTag::WordIng);
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

  // Back pointer argmax pi for the backtracking.
  EmptySpaceTag*** bp_pi;
  bp_pi = (EmptySpaceTag***) malloc (sizeof(EmptySpaceTag**) * words.size());
  bp_pi[0] = (EmptySpaceTag**) malloc (sizeof(EmptySpaceTag*) * words.size() * num_of_state_);
  bp_pi[0][0] = (EmptySpaceTag*) malloc (sizeof(EmptySpaceTag) * words.size() * num_of_state_ * num_of_state_);

  for (unsigned int i=1; i<words.size(); ++i)
  {
    pi_table[i] = pi_table[i-1] + num_of_state_;
    bp_pi[i] = bp_pi[i-1] + num_of_state_;

  }
  for (unsigned int j=1; j<words.size() * num_of_state_; ++j)
  {
    pi_table[0][j] = pi_table[0][j-1] + num_of_state_;
    bp_pi[0][j] = bp_pi[0][j-1] + num_of_state_;
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

  size_t k = 1;
  int w, u, v;
  w = static_cast<int>(EmptySpaceTag::SentenceBegin);
  u = static_cast<int>(EmptySpaceTag::SentenceBegin);

  pi_table[k][w][u] = 0.0f;
  int last_u = 0, last_v = 0;
  for (k=k+1; k<words.size(); ++k)
  {
    for (v=0; v<EmptySpaceTag::NumOfTags; ++v)
    {
      for (u=0; u<EmptySpaceTag::NumOfTags; ++u)
      {
        float max_pi = -std::numeric_limits<float>::max();
        int max_w = static_cast<int>(EmptySpaceTag::SentenceBegin);
        for (w=0; w<EmptySpaceTag::NumOfTags; ++w)
        {
          unsigned int x_k;
          float e;
          auto find_result = word2id_.find(words[k]);
          if (find_result != word2id_.end())
          {
            x_k = find_result->second;
            e = emission_lprob(x_k, v);
          }
          else // OOV
          {
            e = logf(NOT_OCCURED_FREQ_ / sum_of_state_freq_);
          }

          float pi = pi_table[k-1][w][u] + transition_lprob(v, w, u) + e;
          if (pi > max_pi)
          {
            max_pi = pi;
            max_w = w;
            last_u = u;
            last_v = v;
          }

          //printf("pi[%d][%d][%d] + q[%d][%d][%d] + e[%d][%d]\n", k-1,w,u,w,u,v,x_k,u);
          //printf("= %e + %e + %e\n", pi_table[k-1][w][u], transition_lprob(v, w, u), e);
          //printf("pi=%e / max_pi=%e\n", pi, max_pi);
        }
        pi_table[k][u][v] = max_pi;
        bp_pi[k][last_u][last_v] = static_cast<EmptySpaceTag>(max_w);
      }
    }
  }

  std::list<int> bp_list;
  for (k=k-1; k>n_; --k)
  {
    int prev_w = bp_pi[k][last_u][last_v];

    bp_list.push_front(prev_w);

    last_v = last_u;
    last_u = prev_w;
  }

  for (auto& bp : bp_list)
  {
    hidden_state.push_back(static_cast<EmptySpaceTag>(bp));
  }

#ifdef _DEBUG
  for (auto& bp : bp_list)
    std::cout << bp << " ";
  std::cout << std::endl;
#endif

  free(bp_pi[0][0]);
  free(bp_pi[0]);
  free(bp_pi);

  free(pi_table[0][0]);
  free(pi_table[0]);
  free(pi_table);
}

inline
float AutoSpacer::transition_lprob(int v, int w, int u) const
{
  return transition_lprob_[w][u][v];
}

inline
float AutoSpacer::emission_lprob(int x_k, int v) const
{
  return emission_lprob_[v][x_k];
}


} } } // nlp::jang::amuthyst
