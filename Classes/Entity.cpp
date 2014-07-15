#include "Entity.h"


Entity::Entity()
{
	m_sprite = NULL;
	m_armature = NULL;
}
Entity::~Entity()
{

}

Sprite* Entity::getSprite()
{
	return this->m_sprite;
}

void Entity::setSpriteAsNULL()
{
	this->m_sprite = NULL;
}

void Entity::bindSprite(Sprite* sprite)
{
	this->m_sprite = sprite;
	this->setContentSize(sprite->getContentSize());
	//log("sprite->getAnchorPoint() .... %f, %f", sprite->getAnchorPoint().x, sprite->getAnchorPoint().y);
	sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
	this->addChild(m_sprite);
}

void Entity::bindArmature(Armature* armature)
{
	this->m_armature = armature;
	this->addChild(m_armature);
}

Armature* Entity::getArmature()
{
	return this->m_armature;
}