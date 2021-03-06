#include "project/Menu.hpp"

using namespace glimac;

Menu::Menu(bool mainMenu)
{
    m_button = PLAY;
    m_mainMenu = mainMenu;
    if (mainMenu)   // Main menu
    {
        m_texture[0] = new Texture("../Code/assets/textures/buttons/new-game-on.png");
        m_texture[1] = new Texture("../Code/assets/textures/buttons/new-game-off.png");
        m_texture[2] = new Texture("../Code/assets/textures/buttons/continue-on.png");
        m_texture[3] = new Texture("../Code/assets/textures/buttons/continue-off.png");
    }
    else    // Pause Menu
    {
        m_texture[0] = new Texture("../Code/assets/textures/buttons/restart-on.png");
        m_texture[1] = new Texture("../Code/assets/textures/buttons/restart-off.png");
        m_texture[2] = new Texture("../Code/assets/textures/buttons/save-on.png");
        m_texture[3] = new Texture("../Code/assets/textures/buttons/save-off.png");
    }
    m_texture[4] = new Texture("../Code/assets/textures/buttons/quit-on.png");
    m_texture[5] = new Texture("../Code/assets/textures/buttons/quit-off.png");
    m_texture[6] = new Texture("../Code/assets/textures/MAIN_MENU.png");    // Background
    m_texture[7] = new Texture("../Code/assets/textures/HELP MENU.png");  // Help

    for (unsigned int i = 0; i < 8; ++i)
        m_texture[i]->loadTexture();

    m_hide = false;
    m_infos = false;
}

Menu::~Menu()
{
    for (unsigned int i = 0; i < 8; ++i)
        delete(m_texture[i]);
}

Texture** Menu::getTextures()
{
    return m_texture;
}

Menu::Button Menu::getButton() const
{
    return m_button;
}

bool Menu::isMainMenu() const
{
    return m_mainMenu;
}

void Menu::selectButton(Controller* controller, AudioManager* audioManager)
{
    // Game Infos
    if (controller->getInterfaceAction() == Controller::Key::I)
    {
        switchInfos();
        controller->setInterfaceAction(Controller::Key::NONE);
        return;
    }

    // Hide Menu
    if (controller->getInterfaceAction() == Controller::Key::H)
    {
        switchHide();
        controller->setInterfaceAction(Controller::Key::NONE);
        return;
    }
    
    if ((controller->getInterfaceAction() == Controller::Key::DOWN) || (controller->getInterfaceAction() == Controller::Key::UP))
    {
        audioManager->playSound(0,1);

        if (controller->getInterfaceAction() == Controller::Key::UP)
        {
            if(m_button == EXIT)
                m_button = CONTINUE;
            else if (m_button == CONTINUE)
                m_button = PLAY;
            controller->setInterfaceAction(Controller::Key::NONE);      
        }
        else
        {
            if(m_button == PLAY)
                m_button = CONTINUE;
            else if (m_button == CONTINUE)
                m_button = EXIT;
            controller->setInterfaceAction(Controller::Key::NONE);   
        }
    }
}

void Menu::switchHide()
{
    m_hide = !m_hide;
}

void Menu::switchInfos()
{
    m_infos = !m_infos;
}