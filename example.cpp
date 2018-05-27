#include <iostream>
#include <cassert>
#include "pipeline.h"

struct Context {
    int data = 0;
};

struct one
{
    void operator()(Context & ctx, pipeline_complete done) {
        ctx.data += 1;
        done();
    }
};

template <typename UserData>
struct two
{
    void operator()(UserData & ctx, pipeline_complete done) {
        ctx.data += 2;
        done();
    }
};

struct three
{
    void operator()(Context & ctx, pipeline_complete done) {
        ctx.data += 3;
        done();
    }
};

class check_work {
    pipeline<Context, one, two<Context>, one, three, two<Context>, one, one, three> work1;
    pipeline<Context, one, two<Context>, three> work2;
public:
    check_work(): work1(std::bind(&check_work::done1, this)), work2(std::bind(&check_work::done2, this)) {};

    void run() {
        work1.run();
    }

    void done1() {
        assert(work1.ctx.data == 14);
        std::cout << "Done 1\n";
        work2.run();
    }

    void done2() {
        assert(work2.ctx.data == 6);
        std::cout << "Done 2\n";
    }
};

int
main() {
    check_work wrk;
    wrk.run();
    return 0;
}
