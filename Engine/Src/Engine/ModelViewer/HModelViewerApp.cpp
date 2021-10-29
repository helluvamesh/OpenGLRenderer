#include "Engine\pch.h"
#include "HModelViewerApp.h"
#include "Engine\Asset\HAssetfactory.h"
#include "Engine\UI\HWindow3D.h"
#include "Engine\Render\Renderer\HRenderer.h"
#include "Engine\ModelViewer\HModelViewerUI.h"
#include "Engine\UI\SNativeMessage.h"
#include "Engine\Util\SEngine.h"


int32 HModelViewerApp::Start(int32 ArgCount, const char** Args)
{
    SLog::Message(TX("Starting App"));

    // ------------------------- READ CONFIG -----------------------------

    DModelViewerConfig config;

    {
        DString8 jstr;
        if (STextFile::ReadUTF8File(SEngine::PathRoot + TX("Config.json"), /*out*/jstr) == false) return 1;

        DJsonParser jsonParser;
        if (jsonParser.ParseStruct(/*out*/config, DModelViewerConfig::GetDeclaredClass(), jstr) == false) return 1;
    }

    // --------------- CREATE WINDOW WITH OPENGL CONTEXT -----------------

    shared<HWindow3D> window = MakeShared<HWindow3D>(config.WindowWidth, config.WindowHeight, "Window 3D");

    // --------------------- CHECK OPENGL VERSION ------------------------

    int32 majorVersion, minorVersion;
    window->GetOpenGLVersion(/*out*/majorVersion, /*out*/minorVersion);
    
    SLog::Message(DString::Format(TX("OpenGL {0}.{1}"), { majorVersion, minorVersion }));

    int32 versionCmp = HRenderer::CompareOpenGLVersions(
        majorVersion, minorVersion, config.MinOpenGLVersionMajor, config.MinOpenGLVersionMajor
    );
    if (versionCmp < 0)
    {
        DString message = DString::Format(
            TX("OpenGL {0}.{1} or higher is needed, you have {2}.{3}"),
            { config.MinOpenGLVersionMajor, config.MinOpenGLVersionMinor, majorVersion, minorVersion }
        );
        SLog::Error(message);
        SNativeMessage::ShowMessage(message, TX("Error"));

        return 1;
    }

    // ------------- START RENDERER AND MODEL VIEWER -------------

    shared<HRenderer> renderer = MakeShared<HRenderer>();
    renderer->Resize(config.WindowWidth, config.WindowHeight);

    shared<HModelViewer> viewer = MakeShared<HModelViewer>(renderer);

    shared<HModelViewerUI> ui = MakeShared<HModelViewerUI>(window, renderer, viewer, config);
    ui->Init();

    window->OnRenderStart.Bind(renderer, &HRenderer::RenderStart);
    window->OnRenderFrame.Bind(renderer, &HRenderer::RenderFrame);
    window->OnRenderStop.Bind(renderer, &HRenderer::RenderStop);
    window->OnWindowSize.Bind(renderer, &HRenderer::Resize);

    window->StartRender();

    return 0;
}
