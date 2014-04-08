#include "Obstacle.h"

//-----------------------------------------------

Obstacle::Obstacle( Ogre::SceneManager* sceneMgr, OgreDetourTileCache* detourTileCache, Ogre::Real offset, Ogre::String identity, ObstacleType type, Ogre::Vector3 position ) :
    mSceneMgr( sceneMgr ),
    mIdentity( identity ),
    mPosition( position ),
    mType( type ),
    mOffset( offset ),
    mDetourTileCache( detourTileCache )
{
    mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( mIdentity );
    
    switch ( mType )
    {
        case 0:
            mEntity = mSceneMgr->createEntity( mIdentity, "Box.mesh" );
            mNode->attachObject( mEntity );
            mNode->setPosition( mPosition );
            mNode->translate( 0, -mPosition.y, 0 );
            mNode->setScale( 25, 25, 25 );

        case 10:
			mEntity = mSceneMgr->createEntity( mIdentity, "Pot1.mesh" );
            mNode->attachObject( mEntity );
            mNode->setPosition( mPosition );
            mNode->translate( 0, -mPosition.y, 0 );
            mNode->setScale( 5, 5, 5 );
            break;
        case 11:
            mEntity = mSceneMgr->createEntity( mIdentity, "Pot2.mesh" );
            mNode->attachObject( mEntity );
            mNode->setPosition( mPosition );
            mNode->translate( 0, -mPosition.y, 0 );
            mNode->setScale( exhiScale, exhiScale, exhiScale );
            break;

		case 20:
            mEntity = mSceneMgr->createEntity( mIdentity, "Table1.mesh" );
            mNode->attachObject( mEntity );
            mNode->setPosition( mPosition );
            mNode->translate( 0, -mPosition.y, 0 );
            mNode->setScale( exhiScale, exhiScale, exhiScale );
            break;
        case 21:
            mEntity = mSceneMgr->createEntity( mIdentity, "Table2.mesh" );
            mNode->attachObject( mEntity );
            mNode->setPosition( mPosition );
            mNode->translate( 0, -mPosition.y, 0 );
            mNode->setScale( exhiScale, exhiScale, exhiScale );
            break;

        case 30:
            mEntity = mSceneMgr->createEntity( mIdentity, "Chair1.mesh" );
            mNode->attachObject( mEntity );
            mNode->setPosition( mPosition );
            mNode->translate( 0, -mPosition.y, 0 );
            mNode->setScale( exhiScale, exhiScale, exhiScale );
            break;
		
        default:
            mEntity = mSceneMgr->createEntity( mIdentity, "Box.mesh" );
            mNode->attachObject( mEntity );
            mNode->setPosition( mPosition );
            mNode->translate( 0, -mPosition.y, 0 );
            mNode->setScale( 25, 25, 25 );
    }
    
    mEntity->setQueryFlags( OBSTACLE_MASK );
    mEntity->setCastShadows( true );
    
    mInputGeom = new InputGeom( mEntity );
    /*
    Ogre::AxisAlignedBox mBoundingBox = mEntity->getBoundingBox();
    Ogre::Matrix4 transform = mSceneMgr->getRootSceneNode()->_getFullTransform().inverse() * mEntity->getParentSceneNode()->_getFullTransform();
    mBoundingBox.transform(transform);
    mConvexHull = new ConvexVolume( mBoundingBox, mOffset );
    */
    mConvexHull = mInputGeom->getConvexHull( mOffset );

    mConvexHull->area = RC_NULL_AREA;
    
    mDetourTileCache->addConvexShapeObstacle( mConvexHull );
    
#ifdef _DEBUG
    mConvexHullDebug = InputGeom::drawConvexVolume( mConvexHull, mSceneMgr );
#endif
}

//-----------------------------------------------

Obstacle::~Obstacle( void )
{
    mDetourTileCache->removeConvexShapeObstacle( mConvexHull );
    
    mNode->removeAllChildren();
    mNode->getParentSceneNode()->removeChild( mNode );
    mSceneMgr->destroyEntity( mEntity );
    mSceneMgr->destroySceneNode( mNode );
    
    delete mInputGeom;
    
#ifdef _DEBUG
    mConvexHullDebug->detachFromParent();
    mSceneMgr->destroyManualObject( mConvexHullDebug );
    
    mConvexHullDebug = NULL;
#endif
}

//-----------------------------------------------

Ogre::Entity* Obstacle::getEntity( void )
{
    return mEntity;
}

//-----------------------------------------------

Ogre::String Obstacle::getIdentity( void )
{
    return mIdentity;
}

//-----------------------------------------------

ObstacleType Obstacle::getType( void )
{
    return mType;
}

//-----------------------------------------------

Ogre::Vector3 Obstacle::getPosition( void )
{
    return mPosition;
}

//-----------------------------------------------

Ogre::Quaternion Obstacle::getOrientation( void )
{
    return mOrientation;
}

//-----------------------------------------------

void Obstacle::updatePosition( Ogre::Vector3 position )
{
    mDetourTileCache->removeConvexShapeObstacle( mConvexHull );
    mConvexHull->move( position - mPosition );
    std::cout << Ogre::StringConverter::toString( position - mPosition ) << std::endl;
            
    mDetourTileCache->addConvexShapeObstacle( mConvexHull );

    mPosition = position;
    mNode->setPosition( mPosition );
    mNode->translate( 0, -mPosition.y, 0 );

#ifdef _DEBUG    
    mConvexHullDebug->detachFromParent();
    mSceneMgr->destroyManualObject( mConvexHullDebug );
    mConvexHullDebug = InputGeom::drawConvexVolume( mConvexHull, mSceneMgr );
#endif
}

//-----------------------------------------------

void Obstacle::updateOrientation( Ogre::Quaternion orientation )
{
    if ( !mOrientation.equals( orientation, Ogre::Degree( 10 ) ) )
    {
        mDetourTileCache->removeConvexShapeObstacle( mConvexHull );
        
        delete mConvexHull;
        
        Ogre::Quaternion relativeOrientation = orientation * mOrientation.Inverse();
        orientation.normalise();
        mInputGeom->applyOrientation( relativeOrientation, mPosition );
        mConvexHull = mInputGeom->getConvexHull( mOffset );
        mConvexHull->area = RC_NULL_AREA;
        mDetourTileCache->addConvexShapeObstacle( mConvexHull );
        
        mOrientation = orientation;
        mNode->setOrientation( orientation );
        
#ifdef _DEBUG
        mConvexHullDebug->detachFromParent();
        mSceneMgr->destroyManualObject( mConvexHullDebug );
        mConvexHullDebug = InputGeom::drawConvexVolume( mConvexHull, mSceneMgr );
#endif
    }
}
    
//-----------------------------------------------
