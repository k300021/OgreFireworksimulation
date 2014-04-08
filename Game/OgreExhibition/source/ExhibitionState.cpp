#include "ExhibitionState.h"
#include "stdlib.h"
#define HUMAN_CHARACTERS false

//-----------------------------------------------

ExhibitionState::ExhibitionState()
{
    mQuit               = false;
    mFrameEvent         = Ogre::FrameEvent();
    mLMouseDown         = false;
    mRMouseDown         = false;
    mMoveScale          = 0.0f;
    mTranslateVector    = Ogre::Vector3::ZERO;
    mObstacleType       = POT1;
    mRecast             = NULL;
    mDetourTileCache    = NULL;
    mNavMeshNode        = NULL;
    mDebugDraw          = true;
	mCurrentObject		= NULL;
    mWall               = false;
    mDisasterState      = false;
    mCurrentState       =  true;
    mCountDown          = 10 * 1000;
    isfirst             = true;
    timecounter         = 0;
}

//-----------------------------------------------

void ExhibitionState::enter()
{
    mOgre->mLog->logMessage( "Entering ExhibitionState..." );
    
    createSceneManager();
    createCamera();
    setViewport();
    
    createScene();

    buildGUI();
}

//-----------------------------------------------

void ExhibitionState::createSceneManager( void )
{
    mSceneMgr = mOgre->mRoot->createSceneManager( Ogre::ST_EXTERIOR_CLOSE, "ExhibitionSceneMgr" );
    mSceneMgr->setAmbientLight( Ogre::ColourValue( 0.7f, 0.7f, 0.7f ) );
    mOgre->mSceneMgr = mSceneMgr;
	
	//Ogre::ParticleSystem* par = mSceneMgr->createParticleSystem("haha","Examples/Fire");
}

//-----------------------------------------------

void ExhibitionState::createCamera( void )
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

void ExhibitionState::setViewport( void )
{
    mOgre->mViewport->setCamera( mCamera );
    mOgre->mViewport->setBackgroundColour( Ogre::ColourValue( 0.0f, 0.0f, 0.0f, 1.0f ) );
    
    mCamera->setAspectRatio( Ogre::Real( mOgre->mViewport->getActualWidth() ) / Ogre::Real( mOgre->mViewport->getActualHeight() ) );
}

//-----------------------------------------------

void ExhibitionState::buildGUI( void )
{
    mOgre->mTrayMgr->destroyAllWidgets();
    //mOgre->mTrayMgr->showFrameStats( OgreBites::TL_BOTTOMLEFT );
    //mOgre->mTrayMgr->showLogo( OgreBites::TL_BOTTOMRIGHT );
    mOgre->mTrayMgr->showCursor();
    
    mOgre->mTrayMgr->createLabel( OgreBites::TL_TOP, "MenuLbl", "Exhibition mode", 250 );
    mTimeCount = mOgre->mTrayMgr->createLabel( OgreBites::TL_TOPLEFT, "TimeLbl", "Time Left: " + Ogre::StringConverter::toString( mCountDown / 1000, 3, 0, ' ', std::ios::fixed ) + " s", 250 );

    Ogre::StringVector stateMenuList;
	stateMenuList.push_back( "Creation State" );
	stateMenuList.push_back( "Crowd State" );
	

	mOgre->mTrayMgr->createThickSelectMenu( OgreBites::TL_TOPRIGHT, "mStateMenu", "state", 200, 5, stateMenuList );

    mOgre->mTrayMgr->createSeparator( OgreBites::TL_TOPRIGHT, "seperator0", 200.0f );

    Ogre::StringVector obstacleMenuList;
    obstacleMenuList.push_back( "Box" );

	obstacleMenuList.push_back( "Pot1" );
    obstacleMenuList.push_back( "Pot2" );
	
	obstacleMenuList.push_back( "Table1" );
    obstacleMenuList.push_back( "Table2" );

	obstacleMenuList.push_back( "Chair1" );

    mObstacleMenu = 
        mOgre->mTrayMgr->createThickSelectMenu( OgreBites::TL_NONE, "obstacleMenu", "obstacle", 200, 5, obstacleMenuList );

    mSeparator1 = mOgre->mTrayMgr->createSeparator( OgreBites::TL_NONE, "seperator1", 200.0f );

	mDebugDrawBox = mOgre->mTrayMgr->createCheckBox( OgreBites::TL_TOPRIGHT, "debugDrawBox", "Debug Draw (v)", 200 );
	mLogoBox = mOgre->mTrayMgr->createCheckBox( OgreBites::TL_TOPRIGHT, "logoBox", "Logo", 200 );
	mFrameStatesBox = mOgre->mTrayMgr->createCheckBox( OgreBites::TL_TOPRIGHT, "frameStatesBox", "frameStates", 200 );
	
    mSeparator2 = mOgre->mTrayMgr->createSeparator( OgreBites::TL_NONE, "seperator2", 200.0f );
	
    mSaveBtn = mOgre->mTrayMgr->createButton( OgreBites::TL_NONE, "SaveBtn", "save", 200 );
    mClearBtn = mOgre->mTrayMgr->createButton( OgreBites::TL_NONE, "ClearBtn", "clear", 200 );
    mLoadBtn = mOgre->mTrayMgr->createButton( OgreBites::TL_NONE, "LoadBtn", "load", 200 );
	mDetailBtn = mOgre->mTrayMgr->createButton( OgreBites::TL_NONE, "DetailBtn", "Detail", 200 );

	mDeleteBtn = mOgre->mTrayMgr->createButton( OgreBites::TL_NONE, "DeleteBtn", "Delete(Del)", 150 );
	mRotateBtn = mOgre->mTrayMgr->createButton( OgreBites::TL_NONE, "RotateBtn", "Rotate", 150 );

    mDebugDrawBox->setChecked( true );
	mLogoBox->setChecked( false );
	mFrameStatesBox->setChecked( false );

    setStateGUI( true );
	showDynamicBotton( false );
}

//-----------------------------------------------

void ExhibitionState::createScene()
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
 //   Ogre::Entity* ent = mSceneMgr->createEntity( "End1", "Pot.mesh" );
 //   Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "End1Node", Ogre::Vector3( 500, 0, 500 ) );
 //   mNode->setScale( 5, 5, 5 );
 //   mNode->attachObject( ent );
 //   ent->setCastShadows( true );
	//ent->setQueryFlags( DEFAULT_MASK );   // Exclude from ray queries
 //   mNavmeshEnts.push_back( ent );  // Add obstacle

		for(int i = 0 ; i<FIRENUM ; i++){
				char tmp[100];
				sprintf(tmp,"fireNode%d",i);
				fireVec[i] = Ogre::Vector3( 750+ (20+(rand()%15)) * (int)(i/20) , 0 , 750+( 20+(rand()%15))*(int)(i%20) );
				fireNode[i]=mSceneMgr-> getRootSceneNode()->createChildSceneNode(tmp,fireVec[i]);
				firepar[i] = mSceneMgr->createParticleSystem(tmp,"Examples/Fire");
				firepar[i]->setVisible(false);
				fireNode[i]->attachObject(firepar[i]);
				isfire[i]=false;
				srand( time(NULL) );
				setToFire[i] = 3+(rand()%3);

		}


	

    //----------------------------------------------------------------------
        
    OgreRecastConfigParams recastParams = OgreRecastConfigParams();
    recastParams.setCellSize(2);
    recastParams.setCellHeight(15);
    recastParams.setAgentMaxSlope(75);
    recastParams.setAgentHeight(25);
    recastParams.setAgentMaxClimb(20);
    recastParams.setAgentRadius(10);
    recastParams.setEdgeMaxLen(12);
    recastParams.setEdgeMaxError(1.3f);
    recastParams.setRegionMinSize(16);
    recastParams.setRegionMergeSize(5);
    recastParams.setDetailSampleDist(6);
    recastParams.setDetailSampleMaxError(1);

    mRecast = new OgreRecast( mSceneMgr, recastParams );    // Use default configuration
    
    mDetourTileCache = new OgreDetourTileCache( mRecast );
    mDetourTileCache->TileCacheBuild( mNavmeshEnts );
    mDetourTileCache->drawNavMesh();
    
    mNavMeshNode = (Ogre::SceneNode*)mSceneMgr->getRootSceneNode()->getChild( "RecastSN" );
    mNavMeshNode->setVisible( mDebugDraw );
    
    mDetourCrowd = new OgreDetourCrowd( mRecast );
    mObstacleMgr = new ObstacleManager( mSceneMgr, recastParams.getAgentRadius(), mDetourTileCache );
    mCharacterMgr = new CharacterManager( mSceneMgr, mDetourCrowd, mDetourTileCache );
	/*
    for( int i = 0; i < 100; i++ )
        mCharacterMgr->createCharacter( mRecast->getRandomNavMeshPoint(), mRecast->getRandomNavMeshPoint(), mDebugDraw );
	*/
    //----------------------------------------------------------------------
    
    Ogre::Light* pointLight = mSceneMgr->createLight( "pointLight" );
    pointLight->setType( Ogre::Light::LT_POINT );
    pointLight->setPosition( Ogre::Vector3( 750, 5000, 750 ) );
    
    pointLight->setDiffuseColour( 1.0, 1.0, 1.0 );
    pointLight->setSpecularColour( 1.0, 1.0, 1.0 );
}

//-----------------------------------------------

void ExhibitionState::exit( void )
{
    mOgre->mLog->logMessage( "Leaving ExhibitionState..." );

    if ( mSceneMgr )
    {
        mSceneMgr->destroyCamera( mCamera );
        mOgre->mRoot->destroySceneManager( mSceneMgr );
    }
    
    mNavmeshEnts.clear();
    mCharacterMgr->~CharacterManager();

    mOgre->mTrayMgr->clearAllTrays();
    mOgre->mTrayMgr->destroyAllWidgets();
    mOgre->mTrayMgr->setListener( 0 );
}

//-----------------------------------------------

bool ExhibitionState::pause( void )
{
    mOgre->mLog->logMessage( "Pausing ExhibitionState..." );
    return true;
}

//-----------------------------------------------

void ExhibitionState::resume()
{
    mOgre->mLog->logMessage( "Resuming ExhibitionState..." );
    
    setViewport();

    buildGUI();
    
    mQuit = false;
}

//-----------------------------------------------

void ExhibitionState::moveCamera()
{
    mCamera->moveRelative( mTranslateVector / 10 );
}

//-----------------------------------------------

void ExhibitionState::getInput()
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

bool ExhibitionState::keyPressed( const OIS::KeyEvent &keyEventRef )
{
    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_ESCAPE ) )
    {
        pushAppState( findByName( "PauseState" ) );
        return true;
    }

    //for crowd state
    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_SPACE ) && mDetourCrowd->getNbAgents() < mDetourCrowd->getMaxNbAgents() && mCurrentState == 0 && mCountDown > 0 ) 
    {
        Ogre::Vector3 rayHitPoint;
        if ( queryCursorPosition( rayHitPoint ) ) 
        {    
            mOgre->mLog->logMessage( "Info: added agent at position "+Ogre::StringConverter::toString(rayHitPoint) );
            mCharacterMgr->createCharacter( rayHitPoint, mRecast->getRandomNavMeshPoint(), mDebugDraw );
        }
    }

    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_Z ) && mDetourCrowd->getNbAgents() < mDetourCrowd->getMaxNbAgents() && mCurrentState == 0 ) 
    {    
        Ogre::Vector3 rayHitPoint;
        if ( queryCursorPosition( rayHitPoint ) ) 
        {    
            mOgre->mLog->logMessage( "Info: added ExitPoint at position "+Ogre::StringConverter::toString(rayHitPoint) );
            mCharacterMgr->createExitPoint( rayHitPoint );
        } 
        //mDisasterState = !mDisasterState;
    }

    //for create state
    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_SPACE ) && mCurrentState == 1 )
    {
        Ogre::Vector3 rayHitPoint;
        if ( queryCursorPosition( rayHitPoint ) )
        	mObstacleMgr->createObstacle( rayHitPoint, mObstacleType );
		setDebugVisibility( mDebugDraw );
    }

    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_Z ) && mCurrentState == 1 )
	{
        mWall = !mWall;
	}

    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_N ) ) 
        mCharacterMgr->updateRandomDestination();


    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_V ) ) 
        setDebugVisibility( !mDebugDraw );
    
    mOgre->keyPressed( keyEventRef );

    return true;
}

//-----------------------------------------------

bool ExhibitionState::keyReleased( const OIS::KeyEvent &keyEventRef )
{
    mOgre->keyReleased( keyEventRef );
    return true;
}

//-----------------------------------------------

bool ExhibitionState::mouseMoved(const OIS::MouseEvent &evt)
{
    if ( mOgre->mTrayMgr->injectMouseMove( evt ) ) 
        return true;

    if ( mRMouseDown )
    {
        mCamera->yaw( Ogre::Degree( evt.state.X.rel * -0.1f ) );
        mCamera->pitch( Ogre::Degree( evt.state.Y.rel * -0.1f ) );
    }
    if( mWall && mCurrentState == 1 )
    {
		ObstacleType mObType = POT1;
		Ogre::Vector3 rayHitPoint;
        if ( queryCursorPosition( rayHitPoint ) )
        	mObstacleMgr->createObstacle( rayHitPoint, mObType );
		setDebugVisibility( mDebugDraw );	
    }

    return true;
}

//-----------------------------------------------

bool ExhibitionState::mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
    if ( mOgre->mTrayMgr->injectMouseDown( evt, id ) ) 
        return true;

    if ( id == OIS::MB_Left )
        mLMouseDown = true;
    else if ( id == OIS::MB_Right )
        mRMouseDown = true;
  
    /*
    if ( mLMouseDown && mCurrentState == 0 )    //crowd
    {
        Ogre::Vector3 rayHitPoint;
        if ( queryCursorPosition( rayHitPoint, NAVMESH_MASK, true ) ) 
            mCharacterMgr->updateAllDestination( rayHitPoint );
    }
    */
    if ( mLMouseDown && mCurrentState == 1 )    //creation
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

bool ExhibitionState::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
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

void ExhibitionState::update( double timeSinceLastFrame )
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    mOgre->mTrayMgr->frameRenderingQueued( mFrameEvent );

    mDetourTileCache->handleUpdate( timeSinceLastFrame );
    mDetourCrowd->updateTick( timeSinceLastFrame );
    
    mMoveScale = 4000  * timeSinceLastFrame;
    mTranslateVector = Ogre::Vector3::ZERO;
    
    if( !mCurrentState )
        if ( mCountDown <= 0 )
        {    
            mCountUp += timeSinceLastFrame * 1000;
			mTimeCount->setCaption( "Time Started: " + Ogre::StringConverter::toString( mCountUp / 1000, 3, 0, ' ', std::ios::fixed ) + " s" );
            mDisasterState = true;
		}else
        {    
            mCountDown -= timeSinceLastFrame * 1000;
			if( mCountDown < 0 )  mCountDown = 0;
			mTimeCount->setCaption( "Time Left: " + Ogre::StringConverter::toString( mCountDown / 1000, 3, 0, ' ', std::ios::fixed ) + " s" );
        }
	else
	{
		
		mTimeCount->setCaption( "Time Left: " + Ogre::StringConverter::toString( mCountDown / 1000, 3, 0, ' ', std::ios::fixed ) + " s" );
	}

    mCharacterMgr->update( mDisasterState, timeSinceLastFrame );
        
  
	/**********************            add          fire        **************************/
    
    
	 
		if(mDisasterState){
			 timecounter += timeSinceLastFrame;
			 if(timecounter > 1){

				timecounter = 0;

				if(isfirst)
				{
					 firepar[50]->setVisible(true);
					 isfire[50] = true;
					Ogre::AxisAlignedBox aa(  fireVec[50].x-25,  0 ,
						fireVec[50].z-25 ,fireVec[50].x+25 ,500 ,fireVec[50].z+25 );

					mConvexHull[50] = new ConvexVolume(aa, 20);
					mConvexHull[50]->area = RC_NULL_AREA;   
					mDetourTileCache->addConvexShapeObstacle(mConvexHull[50]);
					setDebugVisibility( mDebugDraw );

					isfirst=false;

				}else 
				{
			
						for(int i=0; i <FIRENUM; i++){

						if(isfire[i] == true)
						{
								if((i%20)!=0)
								{
									setToFire[i-1]--;
								}

								if(i-20>0)
								{
									 setToFire[i-20]--;
								}
								if(i+20<400)
								{
									setToFire[i+20]--;
								}
								if((i%19)!=0)
								{
									setToFire[i+1]--;
								}
							}else{
								if(setToFire[i]<=0)
							   { 
                                
									    firepar[i]->setVisible(true);                       
										Ogre::AxisAlignedBox aa(  fireVec[i].x-25,  0 ,
									    fireVec[i].z-25 ,fireVec[i].x+25 ,500 ,fireVec[i].z+25 );
									    mConvexHull[i] = new ConvexVolume(aa, 20);
										mConvexHull[i]->area = RC_NULL_AREA;   
										mDetourTileCache->addConvexShapeObstacle(mConvexHull[i]);
										setDebugVisibility( mDebugDraw );
										isfire[i]=true;
						 
							 }

						}
				
				}


		}
		
	}

		//fireNode[50]->attachObject(firepar[50]);
		//mGrid[0] = new Grid(Ogre::Vector3(750,0,750),mSceneMgr,5,0);
		//mGrid[0]->setFire();
		//Ogre::SceneNode* fireNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(750,0,750));
		//Ogre::ParticleSystem* firepar = mSceneMgr ->createParticleSystem("haha","Examples/Fire");
		//fireNode->attachObject(firepar);


		//Ogre::AxisAlignedBox	aa(730,0,730,760,500,760 );

		//ConvexVolume* mConvexHull = new ConvexVolume(aa, 10);
		// mConvexHull->area = RC_NULL_AREA;   
	//	 mDetourTileCache->addConvexShapeObstacle(mConvexHull);
	}


	/**********************            add          fire        **************************/


    getInput();
    moveCamera();
    
    if( mQuit == true )
        shutdown();
}

//-----------------------------------------------

void ExhibitionState::buttonHit(OgreBites::Button *button)
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
		setDebugVisibility( mDebugDraw );
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
		setDebugVisibility( mDebugDraw );
	}
}

//-----------------------------------------------

void ExhibitionState::itemSelected( OgreBites::SelectMenu* menu )
{
    if( menu->getName() == "mStateMenu" )
		switch( menu->getSelectionIndex() )
		{
            case 1:
                mCurrentState = 0;  //crowd
                setStateGUI( false );
				mCountUp = 0;
                mCountDown = COUNTTIME;
                mDisasterState = false;
                for( int i = 0; i < 10; i++ )
                    mCharacterMgr->createCharacter( mRecast->getRandomNavMeshPoint(), mRecast->getRandomNavMeshPoint(), mDebugDraw );
                break;
            case 0:
                mCurrentState = 1;  //creation
				deletFire();
                setStateGUI( true );
				mCountUp = 0;
                mCountDown = COUNTTIME;
                mDisasterState = false;
                mCharacterMgr->deleteAllCharacter();
                mCharacterMgr->deleteAllExitPoint();
                break;
		}
	else if( menu->getName() == "obstacleMenu" )
		switch( menu->getSelectionIndex() )
		{
			case 0:
				mObstacleType =  BOX;
				break;
            case 1:
				mObstacleType =  POT1;
				break;
			case 2:
				mObstacleType =  POT2;
				break;
			case 3:
				mObstacleType =  TABLE1;
				break;
            case 4:
				mObstacleType =  TABLE2;
				break;
			case 5:
				mObstacleType =  CHAIR1;
				break;
            default:
                mObstacleType =  BOX;
				break;

		}
}

//-----------------------------------------------

void ExhibitionState::checkBoxToggled( OgreBites::CheckBox* box )
{
	if( box->getName() == "debugDrawBox" )
		setDebugVisibility( box->isChecked() ? true : false );
	else if( box->getName() == "logoBox" )
	{
		if( mLogoBox->isChecked() )
			mOgre->mTrayMgr->showLogo( OgreBites::TL_BOTTOMRIGHT );
		else
			mOgre->mTrayMgr->hideLogo();
	}
	else if( box->getName() == "frameStatesBox" )
	{
		if( mFrameStatesBox->isChecked() )
			mOgre->mTrayMgr->showFrameStats( OgreBites::TL_BOTTOMLEFT );
		else
			mOgre->mTrayMgr->hideFrameStats();
	}

}

//-----------------------------------------------

bool ExhibitionState::queryCursorPosition(Ogre::Vector3 &rayHitPoint, unsigned long queryflags, bool clipToNavmesh, Ogre::MovableObject **rayHitObject)
{
    Ogre::Ray mouseRay = mOgre->mTrayMgr->getCursorRay( mCamera );
    
    Ogre::MovableObject *hitObject;
    if ( rayQueryPointInScene( mouseRay, queryflags, rayHitPoint, &hitObject ) ) 
    {
        if( clipToNavmesh ) 
            mRecast->findNearestPointOnNavmesh(rayHitPoint, rayHitPoint);
        
        if (rayHitObject)
            *rayHitObject = hitObject;

        return true;
    }
    return false;
}

//-----------------------------------------------

bool ExhibitionState::rayQueryPointInScene(Ogre::Ray ray, unsigned long queryMask, Ogre::Vector3 &result, Ogre::MovableObject **foundMovable)
{
    mRayScnQuery = mSceneMgr->createRayQuery( ray, queryMask );

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

void ExhibitionState::setDebugVisibility( bool visible )
{
    mDebugDraw = visible;

    mNavMeshNode->setVisible( mDebugDraw );
    mCharacterMgr->setDebugVisibility( mDebugDraw );
}

//-----------------------------------------------

void ExhibitionState::showDynamicBotton( bool visible )
{
	if( visible )
	{
		mDeleteBtn->show();
		mRotateBtn->show();
		mOgre->mTrayMgr->moveWidgetToTray( mDeleteBtn, OgreBites::TL_BOTTOM, 0 );
		mOgre->mTrayMgr->moveWidgetToTray( mRotateBtn, OgreBites::TL_BOTTOM, 0 );
	}
	else
	{
		mDeleteBtn->hide();
		mRotateBtn->hide();
		mOgre->mTrayMgr->moveWidgetToTray( mDeleteBtn, OgreBites::TL_NONE, 0 );
		mOgre->mTrayMgr->moveWidgetToTray( mRotateBtn, OgreBites::TL_NONE, 0 );
	}
}

//-----------------------------------------------

void ExhibitionState::setStateGUI( bool visible )
{
    if( visible )
	{
        mObstacleMenu->show();
		mSaveBtn->show();
		mClearBtn->show();
        mLoadBtn->show();
        mSeparator1->show();
        mSeparator2->show();
        mDetailBtn->show();

        mOgre->mTrayMgr->moveWidgetToTray( mSeparator1, OgreBites::TL_TOPRIGHT );
        mOgre->mTrayMgr->moveWidgetToTray( mObstacleMenu, OgreBites::TL_TOPRIGHT );
        mOgre->mTrayMgr->moveWidgetToTray( mSeparator2, OgreBites::TL_TOPRIGHT );
		mOgre->mTrayMgr->moveWidgetToTray( mSaveBtn, OgreBites::TL_TOPRIGHT );
		mOgre->mTrayMgr->moveWidgetToTray( mClearBtn, OgreBites::TL_TOPRIGHT );
        mOgre->mTrayMgr->moveWidgetToTray( mLoadBtn, OgreBites::TL_TOPRIGHT );
        mOgre->mTrayMgr->moveWidgetToTray( mDetailBtn, OgreBites::TL_TOPRIGHT );
	}
	else
	{
        mObstacleMenu->hide();
		mSaveBtn->hide();
		mClearBtn->hide();
        mLoadBtn->hide();
        mSeparator1->hide();
        mSeparator2->hide();
        mDetailBtn->hide();

        mOgre->mTrayMgr->moveWidgetToTray( mSeparator1, OgreBites::TL_NONE );
        mOgre->mTrayMgr->moveWidgetToTray( mObstacleMenu, OgreBites::TL_NONE );
        mOgre->mTrayMgr->moveWidgetToTray( mSeparator2, OgreBites::TL_NONE );
        mOgre->mTrayMgr->moveWidgetToTray( mSaveBtn, OgreBites::TL_NONE );
		mOgre->mTrayMgr->moveWidgetToTray( mClearBtn, OgreBites::TL_NONE );
        mOgre->mTrayMgr->moveWidgetToTray( mLoadBtn, OgreBites::TL_NONE );
        mOgre->mTrayMgr->moveWidgetToTray( mDetailBtn, OgreBites::TL_NONE );
	}
}
//-----------------------------------------------

void ExhibitionState::deletFire(){

	for(int i = 0 ;  i< FIRENUM ; i++){

		firepar[i]->setVisible(false); 
		isfire[i] = false;
		setToFire[i] = 3+(rand()%3);
		isfirst = true;
		mDetourTileCache->removeConvexShapeObstacle( mConvexHull[i] );
		setDebugVisibility( mDebugDraw );

	}
	
}

//-----------------------------------------------
 
Grid::Grid(Ogre::Vector3	  mposition, Ogre::SceneManager*	SceneMgr, int setfire,  int	   num)
{

	mSceneMgr = SceneMgr;
	position = mposition;

	/*
	mRecast = Recast;
	mDetourTileCache = DetourTileCache;
	mDetourCrowd = DetourCrowd;
	*/

	/*************************************************/

	name = creatname(num);
	setToFire = setfire;
	timeToExtin = 8;
	isfire = false;

	/*************************************************/

	firePar = mSceneMgr ->createParticleSystem("haha","Examples/Fire");
	mScenenode = mSceneMgr->getRootSceneNode()->createChildSceneNode("haha",position);

}
//---------------------------------------------
char* Grid::creatname(int num)
{		
	char tmp[20];
		sprintf(tmp, "%d fireballNode : ",num);
		return tmp;

}
//---------------------------------------------
void Grid::setFire()
{
	mScenenode->attachObject(firePar);
	isfire = true;
	/*
	Ogre::AxisAlignedBox	aa( position.x-20, 0 , position.z-20 , position.x+20 ,500 ,position.z+20 );

	ConvexVolume* mConvexHull = new ConvexVolume(aa, 10);
	mConvexHull->area = RC_NULL_AREA;   
	mDetourTileCache->addConvexShapeObstacle(mConvexHull);
	*/

}
//---------------------------------------------
void Grid::deletFire(){
	mScenenode->detachObject(firePar);
}