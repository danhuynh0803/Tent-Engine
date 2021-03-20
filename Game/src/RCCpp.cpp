#include "RCCpp.h"
#include "RuntimeObjectSystem/RuntimeObjectSystem.h"
#include "RuntimeObjectSystem/IObjectFactorySystem.h"
#include "EntitySystem.h"
#include "StdioLogSystem.h"
#include "Systems.h"

RCCpp::RCCpp(IGame* game)
{
    systemTable = new SystemTable();
    //PerModuleInterface::g_pSystemTable = systemTable; //How come auora doesnt need this?
    
    g_Logger = new StdioLogSystem();//RCCpComplier logger(customizable)

    // Set the systems library to refer to this system table
    // Not sure why Auora had this
    gSys = systemTable;

    systemTable->pGame = game;


    //systemTable->activeScene = std::make_shared<Scene>();
    //systemTable->activeScene->OnStart();
    //systemTable->sceneRenderer = SceneRenderer::Get();

    systemTable->runtimeObjectSystem = new RuntimeObjectSystem();
    systemTable->pObjectFactorySystem = systemTable->runtimeObjectSystem->GetObjectFactorySystem();
    systemTable->pObjectFactorySystem->SetObjectConstructorHistorySize(10);
    systemTable->pEntitySystem = new EntitySystem();

    //sys->pRuntimeObjectSystem->GetFileChangeNotifier();

    //Complier settings
    {
#ifdef _WIN32
        //entt needs C++17 or higher support
        //for msvc,cl
        //https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically?view=msvc-160
        systemTable->runtimeObjectSystem->SetAdditionalCompileOptions("/std:c++latest");
        //systemTable.runtimeObjectSystem->SetAdditionalCompileOptions("/std:c++latest /DXX");  // #define _XX matches with /DXX
#else
        //for gcc or clang
        systemTable.runtimeObjectSystem->SetAdditionalCompileOptions("-std:c++17");
#endif // _WIN32

        FileSystemUtils::Path basePath = systemTable->runtimeObjectSystem->FindFile(__FILE__);

        //.lib
        FileSystemUtils::Path LibDir = basePath.ParentPath().ParentPath().ParentPath() / "Build" / "lib" / "Debug"; //our .libs
        systemTable->runtimeObjectSystem->AddLibraryDir(LibDir.c_str());
        systemTable->runtimeObjectSystem->AddLibraryDir(basePath.ParentPath().ParentPath().c_str());//this is where i put assimp lib for now

        //Not sure how these works
        //FileSystemUtils::Path SceneSourceDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Scene" / "src";
        //FileSystemUtils::Path CoreSourceDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Core" / "src";
        //FileSystemUtils::Path RendererSourceDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Renderer" / "src";
        //systemTable.runtimeObjectSystem->AddPathToSourceSearch(SceneSourceDir.c_str());
        //systemTable.runtimeObjectSystem->AddPathToSourceSearch(CoreSourceDir.c_str());
        //systemTable.runtimeObjectSystem->AddPathToSourceSearch(RendererSourceDir.c_str());

        //.h
        FileSystemUtils::Path includeDir = basePath.ParentPath().ParentPath() / "include";
        FileSystemUtils::Path RCCppIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "RuntimeCompiledCPlusPlus" / "Aurora";
        FileSystemUtils::Path SceneIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Scene" / "include";
        FileSystemUtils::Path CoreIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Core" / "include";
        FileSystemUtils::Path enttIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "entt" / "include";
        FileSystemUtils::Path spdlogIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "spdlog" / "include";
        FileSystemUtils::Path imguiIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "imgui" / "include" / "imgui";
        FileSystemUtils::Path ImguiIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "ImGui" / "include";
        FileSystemUtils::Path RendererIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Renderer" / "include";
        FileSystemUtils::Path AudioIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Audio" / "include";
        FileSystemUtils::Path ScriptingIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Scripting" / "include";
        FileSystemUtils::Path PhysicsIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Physics" / "include";
        FileSystemUtils::Path ParticlesIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Particles" / "include";
        FileSystemUtils::Path glmIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "glm";
        FileSystemUtils::Path assimpIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "assimp" / "include";
        //FileSystemUtils::Path assimpImporterDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "assimp" / "code";
        //FileSystemUtils::Path assimpBuildDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "assimp" / "Build";
        FileSystemUtils::Path assimpBuildIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "assimp" / "Build" / "include"; //for assimp/config.h, generated by cmake
        FileSystemUtils::Path UtilIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Util" / "include";
        FileSystemUtils::Path gladIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "glad" / "include";
        FileSystemUtils::Path EventsIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Events" / "include";
        FileSystemUtils::Path jsonIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "nlohmann" / "include";
        FileSystemUtils::Path bullet3IncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "bullet3" / "src";
        FileSystemUtils::Path tracyIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "tracy";
        FileSystemUtils::Path luaIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "lua-5.4.0" / "include" / "Lua";
        FileSystemUtils::Path glfwIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "glfw" / "include";
        FileSystemUtils::Path OpenGLIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Renderer" / "OpenGL" / "include";
        FileSystemUtils::Path stbIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "stb";
        FileSystemUtils::Path CommandIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Command" / "include";
#ifdef _WIN32
        FileSystemUtils::Path FileSystemIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Platform" / "Windows" / "include";
        FileSystemUtils::Path fmodIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "fmod" / "Windows" / "core" / "inc";
        FileSystemUtils::Path fmodStudioIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "fmod" / "Windows" / "studio" / "inc";
#else
        FileSystemUtils::Path FileSystemIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Engine" / "Platform" / "Linux" / "include";
        FileSystemUtils::Path fmodIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "fmod" / "Linux" / "core" / "inc";
        FileSystemUtils::Path fmodStudioIncludeDir = basePath.ParentPath().ParentPath().ParentPath() / "Vendor" / "fmod" / "Linux" / "studio" / "inc";
#endif // _WIN32

        systemTable->runtimeObjectSystem->AddIncludeDir(includeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(RCCppIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(SceneIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(CoreIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(enttIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(spdlogIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(imguiIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(ImguiIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(RendererIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(AudioIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(ScriptingIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(PhysicsIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(ParticlesIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(glmIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(assimpIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(assimpBuildIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(UtilIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(gladIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(EventsIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(jsonIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(bullet3IncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(tracyIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(luaIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(glfwIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(OpenGLIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(FileSystemIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(fmodIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(fmodStudioIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(stbIncludeDir.c_str());
        systemTable->runtimeObjectSystem->AddIncludeDir(CommandIncludeDir.c_str());
    }
}

bool RCCpp::Init()
{
    //Environment init
    if (!systemTable->runtimeObjectSystem->Initialise(g_Logger, systemTable))
    {
        delete systemTable->runtimeObjectSystem;
        systemTable->runtimeObjectSystem = nullptr;
        return false;
    }

    return true;
}

void RCCpp::Shutdown()
{
    systemTable->RCCppEntry->Shutdown();
}

RCCpp::~RCCpp()
{
    delete systemTable->RCCppEntry;

    delete systemTable->pEntitySystem;
    delete systemTable->runtimeObjectSystem;
    delete g_Logger;
    delete systemTable;
    
    //ObjectFactorySystem's lifecycle is controlled by RCCpp.

    //delete systemTable.RCCppEntry;
    //delete sys->pTimeSystem;
    //delete systemTable.pLogSystem;
}