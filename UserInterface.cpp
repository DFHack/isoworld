#include "UserInterface.h"
#include "console.h"
#include "MapSection.h"

agui::Gui *gui = NULL;
agui::Allegro5Input* inputHandler = NULL;
agui::Allegro5Graphics* graphicsHandler = NULL;

agui::Font *defaultFont = NULL;

MainScreen * main_screen;


void ConnectActionListener::actionPerformed(const agui::ActionEvent &evt) {
    agui::Button * button = dynamic_cast<agui::Button*>(evt.getSource());
    toggle_df_connection();
}
void LoadActionListener::actionPerformed(const agui::ActionEvent &evt) {
    start_load_dialog();
}
void TilesetActionListener::actionPerformed(const agui::ActionEvent &evt) {
    agui::DropDown * drop_down = dynamic_cast<agui::DropDown*>(evt.getSource());
    mapsection->current_tileset = drop_down->getSelectedIndex();
}

void FollowActionListener::actionPerformed(const agui::ActionEvent &evt) {
    center_on_loaded_map = !center_on_loaded_map;
}

MainScreen::MainScreen(agui::Gui *guiInstance) {
        mGui = guiInstance;
        mGui->add(&flow);
        flow.add(&connect_button);
        connect_button.setSize(100,30);
        connect_button.setText("Connect");
        connect_button.addActionListener(&connect_action);

        flow.add(&load_button);
        load_button.setSize(80,30);
        load_button.setText("Load Map");
        load_button.addActionListener(&load_action);

        flow.add(&follow_button);
        follow_button.setSize(80,30);
        follow_button.setText("Follow");
        follow_button.addActionListener(&follow_action);

        flow.add(&tileset_selector);
        tileset_selector.setSize(100,30);
        tileset_selector.setText("Tileset");
        tileset_selector.addActionListener(&tile_set_swap_action);
        tileset_selector.setResizeToWidestItem(true);

        last_connection_status = 0;

}

void MainScreen::UpdateText() {
    if(last_connection_status != (connection_state!=NULL)){
        if((connection_state!=NULL))
            connect_button.setText("Disconnect");
        else
            connect_button.setText("Connect");
        last_connection_status = (connection_state!=NULL);
    }
    if(last_follow !=center_on_loaded_map) {
        if(center_on_loaded_map)
            follow_button.setText("Unfollow");
        else
            follow_button.setText("Follow");
        last_follow = center_on_loaded_map;
    }
}

void initializeAgui()
{

    //Set the image loader
    agui::Image::setImageLoader(new agui::Allegro5ImageLoader);

    //Set the font loader
    agui::Font::setFontLoader(new agui::Allegro5FontLoader);

    //Instance the input handler
    inputHandler = new agui::Allegro5Input();

    //Instance the graphics handler
    graphicsHandler = new agui::Allegro5Graphics();

    //Allegro does not automatically premultiply alpha so let Agui do it
    agui::Color::setPremultiplyAlpha(true);

    //Instance the gui
    gui = new agui::Gui();

    //Set the input handler
    gui->setInput(inputHandler);

    //Set the graphics handler
    gui->setGraphics(graphicsHandler);

    defaultFont = agui::Font::load("isoworld/DejaVuSans.ttf",14);

    //Setting a global font is required and failure to do so will crash.
    agui::Widget::setGlobalFont(defaultFont);


}

void add_widgets()
{
    main_screen = new MainScreen(gui);
}
void cleanup_gui()
{
    gui->getTop()->clear();
    delete main_screen;
    main_screen = NULL;
    delete gui;
    gui = NULL;
    delete inputHandler;
    delete graphicsHandler;
    inputHandler = NULL;
    graphicsHandler = NULL;

    delete defaultFont;
    defaultFont = NULL;
}
void render_gui()
{
    //render the widgets
    main_screen->UpdateText();
    gui->render();

}

void MainScreen::UpdateTilesetList(MapSection * mapsection) {
    for(int i = 0; i < mapsection->tileset_list.size(); i++) {
        tileset_selector.addItem(mapsection->tileset_list[i].tileset_folder);
    }
    tileset_selector.setSelectedIndex(mapsection->current_tileset);
    tile_set_swap_action.mapsection = mapsection;
}