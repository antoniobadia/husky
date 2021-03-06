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

#include <cstdlib>
#include <vector>

#include "base/serialization.hpp"
#include "core/hash_ring.hpp"
#include "core/mailbox.hpp"
#include "core/worker_info.hpp"

namespace husky {

using base::BinStream;

class ChannelBase {
   public:
    virtual ~ChannelBase();

    inline static size_t get_num_channel() { return counter; }

    inline size_t get_channel_id() const { return channel_id_; }
    inline size_t get_channel_progress() const { return progress_; }
    inline LocalMailbox* get_mailbox() { return mailbox_; }

    void set_local_id(size_t local_id);
    void set_global_id(size_t global_id);
    void set_worker_info(WorkerInfo* worker_info);
    void set_mailbox(LocalMailbox* mailbox);
    void set_hash_ring(HashRing* hash_ring);

    void setup(size_t, size_t, WorkerInfo*, LocalMailbox*, HashRing*);

    /// customized_setup() is used to do customized setup for subclass
    virtual void customized_setup() = 0;

    /// prepare() needs to be invoked to do some preparation work (if any), such as clearing buffers,
    /// before it can take new incoming communication using the in(BinStream&) method.
    /// In list_execute (in core/executor.hpp), prepare() is usually used before any in(BinStream&).
    virtual void prepare() {}

    /// in(BinStream&) defines what the channel should do when receiving a binstream
    virtual void in(BinStream& bin) {}

    /// out() defines what the channel should do after a list_execute, normally mailbox->send_complete() will be invoked
    virtual void out() {}

    /// is_flushed() checks whether flush() is invoked
    /// If yes, then list_execute will invoke prepare() and later in(BinStream& bin) will be invoked
    /// If no, list_execute will just omit this channel
    inline bool is_flushed() { return flushed_[progress_]; }

   protected:
    ChannelBase();
    void inc_progress();

    size_t channel_id_;
    size_t global_id_;
    size_t local_id_;
    size_t progress_;

    std::vector<bool> flushed_;

    WorkerInfo* worker_info_ = nullptr;
    LocalMailbox* mailbox_ = nullptr;
    HashRing* hash_ring_ = nullptr;

    static thread_local size_t counter;
};

}  // namespace husky
