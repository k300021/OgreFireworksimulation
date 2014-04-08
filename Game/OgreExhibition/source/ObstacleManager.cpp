#include "ObstacleManager.h"

//-----------------------------------------------

ObstacleManager::ObstacleManager( Ogre::SceneManager* sceneMgr, Ogre::Real offset, OgreDetourTileCache* detourTileCache )
{
    mSceneMgr           = sceneMgr;
    mOffset             = offset;
    mDetourTileCache    = detourTileCache;
    mObstacleCount      = 0;
    mObstacleList.clear();
}

//-----------------------------------------------

ObstacleManager::~ObstacleManager( void )
{
    mObstacleList.clear();    
}

//-----------------------------------------------

void ObstacleManager::createObstacle( Ogre::Vector3 position, ObstacleType type )
{
	mObstacleList.push_back( new Obstacle( mSceneMgr, mDetourTileCache, mOffset, 
		"Obstacle" + Ogre::StringConverter::toString( ++mObstacleCount ), type, position ) );
}

//-----------------------------------------------

void ObstacleManager::deleteObstacle( Ogre::String identity )
{
    for( std::vector< Obstacle* >::iterator iter = mObstacleList.begin(); iter != mObstacleList.end(); iter++ )
        if ( ( *iter )->getIdentity() == identity )
        {
            ( *iter )->~Obstacle();
            mObstacleList.erase( iter );
			break;
        }
}

//-----------------------------------------------

void ObstacleManager::rotateObstacle( Ogre::String identity, Ogre::Quaternion orientation )
{
	Ogre::Entity* ent;
	for( std::vector< Obstacle* >::iterator iter = mObstacleList.begin(); iter != mObstacleList.end(); iter++ )
        if ( ( *iter )->getIdentity() == identity )
			ent = ( *iter )->getEntity();
	ent->getParentNode()->yaw( Ogre::Degree(30.0f) );
	updateOrientation( identity, orientation );
}

//-----------------------------------------------

void ObstacleManager::updatePosition( Ogre::String identity, Ogre::Vector3 position )
{
	for( std::vector< Obstacle* >::iterator iter = mObstacleList.begin(); iter != mObstacleList.end(); iter++ )
        if ( ( *iter )->getIdentity() == identity )
            ( *iter )->updatePosition( position );
}

//-----------------------------------------------

void ObstacleManager::updateOrientation( Ogre::String identity, Ogre::Quaternion orientation )
{
    for( std::vector< Obstacle* >::iterator iter = mObstacleList.begin(); iter != mObstacleList.end(); iter++ )
        if ( ( *iter )->getIdentity() == identity )
            ( *iter )->updateOrientation( orientation );
}

//-----------------------------------------------

void ObstacleManager::saveObstaclesToFile( void )
{
	FILE * pFile;
	pFile = fopen ("saveFile","w");
	if (pFile!=NULL)
	{
		fprintf ( pFile, "FileName_test\n" );
		fprintf ( pFile, "%d\n", mObstacleList.size() );
		for(unsigned int i = 0;i < mObstacleList.size();i ++)
		{
			fprintf ( pFile, mObstacleList[i]->getIdentity().c_str() );
			fprintf ( pFile, "\n%d\n", mObstacleList[i]->getType() );
			fprintf ( pFile, "%f %f %f %f\n", mObstacleList[i]->getOrientation().w, 
				mObstacleList[i]->getOrientation().x, mObstacleList[i]->getOrientation().y, mObstacleList[i]->getOrientation().z );
			fprintf ( pFile, "%f %f %f\n", mObstacleList[i]->getPosition().x, 
				mObstacleList[i]->getPosition().y, mObstacleList[i]->getPosition().z );
		}
	}
	fclose ( pFile );
}

//-----------------------------------------------

void ObstacleManager::clearObstacles( void )
{
	unsigned int size = mObstacleList.size();
	for( unsigned int i = 0;i < size;i ++ )
		deleteObstacle( mObstacleList[0]->getIdentity() );
}

//-----------------------------------------------

bool ObstacleManager::loadObstaclesFromFile( void )
{
	FILE * pFile;
	pFile = fopen ("saveFile","r");
	if (pFile!=NULL)
	{
        clearObstacles();

		char				str[256];
		unsigned int		size;
		ObstacleType		type;
		Ogre::Quaternion	orientation;
		Ogre::Vector3		position;

		fscanf( pFile, "%s", str );
		std::cout<<"load file : "<< str << std::endl;
		fscanf( pFile, "%d", &size );
		std::cout<<"exhibition size : "<< size << std::endl;
		for( unsigned int i = 0;i < size;i ++)
		{
			fscanf( pFile, "%s\n", str );		//ignore name
			fscanf( pFile, "%d\n", &type );
			fscanf( pFile, "%f %f %f %f\n", &orientation.w, &orientation.x, &orientation.y, &orientation.z );
			fscanf( pFile, "%f %f %f\n", &position.x, &position.y, &position.z );

			std::cout << "name        : " << str << std::endl;
			std::cout << "type        : " << type << std::endl;
			std::cout << "qrientation : " << orientation.w << ", " << orientation.x << ", " << orientation.y << ", " << orientation.z << std::endl;
			std::cout << "position    : "<< position.x << ", " << position.y << ", " << position.z << std::endl;
			std::cout << std::endl;

			createObstacle( position, type );
			updateOrientation( mObstacleList[ mObstacleList.size()-1 ]->getIdentity(), orientation );
		}
		fclose ( pFile );
		return 1;
	}
	else
		return 0;
}

//-----------------------------------------------

void ObstacleManager::showObstacleDetail( void )
{
	/*
    for( unsigned int i = 0;i < mObstacleList.size();i ++ )
	{
		std::cout << "Obstacle name   : " << mObstacleList[i]->getIdentity() << std::endl;
		std::cout << "type            : " << mObstacleList[i]->getType() << std::endl;
		std::cout << "qrientation     : " << mObstacleList[i]->getOrientation().w << ", " << mObstacleList[i]->getOrientation().x << ", " << mObstacleList[i]->getOrientation().y << ", " << mObstacleList[i]->getOrientation().z << std::endl;
		std::cout << "position        : " << mObstacleList[i]->getPosition().x << ", " << mObstacleList[i]->getPosition().z << std::endl;
		std::cout << std::endl;
	}
    */
}