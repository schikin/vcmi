/*
 * RandomMapTemplateTab.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include "CSelectionBase.h"
#include "lib/rmg/CRmgTemplate.h"
#include "VCMI_Lib.h"

class CSlider;
class CLabel;

struct Padding {
    int top;
    int bottom;
    int left;
    int right;
};


class RandomMapTemplateTab : public CIntObject
{
	struct ListItem : public CIntObject
	{
//		std::shared_ptr<CLabel> labelMinPlayers;
//		std::shared_ptr<CLabel> labelMaxPlayers;
//		std::shared_ptr<CLabel> labelMapSizeLetter;
		std::shared_ptr<CLabel> labelName;
		std::shared_ptr<CRmgTemplate> rmgTemplate;

		explicit ListItem(Point position);
		void updateItem(std::shared_ptr<CRmgTemplate> rmgTemplate = {}, bool selected = false);
	};
	std::vector<std::shared_ptr<ListItem>> listItems;

public:
    RandomMapTemplateTab();

    std::vector<std::shared_ptr<CRmgTemplate>> allItems;
	std::vector<std::shared_ptr<CRmgTemplate>> curItems;
	std::function<void(std::shared_ptr<CRmgTemplate>)> callOnSelect;

	void clickLeft(tribool down, bool previousState) override;
	void keyPressed(const SDL_KeyboardEvent & key) override;
	void onDoubleClick() override;

	void updateMapSize(int3 size);
	//void select(std::shared_ptr<CRmgTemplate>);

	void setParent(std::shared_ptr<RandomMapTab> parent);

	int getLine();
	std::shared_ptr<CRmgTemplate> getSelected() const;

private:

	std::shared_ptr<CPicture> background;
	std::shared_ptr<CSlider> slider;
	std::shared_ptr<RandomMapTab> parent;

	std::shared_ptr<CRmgTemplate> selectionTemplate;
    size_t selectionPos{};
    std::shared_ptr<ListItem> selectionItem;

    void updateListItems();

    void select(int position); //position: <0 - positions>  position on the screen
    void selectAbs(int position); //position: absolute position in curItems vector
    void sliderMove(int slidPos);

    void rememberCurrentSelection(); //remember selection in setting so next time the same template is autoselected
    void restoreLastSelection();

    /*
     * Apply the following logic:
     * 1. Read the template id from settings
     * 2. If that template exists - set it
     * 3. In any other case - select the first template
     */
    void initializeSelection();

    std::vector<std::shared_ptr<CRmgTemplate>> loadItems();

    std::shared_ptr<ListItem> find(std::shared_ptr<CRmgTemplate> rmgTemplate);

};
