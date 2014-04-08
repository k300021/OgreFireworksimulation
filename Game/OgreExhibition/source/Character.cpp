#include "Character.h"

//-----------------------------------------------

Character::Character( Ogre::SceneManager* sceneMgr, OgreDetourCrowd* detourCrowd, OgreDetourTileCache* detourTileCache, Ogre::String identity, Ogre::Vector3 position, Ogre::Vector3 destination, bool debugDraw ): 
    mSceneMgr( sceneMgr ),
    mDetourCrowd( detourCrowd ),
    mDetourTileCache( detourTileCache ),
    mIdentity( identity ),
    mPosition( position ),
    mDestination( destination ),
    mDebugDraw( debugDraw ),
    mCheckDestination( true ),
    mCheckTimeCount( 0 )
{
    mDebugEntity = mSceneMgr->createEntity( "Debug" + mIdentity + "Entity", "Cylinder.mesh" );
    mDebugEntity->setMaterialName( "Cylinder/Green" );
    mDebugEntity->setQueryFlags( DEBUG_MASK );
    mDebugEntity->setVisible( mDebugDraw );
    createVelocityLine();
    
    mDebugNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "Debug" + mIdentity + "Node" );
    mDebugNode->attachObject( mDebugEntity );
    mDebugNode->setScale( detourCrowd->getAgentRadius() * 2, detourCrowd->getAgentHeight(), detourCrowd->getAgentRadius() * 2 );
    mDebugNode->setPosition( mPosition );
    mDebugNode->translate( 0, -mDetourCrowd->m_recast->getCellHeight(), 0 );
    
    
    mEntity = mSceneMgr->createEntity( mIdentity + "Entity", "robot.mesh" );
    mEntity->setMaterialName( "Examples/Robot" );
    //mEntity = mSceneMgr->createEntity( mIdentity, "Gamechar-male.mesh" );
    //mEntity->setMaterialName( "GameChar_Male_Mat_" + Ogre::StringConverter::toString( ( int ) Ogre::Math::RangeRandom( 0, 14 ) ) );
    
    mEntity->setVisible( !mDebugDraw );
    mEntity->setQueryFlags( CHARACTER_MASK );
    
    Ogre::Vector3 BoundSize = mEntity->getBoundingBox().getSize();

    mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( mIdentity + "Node" );
    mNode->attachObject( mEntity );
    mNode->setScale( detourCrowd->getAgentRadius() / BoundSize.y * 4, detourCrowd->getAgentHeight() / BoundSize.y * 2, detourCrowd->getAgentRadius() / BoundSize.y * 4 );
    mNode->setPosition( mPosition );
    mNode->translate( 0, -mDetourCrowd->m_recast->getCellHeight(), 0 );
   
    mAnimState= mEntity->getAnimationState( "Walk" );
    mAnimState->setEnabled( true );
    mAnimState->setLoop( true );
    mAnimSpeedScale = 1.5f * detourCrowd->getAgentHeight();
    //mAnimSpeedScale = 2.2f * detourCrowd->getAgentHeight();

    mAgentID = mDetourCrowd->addAgent( mPosition );
    mAgent = mDetourCrowd->getAgent( mAgentID );
    updateDestination( mDestination, false );
}

//-----------------------------------------------

Character::~Character( void )
{
    mDetourCrowd->removeAgent( mAgentID );

    mSceneMgr->destroyEntity( mDebugEntity );
    mSceneMgr->destroySceneNode( mDebugNode );
    deleteVelocityLine();
    
    mSceneMgr->destroyEntity( mEntity );
    mSceneMgr->destroySceneNode( mNode );
    
}

//-----------------------------------------------

Ogre::String Character::getIdentity()
{
    return mIdentity;
}

//-----------------------------------------------

Ogre::Vector3 Character::getPosition()
{
    return mPosition;
}

//-----------------------------------------------

void Character::updateDestination( Ogre::Vector3 destination, bool updatePreviousPath )
{
   if( !mDetourCrowd->m_recast->findNearestPointOnNavmesh( destination, destination ) )
        return;

    mDestination = destination;
    mDetourCrowd->setMoveTarget( mAgentID, mDestination, updatePreviousPath );
}

//-----------------------------------------------

void Character::updatePosition( double timeSinceLastFrame )
{
    Ogre::Vector3 position;
    OgreRecast::FloatAToOgreVect3( mAgent->npos, position );
        
    mPosition = position;
    
    mDebugNode->setPosition( mPosition );
    mDebugNode->translate( 0, -mDetourCrowd->m_recast->getCellHeight(), 0 );
    updateVelocityLine();
    
    mNode->setPosition( mPosition );
    mNode->translate( 0, -mDetourCrowd->m_recast->getCellHeight(), 0 );

    Ogre::Vector3 velocity;
    OgreRecast::FloatAToOgreVect3( mAgent->vel, velocity );
    Ogre::Real speed = velocity.length();

    if( speed > 0.15 ) 
    {
        mAnimState->addTime( speed * timeSinceLastFrame / mAnimSpeedScale );
        
        Ogre::Vector3 src = mNode->getOrientation() * ( Ogre::Vector3::UNIT_X );
        //Ogre::Vector3 src = mNode->getOrientation() * - ( Ogre::Vector3::UNIT_Z );
        src.y = 0;
        velocity.y = 0;
        velocity.normalise();
        mNode->rotate( src.getRotationTo( velocity ) );
    }
}

//-----------------------------------------------

bool Character::destinationReached( void )
{
    return mPosition.squaredDistance( mDestination ) < 5000;
}

//-----------------------------------------------

void Character::createVelocityLine()
{
    DynamicLines *lines = new DynamicLines( Ogre::RenderOperation::OT_LINE_LIST );
    lines->addPoint( Ogre::Vector3::ZERO );
    lines->addPoint( Ogre::Vector3::ZERO );
    lines->update();
    mLine = mSceneMgr->getRootSceneNode()->createChildSceneNode( "Debug" + mIdentity + "Line" );
    mLine->attachObject( lines );
    mLine->setVisible( mDebugDraw );
}

//-----------------------------------------------

void Character::updateVelocityLine()
{
    DynamicLines* lines = dynamic_cast< DynamicLines* > ( mLine->getAttachedObject( 0 ) );
    Ogre::Vector3 velocity;
    OgreRecast::FloatAToOgreVect3( mAgent->vel, velocity );
    lines->setPoint( 1, velocity );
    lines->update();
    mLine->setPosition( mPosition );
    mLine->translate( 0, 5,0 );
}

//-----------------------------------------------

void Character::deleteVelocityLine()
{
    mSceneMgr->destroySceneNode( mLine );
}

//-----------------------------------------------

void Character::setDebugVisibility( bool visible )
{
    mDebugDraw = visible;
    mDebugEntity->setVisible( mDebugDraw );
    mLine->setVisible( mDebugDraw );

    mEntity->setVisible( !mDebugDraw );
}

//-----------------------------------------------
