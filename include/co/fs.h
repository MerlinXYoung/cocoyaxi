#pragma once

#include "def.h"
#include "fastream.h"
#include "fastring.h"
#include "stl.h"

namespace fs {

__coapi bool exists(const char* path);

__coapi bool isdir(const char* path);

// modify time
__coapi int64_t mtime(const char* path);

// file size
__coapi int64_t fsize(const char* path);

// p = false  ->  mkdir
// p = true   ->  mkdir -p
__coapi bool mkdir(const char* path, bool p = false);

// async-signal-safe version
__coapi bool mkdir(char* path, bool p);

// r = false  ->  rm or rmdir
// r = true   ->  rm -r
__coapi bool remove(const char* path, bool r = false);

// rename or move a file or directory
__coapi bool mv(const char* from, const char* to);

// deprecated, use mv instead
__coapi bool rename(const char* from, const char* to);

// administrator privileges required on windows
__coapi bool symlink(const char* dst, const char* lnk);

inline bool exists(const fastring& path) { return fs::exists(path.c_str()); }

inline bool exists(const std::string& path) { return fs::exists(path.c_str()); }

inline bool isdir(const fastring& path) { return fs::isdir(path.c_str()); }

inline bool isdir(const std::string& path) { return fs::isdir(path.c_str()); }

inline int64_t mtime(const fastring& path) { return fs::mtime(path.c_str()); }

inline int64_t mtime(const std::string& path) { return fs::mtime(path.c_str()); }

inline int64_t fsize(const fastring& path) { return fs::fsize(path.c_str()); }

inline int64_t fsize(const std::string& path) { return fs::fsize(path.c_str()); }

inline bool mkdir(const fastring& path, bool p = false) { return fs::mkdir(path.c_str(), p); }

inline bool mkdir(const std::string& path, bool p = false) { return fs::mkdir(path.c_str(), p); }

inline bool remove(const fastring& path, bool r = false) { return fs::remove(path.c_str(), r); }

inline bool remove(const std::string& path, bool r = false) { return fs::remove(path.c_str(), r); }

inline bool mv(const fastring& from, const fastring& to) {
    return fs::mv(from.c_str(), to.c_str());
}

inline bool mv(const std::string& from, const std::string& to) {
    return fs::mv(from.c_str(), to.c_str());
}

inline bool rename(const fastring& from, const fastring& to) {
    return fs::rename(from.c_str(), to.c_str());
}

inline bool rename(const std::string& from, const std::string& to) {
    return fs::rename(from.c_str(), to.c_str());
}

inline bool symlink(const fastring& dst, const fastring& lnk) {
    return fs::symlink(dst.c_str(), lnk.c_str());
}

inline bool symlink(const std::string& dst, const std::string& lnk) {
    return fs::symlink(dst.c_str(), lnk.c_str());
}

// open mode:
//   'r': read         open if exists
//   'a': append       created if not exists
//   'w': write        created if not exists, truncated if exists
//   'm': modify       like 'w', but not truncated if exists
//   '+': read/write   created if not exists
class __coapi file {
  public:
    static const int seek_beg = 0;
    static const int seek_cur = 1;
    static const int seek_end = 2;

    inline file() : _p(0) {}
    ~file();

    // @n: reserve n bytes of memory for the path
    explicit file(size_t n);

    inline file(const char* path, char mode) : _p(0) { this->open(path, mode); }

    inline file(const fastring& path, char mode) : file(path.c_str(), mode) {}
    inline file(const std::string& path, char mode) : file(path.c_str(), mode) {}

    inline file(file&& f) : _p(f._p) { f._p = 0; }

    file(const file& x) = delete;
    void operator=(const file& x) = delete;
    void operator=(file&& x) = delete;

    explicit operator bool() const;

    inline bool operator!() const { return !(bool)(*this); }

    const char* path() const;

    inline int64_t size() const { return fs::fsize(this->path()); }
    inline bool exists() const { return fs::exists(this->path()); }

    bool open(const char* path, char mode);

    inline bool open(const fastring& path, char mode) { return this->open(path.c_str(), mode); }

    inline bool open(const std::string& path, char mode) { return this->open(path.c_str(), mode); }

    void close();

    void seek(int64_t off, int whence = seek_beg);

    size_t read(void* buf, size_t n);

    fastring read(size_t n);

    size_t write(const void* s, size_t n);

    inline size_t write(const char* s) { return this->write(s, strlen(s)); }

    inline size_t write(const fastring& s) { return this->write(s.data(), s.size()); }

    inline size_t write(const std::string& s) { return this->write(s.data(), s.size()); }

    inline size_t write(char c) { return this->write(&c, 1); }

  private:
    void* _p;
};

// open mode:
//   'a': append       created if not exists
//   'w': write        created if not exists, truncated if exists
class __coapi fstream {
  public:
    inline fstream() : _s(8192) {}
    explicit inline fstream(size_t cap) : _s(cap) {}

    inline fstream(const char* path, char mode, size_t cap = 8192)
        : _s(cap), _f(path, mode == 'w' ? 'w' : 'a') {}

    inline fstream(const fastring& path, char mode, size_t cap = 8192)
        : fstream(path.c_str(), mode, cap) {}

    inline fstream(const std::string& path, char mode, size_t cap = 8192)
        : fstream(path.c_str(), mode, cap) {}

    inline fstream(fstream&& fs) : _s(std::move(fs._s)), _f(std::move(fs._f)) {}

    inline ~fstream() { this->close(); }

    explicit inline operator bool() const { return (bool)_f; }

    inline bool operator!() const { return !(bool)_f; }

    inline bool open(const char* path, char mode) {
        this->close();
        return _f.open(path, mode == 'w' ? 'w' : 'a');
    }

    inline bool open(const fastring& path, char mode) { return this->open(path.c_str(), mode); }

    inline bool open(const std::string& path, char mode) { return this->open(path.c_str(), mode); }

    inline void reserve(size_t n) { _s.reserve(n); }

    inline void flush() {
        if (!_s.empty()) {
            _f.write(_s.data(), _s.size());
            _s.clear();
        }
    }

    inline void close() {
        this->flush();
        _f.close();
    }

    // n <= cap - szie         ->   append
    // cap - size < n <= cap   ->   flush and append
    // n > cap                 ->   flush and write
    inline fstream& append(const void* s, size_t n) {
        if (_s.capacity() < _s.size() + n) this->flush();
        n <= _s.capacity() ? ((void)_s.append(s, n)) : ((void)_f.write(s, n));
        return *this;
    }

    inline fstream& operator<<(const char* s) { return this->append(s, strlen(s)); }

    inline fstream& operator<<(const fastring& s) { return this->append(s.data(), s.size()); }

    inline fstream& operator<<(const std::string& s) { return this->append(s.data(), s.size()); }

    inline fstream& operator<<(const fastream& s) { return this->append(s.data(), s.size()); }

    template <typename T>
    inline fstream& operator<<(T v) {
        if (_s.capacity() < _s.size() + 24) this->flush();
        _s << v;
        return *this;
    }

  private:
    fastream _s;
    fs::file _f;

    DISALLOW_COPY_AND_ASSIGN(fstream);
};

class __coapi dir {
  public:
    inline dir() : _p(0) {}
    ~dir();

    explicit inline dir(const char* path) : _p(0) { this->open(path); }

    explicit inline dir(const fastring& path) : dir(path.c_str()) {}
    explicit inline dir(const std::string& path) : dir(path.c_str()) {}

    inline dir(dir&& d) : _p(d._p) { d._p = 0; }

    dir(const dir&) = delete;
    void operator=(const dir&) = delete;
    void operator=(dir&&) = delete;

    bool open(const char* path);
    inline bool open(const fastring& path) { return this->open(path.c_str()); }
    inline bool open(const std::string& path) { return this->open(path.c_str()); }

    void close();

    const char* path() const;

    // return all entries
    co::vector<fastring> all() const;

    class iterator {
      public:
        explicit iterator(void* p) : _p(p) {}
        ~iterator() = default;

        fastring operator*() const;
        iterator& operator++();

        inline bool operator==(const iterator& it) const { return _p == it._p; }

        inline bool operator!=(const iterator& it) const { return !this->operator==(it); }

      private:
        void* _p;
    };

    iterator begin() const;
    inline iterator end() const { return iterator(NULL); }

  private:
    void* _p;
};

}  // namespace fs
