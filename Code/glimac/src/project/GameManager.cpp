#include "project/GameManager.hpp"
#include <string>
#include <iostream>
#include <random>

using namespace glimac;

GameManager::GameManager(Map* map)
{
    m_map = map;
    m_state = NORMAL;
    m_player.initialization();
}

GameManager::PacmanState GameManager::getState() const { return m_state;}
void GameManager::setState(PacmanState state) { m_state = state;}

// returns true if no edible on the map
bool GameManager::won() {
    
    return (m_map->getSuperPacGommes().empty() && m_map->getFruits().empty() && m_map->getPacGommes().empty());
}

// returns true if the players lost all his lives
bool GameManager::lost() {

    return (!m_player.getLife());
}

// For console only
void GameManager::play() {

    Pacman *p = m_map->getPacman();
    p->setSpeed(1);
    m_map->setPacman(*p);
    std::string line;
    while (!(this->won())) {
        m_map->display();
        std::cout << "Your move : " << std::endl;
        getline(std::cin, line);
        if (line == "Z") {
            if (!characterWallCollision(m_map->getPacman(), 'Z')) m_map->getPacman()->moveUp();
        }
        if (line == "Q") {
            if (!characterWallCollision(m_map->getPacman(), 'Q')) m_map->getPacman()->moveLeft();
        }
        if (line == "S") {
            if (!characterWallCollision(m_map->getPacman(), 'S')) m_map->getPacman()->moveDown();
        }
        if (line == "D") {
            if (!characterWallCollision(m_map->getPacman(), 'D')) m_map->getPacman()->moveRight();
        }
        if (line == "C") return;
        pacmanGhostCollision();
        pacmanEdibleCollision();
        ghostMove();
    }
}

// Returns true if can move, false if not (or if he takes door)
bool GameManager::moveCharacter(Character* character, Controller::Key action)
{
    switch (action)
	{
		case Controller::Z :
            if (!characterWallCollision(character, 'Z'))
            {
                character->moveUp();
                return true;
            }
			break;
		case Controller::Q :
            if (characterLeftDoorCollision(character))
            {
                Door* d = (Door*) (m_map->getStaticObjects())[character->getPosY()][character->getPosX()];
                character->setPosX(d->getDestX());
                character->setPosY(d->getDestY());
                character->moveLeft();
                return false;
            }
            if (!characterWallCollision(character, 'Q'))
            {
                character->moveLeft();
                return true;
            }
			break;
		case Controller::S :
            if (!characterWallCollision(character, 'S'))
            {
                character->moveDown();
                return true;
            }
			break;
		case Controller::D :
            if (characterRightDoorCollision(character))
            {
                Door* d = (Door*) (m_map->getStaticObjects())[character->getPosY()][character->getPosX()+1];
                character->setPosX(d->getDestX());
                character->setPosY(d->getDestY());
                character->moveRight();
                return false;
            }
            if (!characterWallCollision(character, 'D'))
            {
                character->moveRight();
                return true;
            }
			break;
		default :
			break;
	}
    return false;
}

void GameManager::pacmanMove(Controller* controller)
{
    Controller::Key action = controller->getPlayerAction();

    if (moveCharacter(m_map->getPacman(), action))
    {
        controller->setPlayerPreviousAction(action);
    }
    // Else keep doing the previous action
    else
    {
	    moveCharacter(m_map->getPacman(), controller->getPlayerPreviousAction());
    }
}

void GameManager::play(Controller* controller) {

    while (!(this->won())) {
        pacmanMove(controller);
        ghostMove();
        pacmanGhostCollision();
        pacmanEdibleCollision();
    }
}

// For console only
/*
void GameManager::display() {
    bool ghost = false;
    m_map->getPacman()->display();
    if (m_map->getStaticObjects().empty()) std::cout << "It's empty!" << std::endl;
    else {
        for (int i = 0; i < m_map->getNbX(); i++) {
            std::vector<StaticObject*> tmp = m_map->getStaticObjects().at(i);
            for (int  j = 0; j < m_map->getNbY(); j++) {
                ghost = false;
                for (int k = 0; k < m_map->getGhosts()->size(); k++) {
                    if ( (( m_map->getGhosts()[k].getPosX() >= tmp.at(j)->getPosX()) && (m_map->getGhosts()[k].getPosX() < tmp.at(j)->getPosX()+1)) && ((m_map->getGhosts()[k].getPosY() >= tmp.at(j)->getPosY()) && (m_map->getGhosts()[k].getPosY() < tmp.at(j)->getPosY()+1)) )
                 {std::cout << k+1 << " ";
                    ghost = true;}
                }
                if ( ((m_map->getPacman()->getPosX() >= tmp.at(j)->getPosX()) && (m_map->getPacman()->getPosX() < tmp.at(j)->getPosX()+1)) && ((m_map->getPacman()->getPosY() >= tmp.at(j)->getPosY()) && (m_map->getPacman()->getPosY() < tmp.at(j)->getPosY()+1)) )
                std::cout << m_map->getPacman()->getType() << " ";
                else if ((tmp.at(j) != NULL) && (!ghost)) std::cout << tmp.at(j)->getType() << " ";
                else if (!ghost) std::cout << "V ";
            }
            std::cout << std::endl;
        }
    }
}*/

void GameManager::pacmanGhostCollision() {

    for (int i = 0; i < m_map->getGhosts().size(); i++) {
        if (m_map->getPacman()->collision(m_map->getGhosts()[i])) {
            switch(this->getState()) {

                case GameManager::PacmanState::NORMAL :
                    m_player.loseLife();
                    m_map->getPacman()->reset();
                    for (int i = 0; i < m_map->getGhosts().size(); i++) {
                        m_map->getGhosts()[i]->reset();
                    }
                    std::cout << "Life lost. Life : " << m_player.getLife() << std::endl;
                    break;
                case GameManager::PacmanState::SUPER :
                    m_map->getGhosts()[i]->reset();
                    m_player.gainPoints(1000);  // (200, 400, 800, 1600)
                    std::cout << "Ghost Eaten" << std::endl;
                    break;
                default:
                    break;
            }
        }
    }
}

bool GameManager::ghostCollision() {

    for (int i = 0; i < m_map->getGhosts().size(); i++) {
        for (int j = 0; j < m_map->getGhosts().size() && j!=i; j++) {
               if (m_map->getGhosts()[i]->collision(m_map->getGhosts()[j])) return true;
        }
    }
    return false;
}

bool GameManager::wallCollisionUP(float fposY, int iposY, int iposX, float speed, Character* character)
{
    // Check if we're still going to be inside the cell
    if((fposY - speed) > iposY)
    {
        // Put the character on the left edge of the cell
        character->setPosX((float)iposX);
        // Ok we can move inside the cell
        return false;
    }
    // We're going to be on the upper cell
    else
    {
        // Check if we can go on the upper cell
        if(iposY >= 1)
        {
            // Check if upper cell is a wall
            if (m_map->getStaticObjects()[iposY-1][iposX]->getType()=='W')
            {
                // It's a wall, return true -> it's a collision, put the character on the top of the cell
                character->setPosY((float)iposY);
                return true;
            }
            else
            {
                // Put the character on the left edge of the cell
                character->setPosX((float)iposX);
                // It's ok we can move to the other cell
                return false;
            }
        }
        else
        {
            // We are at the edge of the game
            character->setPosY((float)iposY);
            return true;
        }
    }
}

bool GameManager::wallCollisionLEFT(float fposX, int iposY, int iposX, float speed, Character* character)
{
    if((fposX - speed) > iposX)
    {
        character->setPosY((float)iposY);
        return false;
    }
    else
    {
        if(iposX >= 1)
        {
            if (m_map->getStaticObjects()[iposY][iposX-1]->getType()=='W')
            {
                character->setPosX((float)iposX);
                return true;
            }
            else
            {
                character->setPosY((float)iposY);
                return false;
            }
        }
        else
        {
            character->setPosX((float)iposX);
            return true;
        }
    }
}

bool GameManager::wallCollisionDOWN(float fposY, int iposY, int iposX, float speed, Character* character)
{
    // Check if we can go on the bottom cell
    if(iposY+1 <= m_map->getNbX()-1)
    {
        // Check if bottom cell is a wall
        if (m_map->getStaticObjects()[iposY+1][iposX]->getType()=='W')
        {
            character->setPosY((float)iposY);
            return true;
        }
        else
        {
            character->setPosX((float)iposX);
            return false;
        }
    }
    else
    {
        character->setPosY((float)iposY);
        return true;
    }
}

bool GameManager::wallCollisionRIGHT(float fposX, int iposY, int iposX, float speed, Character* character)
{
    if(iposX+1 <= m_map->getNbY()-1)
    {
        if (m_map->getStaticObjects()[iposY][iposX+1]->getType()=='W')
        {
            character->setPosX((float)iposX);
            return true;
        }
        else
        {
            character->setPosY((float)iposY);
            return false;
        }
    }
    else
    {
        character->setPosX((float)iposX);
        return true;
    }
}

bool GameManager::characterWallCollision(Character* character, char direction) {
    float fposX = character->getPosX();
    float fposY = character->getPosY();
    int iposX = (int)fposX;   // Matrix index X
    int iposY = (int)fposY;   // Matrix index Y
    float speed = character->getSpeed();
    float seuil = 0.005;

    switch(direction) {
        case 'Z':
            // Check if we are close to the left edge
            if(fposX - iposX <= seuil)
            {
                return wallCollisionUP(fposY, iposY, iposX, speed, character);
            }
            // Check if we are close to the right edge
            else if (iposX+1 - fposX <= seuil)
            {
                // Consider we're on the right edge
                return wallCollisionUP(fposY, iposY, iposX+1, speed, character);
            }
            // We are between two cells
            else
            {
                // We can't go up
                return true;
            }
            break;
        case 'Q':
            if(fposY - iposY <= seuil)
            {
                return wallCollisionLEFT(fposX, iposY, iposX, speed, character);
            }
            else if (iposY+1 - fposY <= seuil)
            {
                return wallCollisionLEFT(fposX, iposY+1, iposX, speed, character);
            }
            else
            {
                return true;
            }
            break;
        case 'S':
            if(fposX - iposX <= seuil)
            {
                return wallCollisionDOWN(fposY, iposY, iposX, speed, character);
            }
            else if (iposX+1 - fposX <= seuil)
            {
                return wallCollisionDOWN(fposX, iposY, iposX+1, speed, character);
            }
            else
            {
                return true;
            }
            break;
        case 'D':
            if(fposY - iposY <= seuil)
            {
                return wallCollisionRIGHT(fposX, iposY, iposX, speed, character);
            }
            else if (iposY+1 - fposY <= seuil)
            {
                return wallCollisionRIGHT(fposX, iposY+1, iposX, speed, character);
            }
            else
            {
                return true;
            }
            break;
        default:
            return true;
            break;
    }

}

bool GameManager::characterLeftDoorCollision(Character* character)
{
    float fposX = character->getPosX();
    int iposX = (int)fposX;
    int iposY = character->getPosY();

    if (m_map->getStaticObjects()[iposY][iposX]->getType()=='D')
    {
        if (fposX - iposX < 0.1)
        {
            return true;
        }
    }
    return false;
}

bool GameManager::characterRightDoorCollision(Character* character)
{
    float fposX = character->getPosX();
    int iposX = (int)fposX;
    int iposY = character->getPosY();

    if (m_map->getStaticObjects()[iposY][iposX+1]->getType()=='D')
    {
        if (iposX+1 - fposX < 0.1)
            return true;
    }
    return false;
}

void GameManager::pacmanEdibleCollision() {
    // If we're going left, we want Pacman to be half inside the cell
    if ((m_map->getPacman()->getPosX() - (int)m_map->getPacman()->getPosX()) > m_map->getPacman()->getWidth())
        return;
    // If we're going up, we want Pacman to be half inside the cell
    if ((m_map->getPacman()->getPosY() - (int)m_map->getPacman()->getPosY()) > m_map->getPacman()->getHeight())
        return;
    if (m_map->getStaticObjects()[m_map->getPacman()->getPosY()][m_map->getPacman()->getPosX()]->getType()=='E'){
        Edible *e;
        e =  (Edible*) m_map->getStaticObjects()[m_map->getPacman()->getPosY()][m_map->getPacman()->getPosX()];
        m_player.gainPoints(e->gain());
        if (e->getTypeEdible() == Edible::Type::SUPER_PAC_GOMME) this->setState(GameManager::PacmanState::SUPER);

        m_map->getStaticObjects()[m_map->getPacman()->getPosY()][m_map->getPacman()->getPosX()]->setType('V');

        //std::cout << "Points : " << m_player.getPoints() << std::endl;
    }
}

// Shadow will follow Pacman all along, so he will find the shortest way to go to Pacman
void GameManager::shadowAI() {

    if (m_map->getGhosts()[Ghost::Type::SHADOW]->getDeath()) {

        shortestWay(Ghost::Type::SHADOW, m_map->getGhosts()[Ghost::Type::SHADOW]->getInitY(), m_map->getGhosts()[Ghost::Type::SHADOW]->getInitX());
    }
    else shortestWay(Ghost::Type::SHADOW, m_map->getPacman()->getPosX(), m_map->getPacman()->getPosY());
}

// Speedy aims the direction Pacman is going so he find the shortest way to this direction
void GameManager::speedyAI() {

    if (m_map->getGhosts()[Ghost::Type::SPEEDY]->getDeath()) {

        shortestWay(Ghost::Type::SPEEDY, m_map->getGhosts()[Ghost::Type::SPEEDY]->getInitY(), m_map->getGhosts()[Ghost::Type::SPEEDY]->getInitX());
    }
    else {
        switch(m_map->getPacman()->getOrientation()) {

            case Object::Orientation::LEFT:
                shortestWay(Ghost::Type::SPEEDY, 0, m_map->getPacman()->getPosY());
                break;

            case Object::Orientation::RIGHT:
                shortestWay(Ghost::Type::SPEEDY, m_map->getNbX(), m_map->getPacman()->getPosY());
                break;

            case Object::Orientation::UP:
                shortestWay(Ghost::Type::SPEEDY, m_map->getPacman()->getPosX(), 0);
                break;

            case Object::Orientation::DOWN:
                shortestWay(Ghost::Type::SPEEDY, m_map->getPacman()->getPosX(), m_map->getNbY());
                break;
            default:break;
        }
    }

}

// When Pacman gets really close to Bashful, he goes to Pacman's opposite direction
// else it keeps moving the way it is
// moves like Speedy until he gets close to pacman

void GameManager::bashfulAI() {

    if (m_map->getGhosts()[Ghost::Type::BASHFUL]->getDeath()) {

        shortestWay(Ghost::Type::BASHFUL, m_map->getGhosts()[Ghost::Type::BASHFUL]->getInitY(), m_map->getGhosts()[Ghost::Type::BASHFUL]->getInitX());
    }
    else if ((std::abs(m_map->getPacman()->getPosX() - m_map->getGhosts()[Ghost::Type::BASHFUL]->getPosX()) <= 2)  && (std::abs(m_map->getPacman()->getPosY() - m_map->getGhosts()[Ghost::Type::BASHFUL]->getPosY()) <= 10)) {

        switch(m_map->getPacman()->getOrientation()) {

            case Object::Orientation::LEFT:
                shortestWay(Ghost::Type::BASHFUL, m_map->getNbX(), m_map->getPacman()->getPosY());
                break;

            case Object::Orientation::RIGHT:
                shortestWay(Ghost::Type::BASHFUL, 0, m_map->getPacman()->getPosY());
                break;

            case Object::Orientation::UP:
                shortestWay(Ghost::Type::BASHFUL, m_map->getPacman()->getPosX(), m_map->getNbY());
                break;

            case Object::Orientation::DOWN:
                shortestWay(Ghost::Type::BASHFUL, m_map->getPacman()->getPosX(), 0);
                break;
            default:break;
        }
    }

    else {

        switch(m_map->getPacman()->getOrientation()) {

            case Object::Orientation::LEFT:
                shortestWay(Ghost::Type::BASHFUL, 0, m_map->getPacman()->getPosY());
                break;

            case Object::Orientation::RIGHT:
                shortestWay(Ghost::Type::BASHFUL, m_map->getNbX(), m_map->getPacman()->getPosY());
                break;

            case Object::Orientation::UP:
                shortestWay(Ghost::Type::BASHFUL, m_map->getPacman()->getPosX(), 0);
                break;

            case Object::Orientation::DOWN:
                shortestWay(Ghost::Type::BASHFUL, m_map->getPacman()->getPosX(), m_map->getNbY());
                break;
            default:break;
        }
    }
}

// goes around randomly
void GameManager::pokeyAI() {

    if (m_map->getGhosts()[Ghost::Type::POKEY]->getDeath()) {

        shortestWay(Ghost::Type::POKEY, m_map->getGhosts()[Ghost::Type::POKEY]->getInitY(), m_map->getGhosts()[Ghost::Type::POKEY]->getInitX());
    }
    else {
        int rx = (rand()/RAND_MAX) * m_map->getNbX();
        int ry = (rand()/RAND_MAX) * m_map->getNbY();
        shortestWay(Ghost::Type::POKEY, rx, ry);
    }
}

// Shortest way for a ghost to get to the position (x, y)
// returns 1 if the goal is achieved so we set another one
// returns 0 if the goal isn't achieved yet
int GameManager::shortestWay(int ghostType, float x, float y) {

    float gx = m_map->getGhosts()[ghostType]->getPosX();
    float gy = m_map->getGhosts()[ghostType]->getPosY();
    if ((gx != x) && (gy != y)) {

        // if goal is at the right top
        if ((gx - x < 0) && (gy - y < 0)) {
            if (!characterWallCollision(m_map->getGhosts()[ghostType], 'D')) m_map->getGhosts()[ghostType]->moveRight();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Z')) m_map->getGhosts()[ghostType]->moveUp();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Q')) m_map->getGhosts()[ghostType]->moveLeft();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'S')) m_map->getGhosts()[ghostType]->moveDown();
        }
        // if goal is at the right bottom
        else if ((gx - x < 0) && (gy - y > 0)) {
            if (!characterWallCollision(m_map->getGhosts()[ghostType], 'D')) m_map->getGhosts()[ghostType]->moveRight();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'S')) m_map->getGhosts()[ghostType]->moveDown();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Q')) m_map->getGhosts()[ghostType]->moveLeft();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Z')) m_map->getGhosts()[ghostType]->moveUp();
        }
        // if goal is at the left top
        else if ((gx - x > 0) && (gy - y < 0)) {
            if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Q')) m_map->getGhosts()[ghostType]->moveLeft();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Z')) m_map->getGhosts()[ghostType]->moveUp();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'D')) m_map->getGhosts()[ghostType]->moveRight();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'S')) m_map->getGhosts()[ghostType]->moveDown();
        }
        // if goal is at the left bottom
        else if ((gx - x > 0) && (gy - y > 0)) {
            if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Q')) m_map->getGhosts()[ghostType]->moveLeft();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'S')) m_map->getGhosts()[ghostType]->moveDown();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'D')) m_map->getGhosts()[ghostType]->moveRight();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Z')) m_map->getGhosts()[ghostType]->moveUp();
        }
        // if goal is at the right
        else if ((gx - x < 0) && (gy - y == 0)) {
            if (!characterWallCollision(m_map->getGhosts()[ghostType], 'D')) m_map->getGhosts()[ghostType]->moveRight();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Q')) m_map->getGhosts()[ghostType]->moveLeft();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Z')) m_map->getGhosts()[ghostType]->moveUp();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'S')) m_map->getGhosts()[ghostType]->moveDown();
        }
        // if goal is at the left
        else if ((gx - x > 0) && (gy - y == 0)) {
            if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Q')) m_map->getGhosts()[ghostType]->moveLeft();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'D')) m_map->getGhosts()[ghostType]->moveRight();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Z')) m_map->getGhosts()[ghostType]->moveUp();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'S')) m_map->getGhosts()[ghostType]->moveDown();
        }
        // if goal is at the top
        else if ((gx - x == 0) && (gy - y < 0)) {
            if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Z')) m_map->getGhosts()[ghostType]->moveUp();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'S')) m_map->getGhosts()[ghostType]->moveDown();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Q')) m_map->getGhosts()[ghostType]->moveLeft();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'D')) m_map->getGhosts()[ghostType]->moveRight();
        }
        // if goal is at the bottom
        else if ((gx - x == 0) && (gy - y > 0)) {
            if (!characterWallCollision(m_map->getGhosts()[ghostType], 'S')) m_map->getGhosts()[ghostType]->moveDown();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Z')) m_map->getGhosts()[ghostType]->moveUp();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'Q')) m_map->getGhosts()[ghostType]->moveLeft();
            else if (!characterWallCollision(m_map->getGhosts()[ghostType], 'D')) m_map->getGhosts()[ghostType]->moveRight();
        }
        return 0;
    }
    return 1;
}


/* From a starting position dx, dy
We calculate the shortest way to get to a goal from:
- dx+1, dy
- dx-1, dy
- dx, dy+1
- dx, dy-1

*/

int GameManager::countShortestWay(float dx, float dy, float ax, float ay) {
    
    return 0;
    /*
    if (dx == ax && dy == ay) return 0;
    else {
        std::vector<int> moves;
        if ((m_map->getStaticObjects()[dy][dx+1]->getType()!='W') && ((dx+1) >= 0 && (dx+1) < m_map->getNbX()) && (dx >= 0 && dx < m_map->getNbY())) {
            std::cout << "Added right " << dy << " , " << dx+1 << std::endl;
            moves.push_back(countShortestWay(dx+1, dy, ax, ay));
        }
        else moves.push_back(-1);
        if ((m_map->getStaticObjects()[dy][dx-1]->getType()!='W') && ((dx-1) >= 0 && (dx-1) < m_map->getNbX()) && (dx >= 0 && dx < m_map->getNbY())) {
            std::cout << "Added left" << std::endl;
            moves.push_back(countShortestWay(dx-1, dy, ax, ay));
        }
        else moves.push_back(-1);
        if ((m_map->getStaticObjects()[dy+1][dx]->getType()!='W') && ((dx) >= 0 && (dx) < m_map->getNbX()) && ((dy-1) >= 0 && (dy-1) < m_map->getNbY())) {
            std::cout << "Added down" << std::endl;
            moves.push_back(countShortestWay(dx, dy+1, ax, ay));
        }
        else moves.push_back(-1);
        if ((m_map->getStaticObjects()[dy-1][dx]->getType()!='W') && ((dx) >= 0 && (dx) < m_map->getNbX()) && ((dy-1) >= 0 && (dy-1) < m_map->getNbY())) {
            std::cout << "Added up" << std::endl;
            moves.push_back(countShortestWay(dx, dy-1, ax, ay));
        }
        else moves.push_back(-1);
        return 1+min(moves);
    }
    */
}

char GameManager::nextMove(float dx, float dy, float ax, float ay) {
    
    if (countShortestWay(dx, dy, ax, ay)) {
        std::cout << "Started : " << std::endl;
        std::vector<int> moves;
        if ((m_map->getStaticObjects()[dy][dx+1]->getType()!='W') && ((dx+1) >= 0 && (dx+1) < m_map->getNbX()) && (dx >= 0 && dx < m_map->getNbY())) {
            moves.push_back(countShortestWay(dx+1, dy, ax, ay));
        }
        else moves.push_back(-1);
        if ((m_map->getStaticObjects()[dy][dx-1]->getType()!='W') && ((dx-1) >= 0 && (dx-1) < m_map->getNbX()) && (dx >= 0 && dx < m_map->getNbY())) {
            moves.push_back(countShortestWay(dx-1, dy, ax, ay));
        }
        else moves.push_back(-1);
        if ((m_map->getStaticObjects()[dy+1][dx]->getType()!='W') && ((dx) >= 0 && (dx) < m_map->getNbX()) && ((dy-1) >= 0 && (dy-1) < m_map->getNbY())) {
            moves.push_back(countShortestWay(dx, dy+1, ax, ay));
        }
        else moves.push_back(-1);
        if ((m_map->getStaticObjects()[dy-1][dx]->getType()!='W') && ((dx) >= 0 && (dx) < m_map->getNbX()) && ((dy-1) >= 0 && (dy-1) < m_map->getNbY())) {
            moves.push_back(countShortestWay(dx, dy-1, ax, ay));
        }
        else moves.push_back(-1);
        int mini  = min(moves);
        switch(mini) {
            case 0: return 'D';
            case 1: return 'Q';
            case 2: return 'S';
            case 3: return 'Z';
        }
    }
    return 'N';
}

void GameManager::ghostMove() {
    shadowAI();
    speedyAI();
    bashfulAI();
    pokeyAI();
}