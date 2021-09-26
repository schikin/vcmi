/*
 * RandomMapTemplateTab.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "RandomMapTemplateTab.h"
#include "CSelectionBase.h"
#include "CLobbyScreen.h"

#include "../CGameInfo.h"
#include "../CMessage.h"
#include "../CBitmapHandler.h"
#include "../CPlayerInterface.h"
#include "../CServerHandler.h"
#include "../gui/CAnimation.h"
#include "../gui/CGuiHandler.h"
#include "../widgets/CComponent.h"
#include "../widgets/Buttons.h"
#include "../widgets/MiscWidgets.h"
#include "../widgets/ObjectLists.h"
#include "../widgets/TextControls.h"
#include "../windows/GUIClasses.h"
#include "../windows/InfoWindows.h"

#include "../../CCallback.h"

#include "../../lib/serializer/Connection.h"
#include "../../lib/rmg/CRmgTemplate.h"
#include "../../lib/rmg/CRmgTemplateStorage.h"

const struct Padding RmgTemplateListPadding{30, 10, 20, 20};
const int RmgTemplateListWidth = 372;
const int RmgTemplateListItems = 18;

RandomMapTemplateTab::ListItem::ListItem(Point position)
        : CIntObject(LCLICK, position) {
    OBJ_CONSTRUCTION_CAPTURING_ALL_NO_DISPOSE;

//    labelMinPlayers = std::make_shared<CLabel>(41, 0, FONT_SMALL, EAlignment::CENTER, Colors::WHITE);
//    labelMinPlayers->setAutoRedraw(false);
//
//    labelMaxPlayers = std::make_shared<CLabel>(41, 0, FONT_SMALL, EAlignment::CENTER, Colors::WHITE);
//    labelMaxPlayers->setAutoRedraw(false);
//
//    labelMapSizeLetter = std::make_shared<CLabel>(41, 0, FONT_SMALL, EAlignment::CENTER, Colors::WHITE);
//    labelMapSizeLetter->setAutoRedraw(false);

    labelName = std::make_shared<CLabel>(184, 0, FONT_SMALL, EAlignment::CENTER, Colors::WHITE);
    labelName->setAutoRedraw(false);
}

void RandomMapTemplateTab::ListItem::updateItem(std::shared_ptr<CRmgTemplate> newTemplate, bool selected) {
    rmgTemplate = newTemplate;

    if (!rmgTemplate) {
//        labelMapSizeLetter->disable();
//        labelMinPlayers->disable();
//        labelMaxPlayers->disable()
        labelName->disable();
        return;
    }

    auto color = selected ? Colors::YELLOW : Colors::WHITE;

    labelName->setText(rmgTemplate->getName());
    labelName->enable();
    labelName->setColor(color);
}

RandomMapTemplateTab::RandomMapTemplateTab() {
    recActions = 0;

    OBJ_CONSTRUCTION;

    loadItems();

    background = std::make_shared<CPicture>("RANMAPBK", 0, 6);
    pos = background->pos;

    std::string tabTitle = "Select random map template";

    this->parent = parent;

    for (int i = 0; i < RmgTemplateListItems; i++)
        listItems.push_back(
                std::make_shared<ListItem>(Point(RmgTemplateListPadding.left, RmgTemplateListPadding.top + i * 25)));


    slider = std::make_shared<CSlider>(
            Point(RmgTemplateListWidth, RmgTemplateListPadding.top),
            RmgTemplateListItems * 25,
            std::bind(&RandomMapTemplateTab::sliderMove, this, _1),
            RmgTemplateListItems,
            (int) curItems.size(), 0,
            false,
            CSlider::BLUE);

    initializeSelection();
}

void RandomMapTemplateTab::clickLeft(tribool down, bool previousState) {
    if (down) {
        int line = getLine();
        if (line != -1)
            select(line);
    }
}

void RandomMapTemplateTab::keyPressed(const SDL_KeyboardEvent &key) {
    if (key.state != SDL_PRESSED)
        return;

    int moveBy = 0;
    switch (key.keysym.sym) {
        case SDLK_UP:
            moveBy = -1;
            break;
        case SDLK_DOWN:
            moveBy = +1;
            break;
        case SDLK_PAGEUP:
            moveBy = -(int) listItems.size() + 1;
            break;
        case SDLK_PAGEDOWN:
            moveBy = +(int) listItems.size() - 1;
            break;
        case SDLK_HOME:
            select(-slider->getValue());
            return;
        case SDLK_END:
            select((int) curItems.size() - slider->getValue());
            return;
        default:
            return;
    }
    select((int) selectionPos - slider->getValue() + moveBy);
}

void RandomMapTemplateTab::onDoubleClick() {
    if (getLine() != -1) //double clicked scenarios list
    {

    }
}

void RandomMapTemplateTab::select(int position) {
    if (!curItems.size())
        return;

    // New selection. py is the index in curItems.
    int py = position + slider->getValue();
    vstd::amax(py, 0);
    vstd::amin(py, curItems.size() - 1);

    selectionPos = py;

    if (position < 0)
        slider->moveBy(position);
    else if (position >= listItems.size())
        slider->moveBy(position - (int) listItems.size() + 1);

    auto item = curItems[py];

    selectionTemplate = item;
    selectionItem = find(selectionTemplate);

    updateListItems();
    if (callOnSelect)
        callOnSelect(curItems[py]);
}


void RandomMapTemplateTab::selectAbs(int position) {
    select(position - slider->getValue());
}

void RandomMapTemplateTab::sliderMove(int slidPos) {
    if (!slider)
        return; // ignore spurious call when slider is being created
    updateListItems();
    redraw();
}

void RandomMapTemplateTab::updateListItems() {
    // elemIdx is the index of the maps or saved game to display on line 0
    // slider->capacity contains the number of available screen lines
    // slider->positionsAmnt is the number of elements after filtering
    int elemIdx = slider->getValue();
    for (auto item : listItems) {
        if (elemIdx < curItems.size()) {
            item->updateItem(curItems[elemIdx], elemIdx == selectionPos);
            elemIdx++;
        } else {
            item->updateItem();
        }
    }
}

int RandomMapTemplateTab::getLine() {
    int line = -1;
    Point clickPos(GH.current->button.x, GH.current->button.y);
    clickPos = clickPos - pos.topLeft();

    // Ignore clicks on save name area
    // TODO do we assume all calculations are based on 600x800 screen here?
    int maxPosY = 600 - RmgTemplateListPadding.bottom;

    if (clickPos.y > RmgTemplateListPadding.top && clickPos.y < maxPosY && clickPos.x > RmgTemplateListPadding.left && clickPos.x < RmgTemplateListPadding.left + RmgTemplateListWidth) {
        line = (clickPos.y - RmgTemplateListPadding.top) / 25; //which line
    }

    return line;
}

std::shared_ptr<RandomMapTemplateTab::ListItem> RandomMapTemplateTab::find(std::shared_ptr<CRmgTemplate> rmgTemplate) {

    //return std::find_if(begin(allItems), end(allItems),[](const std::shared_ptr<ListItem> &i) { return i->rmgTemplate == rmgTemplate; });
}

std::shared_ptr<CRmgTemplate> RandomMapTemplateTab::getSelected() const {
    return selectionTemplate;
}

void RandomMapTemplateTab::initializeSelection() {
    select(0);
}

void RandomMapTemplateTab::rememberCurrentSelection() {
    //TODO: implement settings serialization

}

void RandomMapTemplateTab::restoreLastSelection() {
    //TODO: implement settings deserialization
}

std::vector<std::shared_ptr<CRmgTemplate>> RandomMapTemplateTab::loadItems() {
    auto templateMap = VLC->tplh->getTemplates();

    allItems = std::vector<std::shared_ptr<CRmgTemplate>>();

    //this is a C++17 syntax. do we support it?
    for ( const auto& [key, value]: templateMap) {
        allItems.push_back(std::shared_ptr<CRmgTemplate>(value));
    }

    return allItems;
}

void RandomMapTemplateTab::setParent(std::shared_ptr<RandomMapTab> parent) {
    this->parent = parent;
}

void RandomMapTemplateTab::updateMapSize(int3 size) {

}

