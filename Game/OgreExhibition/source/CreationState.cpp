#include "CreationState.h"

#define HUMAN_CHARACTERS false

//-----------------------------------------------

CreationState::CreationState()
{
    mQuit               = false;
    mFrameEvent         = Ogre::FrameEvent();
    mLMouseDown         = false;
    mRMouseDown         = false;
    mMoveScale          = 0.0f;
    mTranslateVector    = Ogre::Vector3::ZERO;
    mRecast             = NULL;
    mDetourTileCache    = NULL;
	mDebugDraw			= true;
    mNavMeshNode        = NULL;
    mDebugDraw          = false;
	mCurrentObject		= NULL;
    mWall               = false;
}

//-----------------------------------------------

void CreationState::enter()
{
    mOgre->mLog->logMessage( "Entering CreationState..." );
    
    createSceneManager();
    createCamera();
    setViewport();
    
	createScene();

	buildGUI();
}

//-----------------------------------------------

void CreationState::createSceneManager( void )
{
    mSceneMgr = mOgre->mRoot->createSceneManager( Ogre::ST_EXTERIOR_CLOSE, "ExhibitionSceneMgr" );
    mSceneMgr->setAmbientLight( Ogre::ColourValue( 0.7f, 0.7f, 0.7f ) );
    mOgre->mSceneMgr = mSceneMgr;
}

//-----------------------------------------------

void CreationState::createCamera( void )
{
    if ( mSceneMgr )
    {
        mCamera = mSceneMgr->createCamera( "ExhibitionCam" );
        mCamera->setPosition( Ogre::Vector3( 0, 300, 750 ) );
        mCamera->lookAt( Ogre::Vector3( 750, 0, 750 ) );
        mCamera->setNearClipDistance( 0.05f );
        mOgre->mCamera = mCamera;
    }
}

//-----------------------------------------------

void CreationState::setViewport( void )
{
    mOgre->mViewport->setCamera( mCamera );
    mOgre->mViewport->setBackgroundColour( Ogre::ColourValue( 0.0f, 0.0f, 0.0f, 1.0f ) );
    
    mCamera->setAspectRatio( Ogre::Real( mOgre->mViewport->getActualWidth() ) / Ogre::Real( mOgre->mViewport->getActualHeight() ) );
}

//-----------------------------------------------

void CreationState::buildGUI( void )
{
    mOgre->mTrayMgr->destroyAllWidgets();
    //mOgre->mTrayMgr->showFrameStats( OgreBites::TL_BOTTOMLEFT );
    //mOgre->mTrayMgr->showLogo( OgreBites::TL_BOTTOMRIGHT );
    mOgre->mTrayMgr->showCursor();
	
	Ogre::StringVector obstacleMenuList;
	obstacleMenuList.push_back( "Pot" );
	obstacleMenuList.push_back( "Box" );
	obstacleMenuList.push_back( "Exhibition1" );
	obstacleMenuList.push_back( "Exhibition2" );
	obstacleMenuList.push_back( "Exhibition3" );
	obstacleMenuList.push_back( "Exhibition4" );
	obstacleMenuList.push_back( "Exhibition5" );
	obstacleMenuList.push_back( "Exhibition6" );
	obstacleMenuList.push_back( "Exhibition7" );
	obstacleMenuList.push_back( "Exhibition8" );
	obstacleMenuList.push_back( "Exhibition9" );
	obstacleMenuList.push_back( "Exhibition10" );
	obstacleMenuList.push_back( "Exhibition11" );
	obstacleMenuList.push_back( "Exhibition12" );
	obstacleMenuList.push_back( "Exhibition13" );

	mOgre->mTrayMgr->createThickSelectMenu( OgreBites::TL_TOPRIGHT, "obstacleMenu", "obstacle", 200, 5, obstacleMenuList );

	mOgre->mTrayMgr->createSeparator( OgreBites::TL_TOPRIGHT, "seperator1", 200.0f );

	debugDrawBox = mOgre->mTrayMgr->createCheckBox( OgreBites::TL_TOPRIGHT, "debugDrawBox", "Tile (v)", 200 );
	logoBox = mOgre->mTrayMgr->createCheckBox( OgreBites::TL_TOPRIGHT, "logoBox", "Logo", 200 );
	frameStatesBox = mOgre->mTrayMgr->createCheckBox( OgreBites::TL_TOPRIGHT, "frameStatesBox", "frameStates", 200 );
	
	debugDrawBox->setChecked( true );
	logoBox->setChecked( false );
	frameStatesBox->setChecked( false );
	
	mOgre->mTrayMgr->createSeparator( OgreBites::TL_TOPRIGHT, "seperator2", 200.0f );

    mOgre->mTrayMgr->createLabel( OgreBites::TL_TOP, "MenuLbl", "SimulationState-Creation mode", 300 );
	
	mOgre->mTrayMgr->createButton( OgreBites::TL_TOPRIGHT, "SaveBtn", "save", 200 );
	mOgre->mTrayMgr->createButton( OgreBites::TL_TOPRIGHT, "ClearBtn", "clear", 200 );
	mOgre->mTrayMgr->createButton( OgreBites::TL_TOPRIGHT, "LoadBtn", "load", 200 );

	detail_btn = mOgre->mTrayMgr->createButton( OgreBites::TL_TOPRIGHT, "DetailBtn", "Detail", 200 );
	delete_btn = mOgre->mTrayMgr->createButton( OgreBites::TL_NONE, "DeleteBtn", "Delete(Del)", 150 );
	rotate_btn = mOgre->mTrayMgr->createButton( OgreBites::TL_NONE, "RotateBtn", "Rotate", 150 );
	showDynamicBotton( false );
}

//-----------------------------------------------

void CreationState::createScene()
{
    mSceneMgr->setAmbientLight( Ogre::ColourValue( 0.5, 0.5, 0.5 ) );
    mSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_ADDITIVE );
	
    Ogre::Plane plane( Ogre::Vector3::UNIT_Y, 0 );
    
    Ogre::MeshManager::getSingleton().createPlane( "ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 150, 150, true, 1, 30, 30, Ogre::Vector3::UNIT_Z );
    
    Ogre::Entity* entGround = mSceneMgr->createEntity( "GroundEntity", "ground" );
    mSceneMgr->getRootSceneNode()->createChildSceneNode( Ogre::Vector3( 750, 0, 750 ) )->attachObject( entGround );
    entGround->setMaterialName( "Dirt" );
    entGround->setCastShadows( false );
	entGround->setQueryFlags( MAP_MASK );

    mNavmeshEnts.push_back( entGround );  // Add obstacle
    
    //---------------------------------------------------------------------------
    //add entity
    //Ogre::Entity* ent = mSceneMgr->createEntity( "End", "Pot.mesh" );
    //Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "End1Node", Ogre::Vector3( 100, 0, 100 ) );
    //mNode->setScale( 5, 5, 5 );
    //mNode->attachObject( ent );
    //ent->setCastShadows( true );
	
    //mNavmeshEnts.push_back( ent );  // Add obstacle
    
    //----------------------------------------------------------------------
        
    OgreRecastConfigParams recastParams = OgreRecastConfigParams();
    recastParams.setCellSize( 10 );
    recastParams.setCellHeight( 10 );
    recastParams.setAgentMaxSlope( 45 );
    recastParams.setAgentHeight( 40 );
    recastParams.setAgentMaxClimb( 15 );
    recastParams.setAgentRadius( 10 );
    recastParams.setEdgeMaxLen( 2 );
    recastParams.setEdgeMaxError( 1.3f );
    recastParams.setRegionMinSize( 50 );
    recastParams.setRegionMergeSize( 20 );
    recastParams.setDetailSampleDist( 5 );
    recastParams.setDetailSampleMaxError( 5 );

    mRecast = new OgreRecast( mSceneMgr, recastParams );    // Use default configuration
    
    mDetourTileCache = new OgreDetourTileCache( mRecast );
    mDetourTileCache->TileCacheBuild( mNavmeshEnts );
    mDetourTileCache->drawNavMesh();
    
    mNavMeshNode = (Ogre::SceneNode*)mSceneMgr->getRootSceneNode()->getChild("RecastSN");
    mNavMeshNode->setVisible( true );
    
    mObstacleMgr = new ObstacleManager( mSceneMgr, recastParams.getAgentRadius(), mDetourTileCache );
    
    //----------------------------------------------------------------------
    
    Ogre::Light* pointLight = mSceneMgr->createLight( "pointLight" );
    pointLight->setType( Ogre::Light::LT_POINT );
    pointLight->setPosition( Ogre::Vector3( 750, 5000, 750 ) );
    
    pointLight->setDiffuseColour( 1.0, 1.0, 1.0 );
    pointLight->setSpecularColour( 1.0, 1.0, 1.0 );
}

//-----------------------------------------------

void CreationState::exit( void )
{
    mOgre->mLog->logMessage( "Leaving CreationState..." );

    if ( mSceneMgr )
    {
        mSceneMgr->destroyCamera( mCamera );
        mOgre->mRoot->destroySceneManager( mSceneMgr );
    }
    
    mNavmeshEnts.clear();
    
    mOgre->mTrayMgr->clearAllTrays();
    mOgre->mTrayMgr->destroyAllWidgets();
    mOgre->mTrayMgr->setListener( 0 );
}

//-----------------------------------------------

bool CreationState::pause( void )
{
    mOgre->mLog->logMessage( "Pausing CreationState..." );
    
    return true;
}

//-----------------------------------------------

void CreationState::resume()
{
    mOgre->mLog->logMessage( "Resuming CreationState..." );
    
    setViewport();

    buildGUI();
    
    mQuit = false;
}

//-----------------------------------------------

void CreationState::moveCamera()
{
    mCamera->moveRelative( mTranslateVector / 10 );
}

//-----------------------------------------------

void CreationState::getInput()
{
    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_A ) )
        mTranslateVector.x = -mMoveScale;
        
    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_D ) )
        mTranslateVector.x = mMoveScale;
        
    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_W ) )
        mTranslateVector.z = -mMoveScale;
        
    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_S ) )
        mTranslateVector.z = mMoveScale;
}

//-----------------------------------------------

bool CreationState::keyPressed( const OIS::KeyEvent &keyEventRef )
{
    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_ESCAPE ) )
    {
        pushAppState( findByName( "PauseState" ) );
        return true;
    }
    //create obstacle
    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_SPACE ) )
    {
        Ogre::Vector3 rayHitPoint;
        if ( queryCursorPosition( rayHitPoint ) )
        	mObstacleMgr->createObstacle( rayHitPoint, mObstacleType );
		setDebugVisibility( mDebugDraw );
    }

    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_Z ) )
	{
        mWall = !mWall;
	}

    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_V ) )
	{
        setDebugVisibility( !mDebugDraw );
		debugDrawBox->setChecked( !debugDrawBox->isChecked() );
	}
    
    mOgre->keyPressed( keyEventRef );
    
    return true;
}

//-----------------------------------------------

bool CreationState::keyReleased( const OIS::KeyEvent &keyEventRef )
{
    mOgre->keyReleased( keyEventRef );
    return true;
}

//-----------------------------------------------

bool CreationState::mouseMoved(const OIS::MouseEvent &evt)
{
    if ( mOgre->mTrayMgr->injectMouseMove( evt ) ) 
        return true;

    if ( mRMouseDown )
    {
        mCamera->yaw( Ogre::Degree( evt.state.X.rel * -0.1f ) );
        mCamera->pitch( Ogre::Degree( evt.state.Y.rel * -0.1f ) );
    }
    if( mWall )
    {
        Ogre::Vector3 rayHitPoint;
        if ( queryCursorPosition( rayHitPoint ) )
        	mObstacleMgr->createObstacle( rayHitPoint, BOX );
		setDebugVisibility( mDebugDraw );
    }

    return true;
}

//-----------------------------------------------

bool CreationState::mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
    if ( mOgre->mTrayMgr->injectMouseDown( evt, id ) ) 
        return true;

    if ( id == OIS::MB_Left )
        mLMouseDown = true;
    else if ( id == OIS::MB_Right )
        mRMouseDown = true;
    
    //select obstacle
    if ( mLMouseDown )
    {
        if( mCurrentObject )
            mCurrentObject->showBoundingBox( false );
        
        Ogre::Vector3 rayHitPoint;
        Ogre::MovableObject* rayHitObject;

        //select obstacle
        if ( queryCursorPosition( rayHitPoint, OBSTACLE_MASK, false, &rayHitObject ) )
        {
            mCurrentObject = rayHitObject->getParentSceneNode();
            mCurrentObject->showBoundingBox( true );
            mCurrentIdentity = rayHitObject->getName();
            
            showDynamicBotton( true );
        }
        //move obstacle
        else if ( mCurrentObject && queryCursorPosition( rayHitPoint ) )
        {
            mObstacleMgr->updatePosition( mCurrentIdentity, rayHitPoint );
			setDebugVisibility( mDebugDraw );
            mCurrentObject->showBoundingBox( false );
			mCurrentObject = NULL;
            
            showDynamicBotton( false );
        }
    }

    return true;
}

//-----------------------------------------------

bool CreationState::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
    if ( mOgre->mTrayMgr->injectMouseUp( evt, id ) ) 
        return true;
    
    if ( id == OIS::MB_Left )
        mLMouseDown = false;
    else if ( id == OIS::MB_Right )
        mRMouseDown = false;
  
    return true;
}

//-----------------------------------------------

void CreationState::update( double timeSinceLastFrame )
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    mOgre->mTrayMgr->frameRenderingQueued( mFrameEvent );

    mDetourTileCache->handleUpdate( timeSinceLastFrame );
    
    mMoveScale = 2000  * timeSinceLastFrame;
    mTranslateVector = Ogre::Vector3::ZERO;
    
    getInput();
    moveCamera();
    
    if(mQuit == true)
    {
        shutdown();
    }
}

//-----------------------------------------------

void CreationState::buttonHit(OgreBites::Button *button)
{
	if( button->getName() == "SaveBtn" )
	{
        mOgre->mLog->logMessage( "press SaveBtn" );
		mObstacleMgr->saveObstaclesToFile();
	}
	else if( button->getName() == "ClearBtn" )
	{
        mOgre->mLog->logMessage( "press ClearBtn" );
		mObstacleMgr->clearObstacles();
		mCurrentObject = 0;	//for dynamic button
		setDebugVisibility( mDebugDraw );
	}
	else if( button->getName() == "LoadBtn" )
	{
        mOgre->mLog->logMessage( "press LoadBtn" );
		if( !mObstacleMgr->loadObstaclesFromFile() )
			mOgre->mTrayMgr->showOkDialog( "error", "No any saved file" );
	}
	else if( button->getName() == "DetailBtn" )
	{
        mOgre->mLog->logMessage( "press DetailBtn" );
		mObstacleMgr->showObstacleDetail();
	}
	else if( button->getName() == "DeleteBtn" )
	{
        mOgre->mLog->logMessage( "press DeleteBtn" );
		mObstacleMgr->deleteObstacle( mCurrentIdentity );
        mCurrentObject = NULL;
		showDynamicBotton( false );
	}
	else if( button->getName() == "RotateBtn" )
	{
        mOgre->mLog->logMessage( "press RotateBtn" );
		mObstacleMgr->rotateObstacle( mCurrentIdentity, mCurrentObject->getOrientation() );
	}
}

//-----------------------------------------------

void CreationState::checkBoxToggled( OgreBites::CheckBox* box )
{
	if( box->getName() == "debugDrawBox" )
		setDebugVisibility( box->isChecked() ? true : false );
	else if( box->getName() == "logoBox" )
	{
		if( logoBox->isChecked() )
			mOgre->mTrayMgr->showLogo( OgreBites::TL_BOTTOMRIGHT );
		else
			mOgre->mTrayMgr->hideLogo();
	}
	else if( box->getName() == "frameStatesBox" )
	{
		if( frameStatesBox->isChecked() )
			mOgre->mTrayMgr->showFrameStats( OgreBites::TL_BOTTOMLEFT );
		else
			mOgre->mTrayMgr->hideFrameStats();
	}

}

//-----------------------------------------------

void CreationState::itemSelected( OgreBites::SelectMenu* menu )
{
	if( menu->getName() == "obstacleMenu" )
		switch( menu->getSelectionIndex() )
		{
			case 0:
				mObstacleType =  POT1;
				break;
            case 15:
				mObstacleType =  POT2;
				break;
			case 1:
				mObstacleType =  BOX;
				break;
			case 2:
				mObstacleType =  TABLE1;
				break;
			case 3:
				mObstacleType =  CHAIR1;
				break;
		}
}

//-----------------------------------------------

bool CreationState::queryCursorPosition(Ogre::Vector3 &rayHitPoint, unsigned long queryflags, bool clipToNavmesh, Ogre::MovableObject **rayHitObject)
{
    Ogre::Ray mouseRay = mOgre->mTrayMgr->getCursorRay( mCamera );
    
    Ogre::MovableObject *hitObject;
    if ( rayQueryPointInScene( mouseRay, queryflags, rayHitPoint, &hitObject ) ) 
    {
        if ( clipToNavmesh ) 
            mRecast->findNearestPointOnNavmesh( rayHitPoint, rayHitPoint );
        
        if ( rayHitObject )
            *rayHitObject = hitObject;
        
        return true;
    }
    return false;
}

//-----------------------------------------------

bool CreationState::rayQueryPointInScene( Ogre::Ray ray, unsigned long queryMask, Ogre::Vector3 &result, Ogre::MovableObject **foundMovable )
{
    mRayScnQuery = mSceneMgr->createRayQuery( ray, queryMask );
    
    //mRayScnQuery->setRay( ray );
    mRayScnQuery->setSortByDistance( true );
    
    Ogre::RaySceneQueryResult& query_result = mRayScnQuery->execute();
    
    Ogre::Real closest_distance = -1.0f;
    Ogre::Vector3 closest_result;
    Ogre::MovableObject *closest_movable;
    for ( size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++ )
    {
        if( ( closest_distance >= 0.0f) && ( closest_distance < query_result[ qr_idx ].distance ) )
            break;
        
        if ( ( query_result[ qr_idx ].movable != NULL ) && ( ( query_result[ qr_idx ].movable->getMovableType().compare( "Entity" ) == 0 ) || query_result[ qr_idx ].movable->getMovableType().compare( "ManualObject" ) == 0 ) )
        {
            size_t vertex_count;
            size_t index_count;
            Ogre::Vector3 *vertices;
            unsigned long *indices;
            
            if ( query_result[ qr_idx ].movable->getMovableType().compare( "Entity" ) == 0 )
            {
                Ogre::Entity *pentity = static_cast< Ogre::Entity* > ( query_result[ qr_idx ].movable );
                
                InputGeom::getMeshInformation( pentity->getMesh(), vertex_count, vertices, index_count, indices,
                                              pentity->getParentNode()->_getDerivedPosition(),
                                              pentity->getParentNode()->_getDerivedOrientation(),
                                              pentity->getParentNode()->_getDerivedScale() );
            }
            else
            {
                Ogre::ManualObject *pmanual = static_cast< Ogre::ManualObject* >( query_result[ qr_idx ].movable );
                
                InputGeom::getManualMeshInformation( pmanual, vertex_count, vertices, index_count, indices,
                                                    pmanual->getParentNode()->_getDerivedPosition(),
                                                    pmanual->getParentNode()->_getDerivedOrientation(),
                                                    pmanual->getParentNode()->_getDerivedScale() );
            }
            
            bool new_closest_found = false;
            for (int i = 0; i < static_cast< int > ( index_count ); i += 3 )
            {
                // check for a hit against this triangle
                std::pair< bool, Ogre::Real > hit = Ogre::Math::intersects( ray, vertices[ indices[ i ] ],
                                                                           vertices[ indices[ i + 1 ] ], vertices[ indices[ i + 2 ] ], true, false );
                
                // if it was a hit check if its the closest
                if ( hit.first )
                    if ( ( closest_distance < 0.0f ) || ( hit.second < closest_distance ) )
                    {
                        closest_distance = hit.second;
                        new_closest_found = true;
                    }
            }
            
            
            delete[] vertices;
            delete[] indices;
            
            if ( new_closest_found )
            {
                closest_result = ray.getPoint( closest_distance );
                if( query_result[ qr_idx ].movable != NULL )
                    closest_movable = query_result[ qr_idx ].movable;
            }
        }
    }
    
    if ( closest_distance >= 0.0f )
    {
        result = closest_result;
        *foundMovable = closest_movable;
        return true;
    }
    else
        return false;
}

//-----------------------------------------------

void CreationState::showDynamicBotton( bool whether_show )
{
	if( whether_show )
	{
		delete_btn->show();
		rotate_btn->show();
		mOgre->mTrayMgr->moveWidgetToTray( delete_btn, OgreBites::TL_BOTTOM, 0 );
		mOgre->mTrayMgr->moveWidgetToTray( rotate_btn, OgreBites::TL_BOTTOM, 0 );
	}
	else
	{
		delete_btn->hide();
		rotate_btn->hide();
		mOgre->mTrayMgr->moveWidgetToTray( delete_btn, OgreBites::TL_NONE, 0 );
		mOgre->mTrayMgr->moveWidgetToTray( rotate_btn, OgreBites::TL_NONE, 0 );
	}
}

//-----------------------------------------------

void CreationState::setDebugVisibility( bool visible )
{
    mDebugDraw = visible;
    mNavMeshNode->setVisible( visible );
}