#include <iostream>
#include "Utils.hpp"
#include "Update/Offsets.hpp"
#include <thread>






void mainthread() {


    if (!Memory::Attach("RobloxPlayerBeta.exe", "Roblox"))
        return;

    while (true) {
        Instance Datamodel(Roblox->GetDataModel(Globals::Base));
        Globals::datamodel = Datamodel.Address;

        Instance Workspace(Datamodel.FindFirstChild("Workspace"));
        Globals::WorkspacePtr = Workspace.Address;

        Instance Players(Datamodel.FindFirstChild("Players"));
        Instance LocalPlayer(Memory::read<uintptr_t>(Players.Address + Offsets::LocalPlayer));
        
        Instance Character(Workspace.FindFirstChild(LocalPlayer.GetName()));
        Globals::PlayerPtr = Character.Address;

        Instance Humanoid(Character.FindFirstChild("Humanoid"));
        Globals::HumanoidPtr = Humanoid.Address;

        Instance Camera(Workspace.FindFirstChild("Camera"));
        Globals::CameraPtr = Camera.Address;

        Memory::write<float>(Globals::HumanoidPtr + Offsets::JumpPower, 100.0f);
        Memory::write<float>(Globals::HumanoidPtr + Offsets::WalkSpeed, 100.0f);
        Memory::write<float>(Globals::HumanoidPtr + Offsets::WalkSpeedCheck, 100.0f);

        Memory::write<float>(Globals::HumanoidPtr + Offsets::Health, 500.0f);
    }


}


int main() {
    std::thread(mainthread).detach();
    std::cin.get();
}