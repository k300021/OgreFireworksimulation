#include "CharacterManager.h"
#include <omp.h>

//-----------------------------------------------

CharacterManager::CharacterManager( Ogre::SceneManager* sceneMgr, OgreDetourCrowd* detourCrowd, OgreDetourTileCache* detourTileCache )
{
    mSceneMgr           = sceneMgr;
    mDetourCrowd        = detourCrowd;
    mDetourTileCache    = detourTileCache;
    mCharacterCount     = 0;
    mCharacterList.clear();
}

//-----------------------------------------------

CharacterManager::~CharacterManager( void )
{
    mCharacterList.clear();
}

//-----------------------------------------------
	
void CharacterManager::createCharacter( Ogre::Vector3 position, Ogre::Vector3 destination, bool debugDraw )
{
    mCharacterList.push_back( new Character( mSceneMgr, mDetourCrowd, mDetourTileCache, "Chaeacter" + Ogre::StringConverter::toString( ++mCharacterCount ), position, destination, debugDraw ) );
}

//-----------------------------------------------

void CharacterManager::deleteCharacter( Ogre::String identity )
{
    for( std::vector< Character* >::iterator iter = mCharacterList.begin(); iter != mCharacterList.end(); iter++ )
        if ( ( *iter )->getIdentity() == identity )
        {
            ( *iter )->~Character();
            mCharacterList.erase( iter );
        }
}

//-----------------------------------------------

void CharacterManager::deleteAllCharacter( void )
{
    for( std::vector< Character* >::iterator iter = mCharacterList.begin(); iter != mCharacterList.end(); iter++ )
        ( *iter )->~Character();
    mCharacterList.clear();
}

//-----------------------------------------------
	
void CharacterManager::createExitPoint( Ogre::Vector3 position )
{
    Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "ExitPoint" + Ogre::StringConverter::toString( ++mExitPointCount ) );
    
    Ogre::Entity* mEntity = mSceneMgr->createEntity( "ExitPoint" + Ogre::StringConverter::toString( ++mExitPointCount ), "Cylinder.mesh" );
    mEntity->setMaterialName( "Cylinder/Yellow" );
    mNode->attachObject( mEntity );
    mNode->setPosition( position );
    mNode->translate( 0, -position.y, 0 );
    mNode->setScale( 50, 15, 50 );
    
    mExitPointList.push_back( mNode );
}

//-----------------------------------------------

void CharacterManager::deleteExitPoint( Ogre::String identity )
{
    for( std::vector< Ogre::SceneNode* >::iterator iter = mExitPointList.begin(); iter != mExitPointList.end(); iter++ )
        if ( ( *iter )->getName() == identity )
        {
            mSceneMgr->destroyEntity( mSceneMgr->getEntity( ( *iter )->getName() ) );
            mSceneMgr->destroySceneNode( ( *iter ) );
            mExitPointList.erase( iter );
        }
}

//-----------------------------------------------

void CharacterManager::deleteAllExitPoint( void )
{
    for( std::vector< Ogre::SceneNode* >::iterator iter = mExitPointList.begin(); iter != mExitPointList.end(); iter++ )
        mSceneMgr->destroySceneNode( ( *iter ) );
    mExitPointList.clear();
}

//-----------------------------------------------
		
void CharacterManager::update( bool disasterState, double timeSinceLastFrame )
{
    updatePosition( timeSinceLastFrame );
    
    std::vector< Character* >::iterator iter = mCharacterList.begin();
    while( iter != mCharacterList.end() ) 
    {
        if ( disasterState )
        {
            ( *iter )->mCheckTimeCount -= timeSinceLastFrame * 1000;
            if ( ( *iter )->mCheckTimeCount <= 0 )
                ( *iter )->mCheckDestination = true;
           
            if ( ( *iter )->mCheckDestination )
            {    
                int CountPath = -1;
                for ( std::vector< Ogre::SceneNode* >::iterator node = mExitPointList.begin(); node != mExitPointList.end(); node++ )
                {
                    float tempCountPath = mDetourTileCache->m_recast->CountPath( ( *iter )->getPosition(), ( *node )->getPosition(), 1, 1 );
                    if( CountPath == -1 || tempCountPath < CountPath )
                    {
                        ( *iter )->updateDestination( ( *node )->getPosition(), false );   
                        CountPath = tempCountPath;
                    }
                }
                ( *iter )->mCheckDestination = false;
                ( *iter )->mCheckDestination = ( int ) Ogre::Math::RangeRandom( 500, 5000 );
            }

            if ( ( *iter )->destinationReached() ) 
            {
                ( *iter )->~Character();
                iter = mCharacterList.erase( iter );
                continue;
            }           
        }
        else if ( ( *iter )->destinationReached() ) 
            ( *iter )->updateDestination( mDetourCrowd->m_recast->getRandomNavMeshPoint(), false );    
        iter++;
    }
}
//-----------------------------------------------
	
void CharacterManager::updatePosition( double timeSinceLastFrame )
{
    for( std::vector< Character* >::iterator iter = mCharacterList.begin(); iter != mCharacterList.end(); iter++ )
        ( *iter )->updatePosition( timeSinceLastFrame );
}

//-----------------------------------------------

void CharacterManager::updateDestination( Ogre::String identity, Ogre::Vector3 destination, bool updatePreviousPath )
{
    for( std::vector< Character* >::iterator iter = mCharacterList.begin(); iter != mCharacterList.end(); iter++ )
        if ( ( *iter )->getIdentity() == identity )
            ( *iter )->updateDestination( destination, updatePreviousPath );
}

//-----------------------------------------------
    
void CharacterManager::updateRandomDestination( bool updatePreviousPath )
{
    for( std::vector< Character* >::iterator iter = mCharacterList.begin(); iter != mCharacterList.end(); iter++ )
        ( *iter )->updateDestination( mDetourCrowd->m_recast->getRandomNavMeshPoint(), updatePreviousPath );
}

//-----------------------------------------------

void CharacterManager::updateAllDestination( Ogre::Vector3 destination, bool updatePreviousPath )
{
    for( std::vector< Character* >::iterator iter = mCharacterList.begin(); iter != mCharacterList.end(); iter++ )
        ( *iter )->updateDestination( destination, updatePreviousPath );
}

//-----------------------------------------------
/*
void CharacterManager::setEndPoint( std::vector< Obstacle* > mObstacleManager )
{
    for( std::vector< Obstacle* >::iterator iter = mObstacleManager.begin(); iter != mObstacleManager.end(); iter++ )
        if ( ( *iter )->getType() == -1 )
            mEndPoint.push_back( ( *iter )->getPosition() );
}

//-----------------------------------------------

void CharacterManager::clearEndPoint()
{
    mEndPoint.clear();
}
*/
//-----------------------------------------------

void CharacterManager::setDebugVisibility( bool visible )
{
    for( std::vector< Character* >::iterator iter = mCharacterList.begin(); iter != mCharacterList.end(); iter++ )
        ( *iter )->setDebugVisibility( visible );
}

//-----------------------------------------------
