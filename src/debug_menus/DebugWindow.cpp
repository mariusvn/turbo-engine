
#ifdef __TURBO_USE_IMGUI__

    #include <turbo/debug_menus/DebugWindow.hpp>

namespace debug {
    void DebugWindow::set_color_theme() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* c = style.Colors;

        // ---- palette : noir délavé (chaud) + jaune fort -----------------------
        const ImVec4 bg0      = ImVec4(0.090f, 0.090f, 0.082f, 1.00f); // fenêtres
        const ImVec4 bg1      = ImVec4(0.135f, 0.134f, 0.122f, 1.00f); // champs / boutons
        const ImVec4 bg2      = ImVec4(0.180f, 0.178f, 0.162f, 1.00f); // survol
        const ImVec4 bg3      = ImVec4(0.235f, 0.232f, 0.208f, 1.00f); // actif / poignées
        const ImVec4 panel    = ImVec4(0.110f, 0.110f, 0.100f, 1.00f); // barres / titres
        const ImVec4 popup    = ImVec4(0.075f, 0.075f, 0.068f, 0.98f);
        const ImVec4 border   = ImVec4(0.230f, 0.228f, 0.205f, 0.50f);
        const ImVec4 text     = ImVec4(0.915f, 0.905f, 0.870f, 1.00f);
        const ImVec4 textdis  = ImVec4(0.480f, 0.474f, 0.435f, 1.00f);

        const ImVec4 accent   = ImVec4(1.000f, 0.780f, 0.050f, 1.00f); // jaune fort
        const ImVec4 accentHi = ImVec4(1.000f, 0.855f, 0.270f, 1.00f);
        const ImVec4 acc20    = ImVec4(1.000f, 0.780f, 0.050f, 0.20f);
        const ImVec4 acc32    = ImVec4(1.000f, 0.780f, 0.050f, 0.32f);
        const ImVec4 acc46    = ImVec4(1.000f, 0.780f, 0.050f, 0.46f);

        c[ImGuiCol_Text]                  = text;
        c[ImGuiCol_TextDisabled]          = textdis;
        c[ImGuiCol_WindowBg]              = bg0;
        c[ImGuiCol_ChildBg]               = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_PopupBg]               = popup;
        c[ImGuiCol_Border]                = border;
        c[ImGuiCol_BorderShadow]          = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_FrameBg]               = bg1;
        c[ImGuiCol_FrameBgHovered]        = bg2;
        c[ImGuiCol_FrameBgActive]         = bg3;
        c[ImGuiCol_TitleBg]               = panel;
        c[ImGuiCol_TitleBgActive]         = panel;
        c[ImGuiCol_TitleBgCollapsed]      = bg0;
        c[ImGuiCol_MenuBarBg]             = panel;
        c[ImGuiCol_ScrollbarBg]           = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_ScrollbarGrab]         = bg3;
        c[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.33f, 0.32f, 0.28f, 1.00f);
        c[ImGuiCol_ScrollbarGrabActive]   = accent;
        c[ImGuiCol_CheckMark]             = accent;
        c[ImGuiCol_SliderGrab]            = accent;
        c[ImGuiCol_SliderGrabActive]      = accentHi;
        c[ImGuiCol_Button]                = bg1;
        c[ImGuiCol_ButtonHovered]         = acc20;   // survol jaune translucide -> "glow"
        c[ImGuiCol_ButtonActive]          = acc32;
        c[ImGuiCol_Header]                = acc20;
        c[ImGuiCol_HeaderHovered]         = acc32;
        c[ImGuiCol_HeaderActive]          = acc46;
        c[ImGuiCol_Separator]             = border;
        c[ImGuiCol_SeparatorHovered]      = acc32;
        c[ImGuiCol_SeparatorActive]       = accent;
        c[ImGuiCol_ResizeGrip]            = acc20;
        c[ImGuiCol_ResizeGripHovered]     = acc32;
        c[ImGuiCol_ResizeGripActive]      = acc46;
        c[ImGuiCol_Tab]                   = bg1;
        c[ImGuiCol_TabHovered]            = acc32;
        c[ImGuiCol_TabActive]             = bg3;
        c[ImGuiCol_TabUnfocused]          = panel;
        c[ImGuiCol_TabUnfocusedActive]    = bg2;
        c[ImGuiCol_PlotLines]             = accent;
        c[ImGuiCol_PlotLinesHovered]      = accentHi;
        c[ImGuiCol_PlotHistogram]         = accent;
        c[ImGuiCol_PlotHistogramHovered]  = accentHi;
        c[ImGuiCol_TextSelectedBg]        = acc32;
        c[ImGuiCol_DragDropTarget]        = accent;
        c[ImGuiCol_NavHighlight]          = accent;
        c[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        c[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.10f, 0.10f, 0.09f, 0.55f);
        c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.04f, 0.04f, 0.03f, 0.65f);

        // ---- formes & espacement : flat mais avec du relief --------------------
        style.WindowRounding      = 7.0f;
        style.ChildRounding       = 7.0f;
        style.FrameRounding       = 5.0f;
        style.PopupRounding       = 7.0f;
        style.GrabRounding        = 5.0f;
        style.TabRounding         = 5.0f;
        style.ScrollbarRounding   = 6.0f;

        style.WindowBorderSize    = 1.0f;
        style.FrameBorderSize     = 0.0f;
        style.PopupBorderSize     = 1.0f;
        style.ChildBorderSize     = 1.0f;

        style.WindowPadding       = ImVec2(13.0f, 12.0f);
        style.FramePadding        = ImVec2(11.0f, 6.0f);
        style.ItemSpacing         = ImVec2(10.0f, 8.0f);
        style.ItemInnerSpacing    = ImVec2(8.0f, 6.0f);
        style.CellPadding         = ImVec2(8.0f, 5.0f);
        style.ScrollbarSize       = 13.0f;
        style.GrabMinSize         = 11.0f;

        style.WindowTitleAlign    = ImVec2(0.5f, 0.5f);
        // Retire la flèche de collapse des barres de titre (moins "ImGui brut").
        style.WindowMenuButtonPosition = ImGuiDir_None;
        style.SeparatorTextBorderSize = 2.0f;
        style.SeparatorTextAlign  = ImVec2(0.0f, 0.5f);
        style.SeparatorTextPadding = ImVec2(18.0f, 4.0f);
    }
}

#endif
