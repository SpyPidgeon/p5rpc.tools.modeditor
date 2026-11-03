#pragma once
#include "panels.h"

class FilePanel : SearchablePanel
{
public:
    FilePanel() { this->label = "Files"; this->refreshable = false; }
    static FilePanel* GetInstance() { FilePanel* instance = new FilePanel(); return instance; }

    void RenderLogic() override;
    void InspectorLogic() override;
    void ScanValues() override;
   // void ApplyChanges() override; Useless for now, will implement later.
   // void Refresh() override;

    static const uint16_t FILE_ARRAY_SIZE = 512;
    std::array<GFDFileInfo, FILE_ARRAY_SIZE>* gfdFiles;
};
static FilePanel* filePanel = FilePanel::GetInstance();