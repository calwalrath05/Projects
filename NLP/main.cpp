#include <iostream>
#include <set>
#include "csvstream.hpp"
#include <map>
#include <cmath>
#include <limits>
using namespace std;
using std::pair;

class Classifier {
  public: 

    void calculate_log_likelihood(const string label) {
      for (auto it2 = posts_with_word_given_label[label].begin(); 
      it2 != posts_with_word_given_label[label].end(); ++it2) {
        // if word is in the post and in the training set
        double word_P = log(static_cast<double>
        (posts_with_word_given_label[label][it2->first])
        /label_freq_map[label]);
        log_likelihood[label][it2->first] = word_P;
      }
    }

    double calculate_log_P(const string label, const set<string> set) {
      double log_P = log(static_cast<double>(label_freq_map[label])/num_posts);
      for (auto it2 = set.begin(); it2 != set.end(); ++it2) {
        log_P += calculate_sum_word_P(label, *it2);
      }
      return log_P;
    }

    double calculate_sum_word_P(const string label, const string word) {
      // if word isn't in any post with label, but is in overall training set
      if ((posts_with_word_given_label[label].find(word)) == 
      posts_with_word_given_label[label].end() && 
        (unique_words.find(word) != unique_words.end())) {
        return log(static_cast<double>(word_freq_map[word])/num_posts);
      }
      // if word is nowhere in training set
      else if (unique_words.find(word) == unique_words.end()) {
        return log(1/static_cast<double>(num_posts));
      }
      else {
        return log_likelihood[label][word];
      }
      return 0.0;
    }

    void training(string filename, bool is_debug) {
      num_posts = 0;
      csvstream csvin(filename);
      map<string, string> row; // <colum name, datum (words)>
      
      while(csvin >> row) { // for each iteration, one row of csv file is 
                            //stored in row var. row stores a pair of 
                            //<label, content>
        string tag = row["tag"];
        string content = row["content"];
        set<string> set;
        num_posts++;
        label_freq_map[tag]++; // increments frequency of posts w/the label 

        if (is_debug) {
          cout << "  label = " << tag << ", content = " << content << endl;
        }
        stringstream word_stream(content);
        string next_word;
        while (word_stream >> next_word) {
          set.insert(next_word);
        }

        // iterates through each word in the "content" row in file
        for (auto it = set.begin(); it != set.end(); ++it) {
        //for (auto col:row) {
          //set.insert(*it);
          unique_words.insert(*it);
          word_freq_map[*it]++; // increments frequency of posts w/the label
          posts_with_word_given_label[tag][*it]++;
        }
      }
      vocab_size = unique_words.size();
      for (auto it = label_freq_map.begin(); it != 
      label_freq_map.end(); ++it) {
        string label = it->first;
        calculate_log_likelihood(label);
      }
    }

    int predict(string filename) {
      csvstream csvin(filename);
      map<string, string> row; // <colum name, datum (words)>
      int num_correct = 0;
      num_test_posts = 0;
      
      while(csvin >> row) {
        ++num_test_posts;
        string content = row["content"];
        string label = row["tag"];

        set<string> set; // contains all unique words in the content of one row

        istringstream word_stream(content);
        string next_word;
        while (word_stream >> next_word) {
          set.insert(next_word);
        }

        double highest = std::numeric_limits<double>::lowest();
        string prediction = "";
        double log_P;
        
        for (auto it = label_freq_map.begin(); it != 
        label_freq_map.end(); ++it) {
          log_P = calculate_log_P(it->first, set);
          if (log_P > highest) {
            highest = log_P;
            prediction = it->first;
          }
        }
        cout << "  correct = " << label << ", predicted = " << 
        prediction << ", log-probability score = " << highest << endl;
        cout << "  content = " << content << endl << endl;
        if (prediction == label) {
          ++num_correct;
        }
      }
      return num_correct;
    }

    void print_debug() {
      cout << "vocabulary size = " << vocab_size << endl << endl;
      cout << "classes:" << endl;
      for (auto it = label_freq_map.begin(); 
      it != label_freq_map.end(); ++it) {
        string label = it->first;
        double log_prior = log(static_cast<double>
        (label_freq_map[label])/num_posts); 
        cout << "  " << label << ", " << label_freq_map[label] 
        << " examples, log-prior = " << log_prior << endl;
      }
      
      cout << "classifier parameters:" << endl;
      for (auto it = label_freq_map.begin(); it != label_freq_map.end(); 
      ++it) {
        for (auto it2 = posts_with_word_given_label[it->first].begin(); it2 
        != posts_with_word_given_label[it->first].end(); ++it2) {
          string label = it->first;
          cout << "  " << label << ":" << it2->first << ", count = " << 
            posts_with_word_given_label[label][it2->first] 
            << ", log-likelihood = " << 
            log_likelihood[label][it2->first] << endl;
        }
      }
      cout << endl;
    }

    void print(string train_filename, string test_filename, bool is_debug) {
      if (is_debug) {
        cout << "training data:" << endl;
      }
      training(train_filename, is_debug);
      cout << "trained on " << num_posts << " examples" << endl;
      if (is_debug == true) {
        print_debug();
      }
      cout << "test data:" << endl;
      int num_correct = predict(test_filename);
      cout << "performance: " << num_correct << " / " << num_test_posts 
      << " posts predicted correctly" << endl;
    }
    
  private:
    int num_posts; // # total posts in training set
    int vocab_size; // # of unique words in training set
    int num_test_posts;
    map<string, int> word_freq_map; // map of <word, freq>. tracks # of posts
                            // that contain word
    map<string, int> label_freq_map; // map of <label, freq>. tracks # of posts
                             // with given label
    // map of maps. tracks # posts containing word, given the label
    map<string, map<string, int>> posts_with_word_given_label;
    // for keeping track of vocab size in entire training set
    set<string> unique_words; 
    // how likely a post is to be under a label if it contains a certain word
    map<string, map<string, double>> log_likelihood; 
};

int main(int argc, char *argv[]) {
  cout.precision(3);

  string train_file = argv[1];
  ifstream fin;
  fin.open(train_file);

  string test_file = argv[2];
  ifstream fin2;
  fin2.open(test_file);

  if (!fin.is_open()) {
    cout << "Error opening file: " << train_file << endl;
    return 1;
  }
  if (!fin2.is_open()) {
    cout << "Error opening file: " << test_file << endl;
    return 1;
  }
  if (argc != 3 && argc != 4) {
    cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
    return 1;
  }
  if (argc == 4 && static_cast<string>(argv[3]) != "--debug") {
    cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
  }
  else {
    Classifier c;
    if (argc == 4) {
      c.print(train_file, test_file, true);
    }
    else {
      c.print(train_file, test_file, false);
    }
  }
}