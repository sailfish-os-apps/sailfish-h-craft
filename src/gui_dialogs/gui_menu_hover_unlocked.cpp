// Written by Michael Zeilfelder, please check licenseHCraft.txt for the zlib-style license text.

#include "gui_menu_hover_unlocked.h"
#include "gui_menu_championship_progress.h"
#include "../gui.h"
#include "../main.h"
#include "../config.h"

using namespace irr;
using namespace gui;

GuiMenuHoverUnlocked::GuiMenuHoverUnlocked(const Config& config)
: GuiDialog(config)
, mImageHover(0)
{
}

bool GuiMenuHoverUnlocked::Load(const char* filename_, bool reloadLast_)
{
    bool ok = GuiDialog::Load(filename_, reloadLast_);
    if ( ok )
    {
		ADD_EVENT_HANDLER( "id_continue", GuiMenuHoverUnlocked, OnButtonContinue );

        const IGUIElement * root = GetDialogRoot();
        if ( !root )
            return false;

        std::string errorMsg("GuiMenuHoverUnlocked::Load");
        mImageHover = static_cast<IGUIImage*>(GetElementByName(root, "preview", errorMsg));
    }
    return ok;
}

void GuiMenuHoverUnlocked::SetHover(unsigned int idx_)
{
    if ( idx_ >= APP.GetNumHovers() )
        return;

    const Hover& hover = APP.GetHover(idx_);
    std::string texName(APP.GetConfig()->GetPathMedia() + hover.mImage);
    video::ITexture * texture = GetDriver()->getTexture( texName.c_str() );
    if ( texture && mImageHover )
    {
        mImageHover->setImage(texture);
    }
}

bool GuiMenuHoverUnlocked::OnButtonContinue(const irr::SEvent &event_)
{
    if ( event_.GUIEvent.EventType == EGET_BUTTON_CLICKED )
    {
        APP.GetGui()->SetActiveDialog( APP.GetGui()->GetGuiMenuChampionshipProgress() );
    }
    return false;
}
