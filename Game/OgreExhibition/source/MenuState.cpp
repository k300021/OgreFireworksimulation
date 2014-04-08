#include "MenuState.h"

//-----------------------------------------------

MenuState::MenuState( void )
{
    mQuit           = false;
    mQuestionActive = false;
    mFrameEvent     = Ogre::FrameEvent();
}

//-----------------------------------------------

void MenuState::enter()
{
    mOgre->mLog->logMessage( "Entering MenuState..." );

    createSceneManager();
    createCamera();
    setViewport();
    
    buildGUI();
    
    createScene();
}

//-----------------------------------------------

void MenuState::createSceneManager( void )
{
    mSceneMgr = mOgre->mRoot->createSceneManager( Ogre::ST_GENERIC, "MenuSceneMgr" );
    mSceneMgr->setAmbientLight( Ogre::ColourValue( 0.0f, 0.0f, 0.0f ) );
    mOgre->mSceneMgr = mSceneMgr;
}

//-----------------------------------------------

void MenuState::createCamera( void )
{
    if ( mSceneMgr )
    {
        mCamera = mSceneMgr->createCamera( "MenuCam" );
        mCamera->setPosition( Ogre::Vector3( 0, 25, -50 ) );
        mCamera->lookAt( Ogre::Vector3( 0, 0, 0 ) );
        mCamera->setNearClipDistance( 1 );
        mOgre->mCamera = mCamera;
    }
}

//-----------------------------------------------

void MenuState::setViewport( void )
{
    mOgre->mViewport->setCamera( mCamera );
    mOgre->mViewport->setBackgroundColour( Ogre::ColourValue( 0.0f, 0.0f, 0.0f, 1.0f ) );
    
    mCamera->setAspectRatio( Ogre::Real( mOgre->mViewport->getActualWidth() ) / Ogre::Real( mOgre->mViewport->getActualHeight() ) );
}

//-----------------------------------------------

void MenuState::buildGUI( void )
{
    mOgre->mTrayMgr->destroyAllWidgets();
    mOgre->mTrayMgr->showFrameStats( OgreBites::TL_BOTTOMLEFT );
    mOgre->mTrayMgr->showLogo( OgreBites::TL_BOTTOMRIGHT );
    mOgre->mTrayMgr->showCursor();
    mOgre->mTrayMgr->createButton( OgreBites::TL_CENTER, "EnterExhibitionBtn", "Enter ExhibitionState", 250 );
	mOgre->mTrayMgr->createButton( OgreBites::TL_CENTER, "EnterCreationBtn", "Enter CreationState", 250 );
    mOgre->mTrayMgr->createButton( OgreBites::TL_CENTER, "EnterExhibitionCEGUIBtn", "Enter ExhibitionState-CEGUI", 250 );
    /*
    OgreFramework::getSingletonPtr()->mTrayMgr->createButton( OgreBites::TL_CENTER, "EnterSelectionBtn", "Enter SimulationState-Selection", 250 );
    OgreFramework::getSingletonPtr()->mTrayMgr->createButton( OgreBites::TL_CENTER, "EnterPathFindingBtn", "Enter SimulationState-PathFinding", 250 );
    */
    mOgre->mTrayMgr->createLabel( OgreBites::TL_TOP, "MenuLbl", "Menu mode", 250 );
    mOgre->mTrayMgr->createButton( OgreBites::TL_CENTER, "ExitBtn", "Exit Demo", 250 );
    
}

//-----------------------------------------------

void MenuState::createScene( void )
{
}

//-----------------------------------------------

void MenuState::exit( void )
{
    mOgre->mLog->logMessage( "Leaving MenuState..." );

    if ( mSceneMgr )
    {    
        mSceneMgr->destroyCamera( mCamera );
        mOgre->mRoot->destroySceneManager(mSceneMgr);
    }
    
    mOgre->mTrayMgr->clearAllTrays();
    mOgre->mTrayMgr->destroyAllWidgets();
    mOgre->mTrayMgr->setListener( 0 );
}

//-----------------------------------------------

bool MenuState::keyPressed( const OIS::KeyEvent &keyEventRef )
{
    if ( mOgre->mKeyboard->isKeyDown( OIS::KC_ESCAPE ) )
    {
        mOgre->mTrayMgr->showYesNoDialog("Sure?", "Really leave?");
        mQuestionActive = true;
        return true;
    }

    mOgre->keyPressed( keyEventRef );
    return true;
}

//-----------------------------------------------

bool MenuState::keyReleased( const OIS::KeyEvent &keyEventRef )
{
    mOgre->keyReleased( keyEventRef );
    return true;
}

//-----------------------------------------------

bool MenuState::mouseMoved( const OIS::MouseEvent &evt )
{
    if ( mOgre->mouseMoved( evt ) ) 
        return true;
    return true;
}

//-----------------------------------------------

bool MenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if ( mOgre->mousePressed( evt, id ) ) 
        return true;
    return true;
}

//-----------------------------------------------

bool MenuState::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
    if ( mOgre->mouseReleased( evt, id ) ) 
        return true;
    return true;
}

//-----------------------------------------------

void MenuState::update( double timeSinceLastFrame )
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    mOgre->mTrayMgr->frameRenderingQueued( mFrameEvent );

    if ( mQuit == true )
    {
        shutdown();
        return;
    }
}

//-----------------------------------------------

void MenuState::buttonHit( OgreBites::Button *button )
{
    if ( button->getName() == "ExitBtn" )
    {
        mOgre->mTrayMgr->showYesNoDialog("Sure?", "Really leave?");
        mQuestionActive = true;
    }
    else if ( button->getName() == "EnterExhibitionBtn" )
        changeAppState( findByName( "ExhibitionState" ) );
	else if ( button->getName() == "EnterCreationBtn" )
        changeAppState( findByName( "CreationState" ) );
    else if ( button->getName() == "EnterExhibitionCEGUIBtn" )
        popAllAndPushAppState( findByName( "MenuState_CEGUI" ) );
    /*
    else if ( button->getName() == "EnterFlameBtn" )
        changeAppState( findByName( "SimulationState-Flame" ) );
    else if ( button->getName() == "EnterSelectionBtn" )
        changeAppState( findByName( "SimulationState-Selection" ) );
    else if ( button->getName() == "EnterPathFindingBtn" )
        changeAppState( findByName( "SimulationState-PathFinding" ) );
    */
}

//-----------------------------------------------

void MenuState::yesNoDialogClosed( const Ogre::DisplayString& question, bool hitYes )
{
    if ( hitYes == true )
        shutdown();
    else
        mOgre->mTrayMgr->closeDialog();
    
    mQuestionActive = false;
}

//-----------------------------------------------

