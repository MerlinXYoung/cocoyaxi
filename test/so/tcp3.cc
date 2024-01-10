#include <mutex>

#include "co/all.h"

DEF_string(ip, "127.0.0.1", "server ip");
DEF_int32(port, 9988, "server port");
DEF_int32(client_num, 1, "client num");
DEF_string(key, "", "private key file");
DEF_string(ca, "", "certificate file");

class Connection {
  public:
    struct Message {
        bool close;
        const void* data;
        size_t size;
        std::function<void(void*)> D;
    };
    explicit Connection(tcp::Connection c) : _impl(std::move(c)), _out_msg(2) {}
    // get the underlying socket fd
    int socket() const { return _impl.socket(); }

    /**
     * recv using co::recv or ssl::recv
     *
     * @return  >0 on success, -1 on timeout or error, 0 will be returned if the
     *          peer closed the connection.
     */
    int recv(void* buf, int n, int ms = -1) { return _impl.recv(buf, n, ms); }

    /**
     * recv n bytes using co::recvn or ssl::recvn
     *
     * @return  n on success, -1 on timeout or error, 0 will be returned if the
     *          peer closed the connection.
     */
    int recvn(void* buf, int n, int ms = -1) { return _impl.recvn(buf, n, ms); }

    /**
     * send n bytes using co::send or ssl::send
     *   - If use SSL, this method may return 0 on error.
     *
     * @return  n on success, <=0 on timeout or error.
     */
    template <class F>
    int send(const void* buf, int n, F&& f) {
        _out_msg << Message{false, buf, n, std::move(f)};
        return 0;
    }

    /**
     * close the connection
     *   - Once a Connection was closed, it can't be used any more.
     *
     * @param ms  if ms > 0, the connection will be closed ms milliseconds later.
     */
    int close() {
        _out_msg << Message{true, nullptr, 0, nullptr};
        return 0;
    }

    /**
     * reset the connection
     *   - Once a Connection was reset, it can't be used any more.
     *   - Server may use this method instead of close() to avoid TIME_WAIT state.
     *
     * @param ms  if ms > 0, the connection will be closed ms milliseconds later.
     */
    int reset(int ms = 0) { return _impl.reset(ms); }

    /**
     * get error message of the last I/O operation
     *   - If an error occured in send() or recv(), this method can be called to
     *     get the error message.
     */
    const char* strerror() const { return _impl.strerror(); }

    void start() {
        GO[this] {
            while (true) {
                Message msg;
                _out_msg >> msg;
                if (msg.close) break;
                LOG << " send(" << msg.size << "):" << msg.data;
                _impl.send(msg.data, msg.size);
                msg.D((void*)msg.data);
            };
            LOG << "send finish";
        };
    }

  private:
    tcp::Connection _impl;
    // co::mutex _mtx;
    co::chan<Message> _out_msg;
};

void conn_cb(tcp::Connection _conn) {
    Connection conn(std::move(_conn));
    conn.start();
    char buf[8] = {0};

    while (true) {
        int r = conn.recv(buf, 8);
        if (r == 0) { /* client close the connection */
            conn.close();
            break;
        } else if (r < 0) { /* error */
            conn.reset(3000);
            break;
        } else {
            LOG << "server recv " << fastring(buf, r);
            for (int i = 0; i < 100; ++i) {
                GO[i, &conn] {
                    auto str = new fastring("pong ");
                    *str << i;
                    conn.send(str->data(), str->size(), [str](void* p) {
                        LOG << "std:" << (void*)str->data() << " [" << *str << "]"
                            << " p:" << p;
                        delete str;
                    });
                };
            }
        }
    }
    LOG << "end";
}

void client_fun() {
    bool use_ssl = !FLG_key.empty() && !FLG_ca.empty();
    tcp::Client c(FLG_ip.c_str(), FLG_port, use_ssl);
    if (!c.connect(3000)) return;

    bool stop;

    char buf[20] = {0};

    go([&c, &stop] {
        char buf[20] = {0};
        int r;
        while (!stop) {
            r = c.recv(buf, 20);
            if (r < 0) {
                LOG << "client recv error: " << c.strerror();
                break;
            } else if (r == 0) {
                LOG << "server close the connection";
                break;
            } else {
                LOG << "client recv " << fastring(buf, r) << '\n';
            }
        }
    });

    for (int i = 0; i < 3; ++i) {
        LOG << "client send ping";
        int r = c.send("ping", 4);
        if (r <= 0) {
            LOG << "client send error: " << c.strerror();
            break;
        }
    }
    co::sleep(10000);
    c.disconnect();
}

co::pool* gPool = NULL;

// we don't need to close the connection manually with co::Pool.
void client_with_pool() {
    co::pool_guard<tcp::Client> c(*gPool);
    if (!c->connect(3000)) return;

    char buf[8] = {0};

    for (int i = 0; i < 3; ++i) {
        LOG << "client send ping";
        int r = c->send("ping", 4);
        if (r <= 0) {
            LOG << "client send error: " << c->strerror();
            break;
        }

        r = c->recv(buf, 8);
        if (r < 0) {
            LOG << "client recv error: " << c->strerror();
            break;
        } else if (r == 0) {
            LOG << "server close the connection";
            break;
        } else {
            LOG << "client recv " << fastring(buf, r) << '\n';
            co::sleep(500);
        }
    }
}

int main(int argc, char** argv) {
    flag::parse(argc, argv);
    gPool = new co::Pool(
        []() {
            bool use_ssl = !FLG_key.empty() && !FLG_ca.empty();
            return (void*)new tcp::Client(FLG_ip.c_str(), FLG_port, use_ssl);
        },
        [](void* p) { delete (tcp::Client*)p; });

    tcp::Server().on_connection(conn_cb).start("0.0.0.0", FLG_port, FLG_key.c_str(),
                                               FLG_ca.c_str());

    sleep::ms(32);

    if (FLG_client_num > 1) {
        for (int i = 0; i < FLG_client_num; ++i) {
            go(client_with_pool);
        }
    } else {
        go(client_fun);
    }

    sleep::sec(2);
    delete gPool;
    return 0;
}
