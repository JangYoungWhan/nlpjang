/*
 *  File    : hiddenmarkovmodel.hpp
 *  Author  : YW. Jang
 *  Date    : 2015.08.26.Tue
 *
 *  Copyright 2015, YW. Jang, All rights reserved.
 */

#ifndef HIDDEN_MARKOV_MODEL_HPP_
#define HIDDEN_MARKOV_MODEL_HPP_

#define TRIGRAM

#include "garnut/include/ngram.hpp"

#include <map>
#include <vector>

namespace nlp { namespace jang { namespace aquamaron {

template <typename T_Word, typename T_State>
class HiddenMarkovModel
{
public:
  HiddenMarkovModel()
    :n_(3), num_of_word_(0), num_of_state_(0)
  {
    total_state_freq_ = nullptr;
    transition_lprob_ = nullptr;
    emission_lprob_ = nullptr;
  }

  HiddenMarkovModel(unsigned int n)
    :n_(n), num_of_word_(0), num_of_state_(0)
  {
    total_state_freq_ = nullptr;
    transition_lprob_ = nullptr;
    emission_lprob_ = nullptr;
  }

  virtual ~HiddenMarkovModel()
  {
    free(total_state_freq_);

 #ifdef BIGRAM
    free(transition_lprob_[0]);
    free(transition_lprob_);
 #endif

 #ifdef TRIGRAM
    free(transition_lprob_[0][0]);
    free(transition_lprob_[0]);
    free(transition_lprob_);
 #endif

    free(emission_lprob_[0]);
    free(emission_lprob_);
  }

  virtual void countTransition(const garnut::Ngram<T_State>& tags)
  {
    garnut::Ngram<unsigned int> tag_ids;
    convertTagToTagId(tags, tag_ids);
#ifdef TRIGRAM
    for (size_t i=2; i<tag_ids.size(); ++i)
    {      
      transition_lprob_[tag_ids[i-2]][tag_ids[i-1]][tag_ids[i]] += 1.0f;
    }
#endif
  }

  virtual void countEmission(const garnut::Ngram<T_Word>& words, const garnut::Ngram<T_State>& tags)
  {
    if (words.size() != tags.size())
      return;

    for (size_t i=0; i<words.size(); ++i)
    {
      unsigned int word_id = word2id_.find(words[i])->second;
      unsigned int tag_id = static_cast<unsigned int>(tags[i]);

      emission_lprob_[word_id][tag_id] += 1.0;
    }
  }

  virtual void trainHMM()
  {
    for (unsigned int i=0; i<num_of_state_; ++i)
    {
      for (unsigned int j=0; j<num_of_state_; ++j)
      {
        for (unsigned int k=0; k<num_of_state_; ++k)
        {
          if (transition_lprob_[i][j][k] < 0.9)
          {
            transition_lprob_[i][j][k] = 1.0 / total_state_freq_[k];
          }
          else
          {
            transition_lprob_[i][j][k] = log(transition_lprob_[i][j][k] / total_state_freq_[k]);
          }
        }
      }
    }

    for (unsigned int i=0; i<num_of_word_; ++i)
    {
      for (unsigned int j=0; j<num_of_state_; ++j)
      {
        if (emission_lprob_[i][j] < 0.9)
        {
          emission_lprob_[i][j] = 0.01 / total_state_freq_[j];
        }
        else
        {
          emission_lprob_[i][j] = log(emission_lprob_[i][j] / total_state_freq_[j]);
        }
      }
    }
  }

protected:
  void prepareLogProbMatrix()
  {
    // Memory allocate for the total state frequencies.
    total_state_freq_ = (float*) calloc (sizeof(float), num_of_state_);

    // Memory allocate for the matrix of transition log prob.
#ifdef BIGRAM
    transition_lprob_ = (float**) calloc (sizeof(float*), num_of_state_);
    transition_lprob_[0] = (float*) calloc (sizeof(float), num_of_state_ * num_of_state_);
    for(unsigned int i=1; i<num_of_state_; ++i)
    {
      transition_lprob_[i] = transition_lprob_[i-1] + num_of_state_;
    }
#endif

#ifdef TRIGRAM
    transition_lprob_ = (float***) calloc (sizeof(float**), num_of_state_);
    transition_lprob_[0] = (float**) calloc (sizeof(float*), num_of_state_ * num_of_state_);
    transition_lprob_[0][0] = (float*) calloc (sizeof(float), num_of_state_ * num_of_state_ * num_of_state_);
    for(unsigned int i=1; i<num_of_state_; ++i)
    {
      transition_lprob_[i] = transition_lprob_[i-1] + num_of_state_;

    }
    for(unsigned int j=1; j<num_of_state_ * num_of_state_; ++j)
    {
      transition_lprob_[0][j] = transition_lprob_[0][j-1] + num_of_state_;
    }
#endif

    // Memory allocate for the matrix of emission log prob.
    emission_lprob_ = (float**) calloc (sizeof(float*), num_of_word_);
    emission_lprob_[0] = (float*) calloc (sizeof(float), num_of_word_ * num_of_state_);
    for(unsigned int i=1; i<num_of_word_; ++i)
    {
      emission_lprob_[i] = emission_lprob_[i-1] + num_of_state_;
    }
  }

protected:
  inline
  bool insertWord(const T_Word& word)
  {
    auto find_result = word2id_.find(word);
    if (find_result == word2id_.end())
    {
      word2id_.insert(std::make_pair(word, num_of_word_++));
      id2word_.push_back(word);

      return true;
    }
    // there was a word already.
    return false;
  }

  inline
  bool insertState(const T_State& state)
  {
    auto find_result = state2id_.find(word);
    if (find_result == state2id_.end())
    {
      state2id_.insert(std::make_pair(state, num_of_state_++));
      id2state_.push_back(state);

      return true;
    }
    // there was a state already.
    return false;
  }
  
protected:
  void setNumOfWord(const unsigned int num_of_word)
  {
    num_of_word_ = num_of_word;
  }

  void setNumOfState(const unsigned int num_of_state)
  {
    num_of_state_ = num_of_state;
  }

private:
  void convertTagToTagId(const garnut::Ngram<T_State>& tags, garnut::Ngram<unsigned int>& tag_ids)
  {
    for (size_t i=0; i<tags.size(); ++i)
    {
      unsigned int tag_id = static_cast<unsigned int>(tags[i]);
      tag_ids.push_back(tag_id);
      total_state_freq_[tag_id] += 1.0f;
    }
  }

  //calculateTagSequenceProb(unsigned int k, 

protected:
  unsigned int n_;
  unsigned int num_of_word_;
  unsigned int num_of_state_;
  std::map<T_Word, unsigned int> word2id_;
  std::vector<T_Word> id2word_;
  std::map<T_State, unsigned int> state2id_;
  std::vector<T_State> id2state_;

protected:
  float* total_state_freq_;

#ifdef BIGRAM
  float** transition_lprob_;
#endif

#ifdef TRIGRAM
  float*** transition_lprob_;
#endif
  float** emission_lprob_;
};

} } } // nlp::jang::aquamaron

#endif