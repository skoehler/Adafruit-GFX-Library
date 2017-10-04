#ifndef _PRINT_H_
#define _PRINT_H_

#include <string>
#include <cstdint>


class Print {
  protected:
    virtual void write(char) = 0;
    virtual void write(const char *, size_t) = 0;
  public:
    Print() {}
    virtual ~Print() {}

    void print(char);
    void print(const char *);
    void print(const char *, size_t);
    void print(const std::string &);
    void print(int32_t, int = 10);
    void print(int64_t, int = 10);
    void print(uint32_t, int = 10);
    void print(uint64_t, int = 10);
    void print(float, int = 2);
    void print(double, int = 2);

    void println(void);
    void println(char);
    void println(const char *);
    void println(const char *, size_t);
    void println(const std::string &v);
    void println(int32_t, int = 10);
    void println(int64_t, int = 10);
    void println(uint32_t, int = 10);
    void println(uint64_t, int = 10);
    void println(float, int = 2);
    void println(double, int = 2);
};

#endif // _PRINT_H_
