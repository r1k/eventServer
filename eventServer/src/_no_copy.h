#pragma once

class _no_copy
{
public:

    _no_copy() {}
    virtual ~_no_copy() {};

private:

    _no_copy(const _no_copy&) = delete;
    _no_copy& operator=(const _no_copy&) = delete;
};

