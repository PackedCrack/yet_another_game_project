//
// Created by qwerty on 28/02/2025.
//

#pragma once
#include <shared_mutex>
#include <unordered_set>

template<typename element_t>
class ThreadSafeHashset
{
public:
private:
    std::unordered_set<element_t> m_Set;
};