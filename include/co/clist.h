#pragma once

namespace co {

struct clink {
    clink* prev{nullptr};
    clink* next{nullptr};
};

class clist {
  public:
    constexpr clist() noexcept : _head(nullptr) {}
    ~clist() = default;

    inline clink* front() const noexcept { return _head; }
    inline clink* back() const noexcept { return _head ? _head->prev : nullptr; }
    inline bool empty() const noexcept { return _head == nullptr; }
    inline void clear() noexcept { _head = nullptr; }

    inline void push_front(clink* node) noexcept {
        if (_head) {
            node->next = _head;
            node->prev = _head->prev;
            _head->prev = node;
            _head = node;
        } else {
            node->next = nullptr;
            node->prev = node;
            _head = node;
        }
    }

    inline void push_back(clink* node) noexcept {
        if (_head) {
            node->next = nullptr;
            node->prev = _head->prev;
            _head->prev->next = node;
            _head->prev = node;
        } else {
            node->next = nullptr;
            node->prev = node;
            _head = node;
        }
    }

    inline clink* pop_front() noexcept {
        clink* const x = _head;
        if (_head) {
            _head = _head->next;
            if (_head) _head->prev = x->prev;
        }
        return x;
    }

    inline clink* pop_back() noexcept {
        clink* const x = this->back();
        if (x) this->erase(x);
        return x;
    }

    inline void erase(clink* node) noexcept {
        if (node != _head) {
            node->prev->next = node->next;
            const auto x = node->next ? node->next : _head;
            x->prev = node->prev;
        } else {
            _head = _head->next;
            if (_head) _head->prev = node->prev;
        }
    }

    inline void move_front(clink* node) noexcept {
        if (node != _head) {
            node->prev->next = node->next;
            if (node->next) {
                node->next->prev = node->prev;
                node->prev = _head->prev;
                _head->prev = node;
            }
            node->next = _head;
            _head = node;
        }
    }

    inline void move_back(clink* node) noexcept {
        if (node != _head->prev) {
            if (node == _head) {
                _head = _head->next;
                node->prev->next = node;
                node->next = 0;
            } else {
                node->prev->next = node->next;
                node->next->prev = node->prev;
                node->prev = _head->prev;
                node->next = 0;
                _head->prev->next = node;
                _head->prev = node;
            }
        }
    }

    inline void swap(clist& l) noexcept {
        clink* const x = _head;
        _head = l._head;
        l._head = x;
    }

    inline void swap(clist&& l) noexcept { l.swap(*this); }

  private:
    clink* _head;
};

}  // namespace co
