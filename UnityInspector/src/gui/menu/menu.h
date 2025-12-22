#pragma once

struct Menu 
{
    static void RenderMenu();
private:
    static void RenderVisualTab();
    static void RenderAimTab();
    static void RenderMiscTab();
    static void RenderInfoTab();
    static void RenderDebugTab();
};
