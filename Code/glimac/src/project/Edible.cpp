#include "../include/project/Edible.hpp"


Edible::Edible(int posX, int posY, float width, float height, int type) : StaticObject(posX, posY, width, height), m_type(type) { }
Edible::Edible() { }

int Edible::getType() const { return m_type; }

void Edible::setType(int type) { m_type = type;}
