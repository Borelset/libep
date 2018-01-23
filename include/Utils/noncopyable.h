//
// Created by borelset on 1/15/18.
//

#ifndef LIBEP_NONCOPYABLE_H
#define LIBEP_NONCOPYABLE_H

namespace Utils{
    class noncopyable{
    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    private:
        noncopyable(const noncopyable&) = delete;
        const noncopyable&operator=(const noncopyable&) = delete;
    };
}

#endif //LIBEP_NONCOPYABLE_H
