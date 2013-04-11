#pragma once

#include "common.h"

#include <Agui/Agui.hpp>
#include <Agui/Backends/Allegro5/Allegro5.hpp>
#include <Agui/FlowLayout.hpp>

#include <Agui/Widgets/Button/Button.hpp>

class ConnectActionListener : public agui::ActionListener
{
public:
    virtual void actionPerformed(const agui::ActionEvent &evt);
};
class LoadActionListener : public agui::ActionListener
{
public:
    virtual void actionPerformed(const agui::ActionEvent &evt);
};

class MainScreen {
private:
    bool last_connection_status;
    agui::Button connect_button;
    agui::Button load_button;
    agui::Gui* mGui;
    agui::FlowLayout flow;
public:
    ConnectActionListener connect_action;
    LoadActionListener load_action;
    MainScreen(agui::Gui *guiInstance);
    void UpdateText();
};

extern MainScreen * main_screen;
extern agui::Allegro5Input* inputHandler;
extern agui::Gui *gui;

void initializeAgui();

void add_widgets();
void cleanup_gui();
void render_gui();