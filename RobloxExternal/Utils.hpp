#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <string>
#include "Update/Offsets.hpp"

namespace Memory{

    inline HANDLE g_procHandle = nullptr;
    inline DWORD g_targetPID = 0;



    inline BYTE* LocateModuleBase(DWORD pid, const char* modulename) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return nullptr;
        }

        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(snapshot, &moduleEntry)) {
            do {
                if (_stricmp(moduleEntry.szModule, modulename) == 0) {
                    CloseHandle(snapshot);
                    return moduleEntry.modBaseAddr;
                }
            } while (Module32Next(snapshot, &moduleEntry));
        }

        CloseHandle(snapshot);
        return nullptr;
    }

    template <typename T>
    inline T read(uintptr_t address) {
        T buffer{};
        SIZE_T bytesRead = 0;
        BOOL success = ReadProcessMemory(g_procHandle, reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), &bytesRead);
        if (!success || bytesRead != sizeof(T)) {
            DWORD error = GetLastError();
            return T{};
        }
        return buffer;
    }
    template <typename T>
    inline bool write(uintptr_t address, const T& value) {
        g_procHandle = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_VM_READ, FALSE, Globals::Pid);
        SIZE_T bytesWritten = 0;
        BOOL success = WriteProcessMemory(g_procHandle, reinterpret_cast<LPVOID>(address), &value, sizeof(T), &bytesWritten);
        if (!success || bytesWritten != sizeof(T)) {
            DWORD error = GetLastError();
            return false;
        }
        return true;
    }
    inline bool Attach(const char* pname, const char* title) {

        HWND targetWindow = FindWindowA(nullptr, title);
        GetWindowThreadProcessId(targetWindow, &Globals::Pid);
        g_targetPID = Globals::Pid;
        
        Globals::Base = Memory::LocateModuleBase(g_targetPID, pname);
        if (!Globals::Base) {
            CloseHandle(g_procHandle);
            std::cin.get();
            return false;

        }
        return true;

    }

    inline std::string readstring(std::uintptr_t address) {
        std::string result;
        result.reserve(204);
        for (int offset = 0; offset < 200; offset++) {
            char character = Memory::read<char>(address + offset);
            if (character == 0) break;
            result.push_back(character);
        }
        return result;
    }


}

struct SRoblox {

    static uintptr_t GetDataModel(BYTE* Base) {

        uintptr_t fakedmptr = Memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(Base) + Offsets::FakeDataModelPointer);
        uintptr_t realdm = Memory::read<uintptr_t>(fakedmptr + Offsets::FakeDataModelToRealDataModel);

        return realdm;

    }
};

inline SRoblox* Roblox = new SRoblox();
class Instance{
public:
    uintptr_t Address;
    Instance() : Address(0) {}

    explicit Instance(uintptr_t address) : Address(address) {}

    [[nodiscard]] std::string GetName() const {
        const auto ptr = Memory::read<uintptr_t>(Address + Offsets::Name);
        if (ptr)
            return Memory::readstring(ptr);
        return "";
    }

    [[nodiscard]] Instance GetParent() const {
        return Instance(Memory::read<uintptr_t>(Address + Offsets::Parent));
    }

    [[nodiscard]] std::vector<Instance> GetChildren() const {
        std::vector<Instance> container;

        auto start = Memory::read<uint64_t>(Address + Offsets::Children);
        auto end = Memory::read<uint64_t>(start + Offsets::ChildrenEnd);

        for (auto child = Memory::read<uint64_t>(start); child != end; child += 0x10)
            container.emplace_back(Memory::read<uint64_t>(child));

        return container;
    }

    [[nodiscard]] Instance FindFirstChild(const std::string& childname) const {
        for (const Instance& child : GetChildren()) {
            if (child.GetName() == childname) {
                return child;
            }
        }
        return Instance(0);
    }

    [[nodiscard]] Instance GetValue() const {
        return Instance(Memory::read<uintptr_t>(Address + Offsets::ObjectValue));
    }

    [[nodiscard]] bool IsValid() const {
        return Address != 0;
    }
};