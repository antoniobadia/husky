// Copyright 2016 Husky Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <map>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef WITH_HDFS
#include "hdfs/hdfs.h"
#endif

#include "io/input/mongodb_split.hpp"

namespace husky {

using namespace io;

struct Triplet {
    std::string filename;
    size_t offset;
    std::string block_location;
};

bool operator==(const Triplet& lhs, const Triplet& rhs);

#ifdef WITH_HDFS
class HDFSAssigner {
   public:
    ~HDFSAssigner();

    inline bool is_valid() const { return is_valid_; }
    void init_hdfs(const std::string& node, const std::string& port);
    void browse_hdfs(const std::string& url);
    std::string answer(const std::string& host, const std::string& url);
    int get_num_finished(std::string& url);
    void finish_url(std::string& url);

   private:
    bool is_valid_ = false;
    hdfsFS fs_ = NULL;
    std::map<std::string, int> finish_dict;
    std::map<std::string, std::unordered_set<std::pair<std::string, std::string>>> files_locality_dict;
};

class HDFSBlockAssigner {
   public:
    ~HDFSBlockAssigner();

    inline bool is_valid() const { return is_valid_; }
    void init_hdfs(const std::string& node, const std::string& port);
    void browse_hdfs(const std::string& url);
    std::pair<std::string, size_t> answer(const std::string& host, const std::string& url);
    /// Return the number of workers who have finished reading the files in
    /// the given url
    int get_num_finished(std::string& url);
    /// Use this when all workers finish reading the files in url
    void finish_url(std::string& url);

    int num_workers_alive;

   private:
    bool is_valid_ = false;
    hdfsFS fs_ = NULL;
    std::map<std::string, int> finish_dict;
    std::map<std::string, std::unordered_set<Triplet>> files_locality_dict;
};
#endif

#ifdef WITH_MONGODB
class MongoSplitAssigner {
   public:
    MongoSplitAssigner();
    virtual ~MongoSplitAssigner();
    void set_auth(const std::string&, const std::string&);
    void reset_auth();

    void create_splits();
    MongoDBSplit answer(const std::string&, const std::string&);
    void recieve_end(MongoDBSplit& split);

   private:
    bool need_auth_ = false;
    int end_count_;
    int split_num_;
    std::string error_msg_;
    std::string ns_;
    std::string password_;
    std::string server_;
    std::string username_;

    std::map<std::string, std::string> shards_map_;
    std::vector<MongoDBSplit> splits_;
    std::vector<MongoDBSplit> splits_end_;
};
#endif

}  // namespace husky

namespace std {

template <>
struct hash<husky::Triplet> {
    size_t operator()(const husky::Triplet& t) const { return hash<string>()(t.filename); }
};

}  // namespace std
