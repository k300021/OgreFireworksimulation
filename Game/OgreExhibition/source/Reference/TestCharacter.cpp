#include "TestCharacter.h"

TestCharacter::TestCharacter(Ogre::String name, Ogre::SceneManager *sceneMgr, OgreDetourCrowd* detourCrowd, Ogre::Vector3 position)
    : Character(name, sceneMgr, detourCrowd, position)
{
    // Depict agent as blue cylinder
    mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name+"Node");
    mEnt = mSceneMgr->createEntity(name, "Cylinder.mesh");
    mEnt->setMaterialName("Cylinder/Blue");
    mNode->attachObject(mEnt);
    mNode->setPosition(position);
    mNode->translate( 0, -mDetourCrowd->m_recast->getCellHeight(),0 );
     
    // Set marker scale to size of agent
    mNode->setScale(detourCrowd->getAgentRadius() * 2, detourCrowd->getAgentHeight(), detourCrowd->getAgentRadius() * 2 );

    mEnt->setQueryFlags( DEFAULT_MASK );   // Exclude from ray queries
    createVelocityLine();
}

void TestCharacter::update(Ogre::Real timeSinceLastFrame)
{
    updatePosition(timeSinceLastFrame);
    updateVelocityLine();
}

void TestCharacter::setDebugVisibility(bool visible)
{
    return;
}

void TestCharacter::createVelocityLine()
{
    DynamicLines *lines = new DynamicLines( Ogre::RenderOperation::OT_LINE_LIST );
    lines->addPoint( Ogre::Vector3::ZERO );
    lines->addPoint( Ogre::Vector3::ZERO );
    lines->update();
    mLine = mSceneMgr->getRootSceneNode()->createChildSceneNode( mName + "Line" );
    mLine->attachObject( lines );
}

void TestCharacter::updateVelocityLine()
{
    DynamicLines* lines = dynamic_cast< DynamicLines* > ( mLine->getAttachedObject( 0 ) );
    lines->setPoint( 1, getVelocity() );
    lines->update();
    mLine->setPosition( getPosition() );
    mLine->translate( 0, 5,0 );
}
