#ifndef MIKU_TEMPLAR_TICK_WITH_MAX_MIN
#define MIKU_TEMPLAR_TICK_WITH_MAX_MIN
namespace MikuTemplar{
template <class T>
struct Tick{
    std::size_t index_;
    std::string datetime_;
    T price_;
};
}
#endif
