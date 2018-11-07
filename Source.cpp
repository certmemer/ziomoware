#include "Header.h"
#include <cstddef>

namespace offsets {
    constexpr std::ptrdiff_t team           = 0xf0;
    constexpr std::ptrdiff_t flags          = 0x100;
    constexpr std::ptrdiff_t incross        = 0xb2dc;
    constexpr std::ptrdiff_t force_jump     = 0x50e33ac;
    constexpr std::ptrdiff_t entity_list    = 0x4c40664;
    constexpr std::ptrdiff_t force_attack   = 0x3081d40;
    constexpr std::ptrdiff_t local_player   = 0xc638ac;
}

int main()
{
    LPDWORD rzyg{}; WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE),
        R"(          (             )           (  (        )   (       (
     (    )\    (      (       (    )\))(    ( /(   )(     ))\
     )\  ((_)   )\     )\  '   )\  ((_)()\   )(_)) (()\   /((_)
    ((_)  (_)  ((_)  _((_))   ((_) _(()((_) ((_)_   ((_) (_))
    |_ /  | | / _ \ | '  \() / _ \ \ V  V / / _` | | '_| / -_)
    /__|  |_| \___/ |_|_|_|  \___/  \_/\_/  \__,_| |_|   \___|
              s/o nave, wando, pep, hvhrodeo d-_-b)", /*uffya*/0x1ac, rzyg, nullptr);

    c_mem memory;
    while (!memory.open_process("csgo.exe"))        Sleep(1000);
    while (!memory.get_module("serverbrowser.dll")) Sleep(1000);

    auto client = memory.get_module("client_panorama.dll");
    auto engine = memory.get_module("engine.dll");

    auto local = memory.read<uintptr_t>(client + offsets::local_player);
    auto get_ent = [&](int i) { return memory.read<int>(client + offsets::entity_list + ((i - 1) * 0x10)); };

    for (;;) {

        if (GetAsyncKeyState(VK_SPACE) & -1) {
            auto flags = memory.read<int>(local + offsets::flags);
            memory.write(client + offsets::force_jump, (flags & 1) ? 6 : 4);
        }

        if (GetAsyncKeyState(VK_MENU) & -1) {
            auto target = memory.read<int>(local + offsets::incross);
            if (target != 0) {
                if (memory.read<int>(get_ent(target) + offsets::team) != memory.read<int>(local + offsets::team))
                    memory.write(client + offsets::force_attack, 6);
            }
        }

        for (auto i{ 1 }; i <= 64; ++i) {
            memory.write(get_ent(i) + 0x38f4, 1000000.f);
        }

        Sleep(7);
    }
}
