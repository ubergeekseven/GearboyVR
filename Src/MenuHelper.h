#ifndef ANDROID_MENUHELPER_H
#define ANDROID_MENUHELPER_H

#include "KingInclude.h"

class MenuItem {
 public:
  bool Selectable = false;
  bool Selected = false;
  bool Visible = true;
  int PosX = 100, PosY = 100;
  int ScrollDelay = 15;
  int ScrollTimeV = 5;
  int ScrollTimeH = 5;

  void (*UpdateFunction)(MenuItem *item) = nullptr;

  virtual ~MenuItem() {}

  virtual int PressedUp() { return 0; }
  virtual int PressedDown() { return 0; }
  virtual int PressedLeft() { return 0; }
  virtual int PressedRight() { return 0; }
  virtual int PressedEnter() { return 0; }

  virtual void Update() {
    if (UpdateFunction != nullptr) UpdateFunction(this);
  }
  virtual void DrawText(float offsetX, float transparency) {}
  virtual void DrawTexture(float offsetX, float transparency) {}
};

class MenuLabel : public MenuItem {
 public:
  MenuLabel(FontManager::RenderFont *font, std::string text, int posX, int posY, int width,
            int height, ovrVector4f color) {
    Font = font;
    Text = text;
    // center text
    int textWidth = FontManager::GetWidth(*font, text);
    PosX = posX + width / 2 - textWidth / 2;
    PosY = posY + height / 2 - font->FontSize;
    Color = color;
  }

  virtual ~MenuLabel() {}

  FontManager::RenderFont *Font;

  ovrVector4f Color;

  std::string Text;

  virtual void DrawText(float offsetX, float transparency) override;
};

class MenuImage : public MenuItem {
 public:
  MenuImage(GLuint imageId, int posX, int posY, int width, int height, ovrVector4f color) {
    ImageId = imageId;
    PosX = posX;
    PosY = posY;
    Width = width;
    Height = height;
    Color = color;
  }

  ovrVector4f Color;

  GLuint ImageId;

  int Width, Height;

  virtual ~MenuImage() {}

  virtual void DrawTexture(float offsetX, float transparency) override;
};

class MenuButton : public MenuItem {
 public:
  MenuButton(FontManager::RenderFont *font, GLuint iconId, std::string text, int posX, int posY,
             void (*pressFunction)(MenuItem *item), void (*leftFunction)(MenuItem *item),
             void (*rightFunction)(MenuItem *item)) {
    PosX = posX;
    PosY = posY;
    IconId = iconId;
    Text = text;
    Font = font;
    PressFunction = pressFunction;
    LeftFunction = leftFunction;
    RightFunction = rightFunction;
    Selectable = true;
  }

  virtual ~MenuButton() {}

  void (*PressFunction)(MenuItem *item) = nullptr;
  void (*LeftFunction)(MenuItem *item) = nullptr;
  void (*RightFunction)(MenuItem *item) = nullptr;

  FontManager::RenderFont *Font;

  GLuint IconId;

  std::string Text;

  virtual int PressedLeft() override {
    if (LeftFunction != nullptr) {
      LeftFunction(this);
      return 1;
    }
    return 0;
  }

  virtual int PressedRight() override {
    if (RightFunction != nullptr) {
      RightFunction(this);
      return 1;
    }
    return 0;
  }

  virtual int PressedEnter() override {
    if (PressFunction != nullptr) {
      PressFunction(this);
      return 1;
    }
    return 0;
  }

  virtual void DrawText(float offsetX, float transparency) override;

  virtual void DrawTexture(float offsetX, float transparency) override;
};

class MenuList : public MenuItem {
 public:
  MenuList(FontManager::RenderFont *font, void (*pressFunction)(Emulator::Rom *item),
           std::vector<Emulator::Rom> *romList, int posX, int posY, int width, int height) {
    Font = font;
    PressFunction = pressFunction;
    RomList = romList;

    PosX = posX;
    PosY = posY;
    Width = width;
    Height = height;

    listItemSize = (font->FontSize + 14);
    itemOffsetY = 7;
    maxListItems = height / listItemSize;
    scrollbarHeight = height;
    listStartY = posY + (scrollbarHeight - (maxListItems * listItemSize)) / 2;
    Selectable = true;
  }

  virtual ~MenuList() {}

  FontManager::RenderFont *Font;

  void (*PressFunction)(Emulator::Rom *item) = nullptr;

  std::vector<Emulator::Rom> *RomList;

  int maxListItems = 0;
  int Width = 0, Height = 0;
  int scrollbarWidth = 12, scrollbarHeight = 0;
  int listItemSize = 0;
  int itemOffsetY = 0;
  int listStartY = 0;

  int CurrentSelection = 0;
  int menuListState = 0;

  virtual int PressedUp() override {
    CurrentSelection--;
    if (CurrentSelection < 0) CurrentSelection = (int)(RomList->size() - 1);
    return 1;
  }

  virtual int PressedDown() override {
    CurrentSelection++;
    if (CurrentSelection >= RomList->size()) CurrentSelection = 0;
    return 1;
  }

  virtual int PressedEnter() override {
    if (RomList->size() <= 0) return 1;

    if (PressFunction != nullptr) PressFunction(&RomList->at(CurrentSelection));

    return 1;
  }

  virtual void Update() override {
    // scroll the list to the current Selection
    if (CurrentSelection - 2 < menuListState && menuListState > 0) {
      menuListState--;
    }
    if (CurrentSelection + 2 >= menuListState + maxListItems &&
        menuListState + maxListItems < RomList->size()) {
      menuListState++;
    }
  }

  /*
  virtual int PressedUp() override {
    ListItems[CurrentSelection]->Selected = false;

    CurrentSelection--;
    if (CurrentSelection < 0) {
      CurrentSelection = (int)(ListItems.size() - 1);
    }

    ListItems[CurrentSelection]->Selected = true;
  }

  virtual int PressedDown() override {
    ListItems[CurrentSelection]->Selected = false;

    CurrentSelection++;
    if (CurrentSelection >= ListItems.size()) {
      CurrentSelection = 0;
    }

    ListItems[CurrentSelection]->Selected = true;
  }
   */

  virtual void DrawText(float offsetX, float transparency) override;

  virtual void DrawTexture(float offsetX, float transparency) override;
};

class Menu {
 public:
  std::vector<MenuItem *> MenuItems;

  int CurrentSelection = 0;
  int buttonDownCount = 0;

 public:
  void (*BackPress)() = nullptr;

  void Init() { MenuItems[CurrentSelection]->Selected = true; }

  bool ButtonPressed(int buttonState, int lastButtonState, int button) {
    return buttonState & button && (!(lastButtonState & button) ||
                                    buttonDownCount > MenuItems[CurrentSelection]->ScrollDelay);
  }

  void MoveSelection(int dir) {
    // WARNIGN: this will not work if there is nothing selectable
    // LOG("Move %i" + dir);
    do {
      CurrentSelection += dir;
      if (CurrentSelection < 0) CurrentSelection = (int)(MenuItems.size() - 1);
      if (CurrentSelection >= MenuItems.size()) CurrentSelection = 0;
    } while (!MenuItems[CurrentSelection]->Selectable);
  }

  void Update(int buttonState, int lastButtonState) {
    MenuItems[CurrentSelection]->Selected = false;

    // could be done with a single &
    if (buttonState & BUTTON_LSTICK_UP || buttonState & BUTTON_DPAD_UP ||
        buttonState & BUTTON_LSTICK_DOWN || buttonState & BUTTON_DPAD_DOWN ||
        buttonState & BUTTON_LSTICK_LEFT || buttonState & BUTTON_DPAD_LEFT ||
        buttonState & BUTTON_LSTICK_RIGHT || buttonState & BUTTON_DPAD_RIGHT) {
      buttonDownCount++;
    } else {
      buttonDownCount = 0;
    }

    if (ButtonPressed(buttonState, lastButtonState, BUTTON_LSTICK_UP) ||
        ButtonPressed(buttonState, lastButtonState, BUTTON_DPAD_UP)) {
      buttonDownCount -= MenuItems[CurrentSelection]->ScrollTimeV;
      if (MenuItems[CurrentSelection]->PressedUp() == 0) {
        MoveSelection(-1);
      }
    }

    if (ButtonPressed(buttonState, lastButtonState, BUTTON_LSTICK_DOWN) ||
        ButtonPressed(buttonState, lastButtonState, BUTTON_DPAD_DOWN)) {
      buttonDownCount -= MenuItems[CurrentSelection]->ScrollTimeV;
      if (MenuItems[CurrentSelection]->PressedDown() == 0) {
        MoveSelection(1);
      }
    }

    MenuItems[CurrentSelection]->Selected = true;

    if (ButtonPressed(buttonState, lastButtonState, BUTTON_LSTICK_LEFT) ||
        ButtonPressed(buttonState, lastButtonState, BUTTON_DPAD_LEFT)) {
      buttonDownCount -= MenuItems[CurrentSelection]->ScrollTimeH;
      if (MenuItems[CurrentSelection]->PressedLeft() == 0) {
      }
    }

    if (ButtonPressed(buttonState, lastButtonState, BUTTON_LSTICK_RIGHT) ||
        ButtonPressed(buttonState, lastButtonState, BUTTON_DPAD_RIGHT)) {
      buttonDownCount -= MenuItems[CurrentSelection]->ScrollTimeH;
      if (MenuItems[CurrentSelection]->PressedRight() == 0) {
      }
    }

    if (ButtonPressed(buttonState, lastButtonState, BUTTON_A)) {
      buttonDownCount -= MenuItems[CurrentSelection]->ScrollTimeH;
      if (MenuItems[CurrentSelection]->PressedEnter() == 0) {
      }
    }

    if (buttonState & BUTTON_B && !(lastButtonState & BUTTON_B) && BackPress != nullptr) {
      BackPress();
    }

    for (int i = 0; i < MenuItems.size(); ++i) {
      MenuItems[i]->Update();
    }
  }
};

#endif  // ANDROID_MENUHELPER_H
