#include "PauseState.h"

//-----------------------------------------------

PauseState::PauseState()
{
    mQuit             = false;
    mQuestionActive   = false;
    mFrameEvent       = Ogre::FrameEvent();
}

//-----------------------------------------------

void PauseState::enter()
{
    mOgre->mLog->logMessage( "Entering PauseState..." );

    mQuit = false;

    createSceneManager();
    createCamera();
    setViewport();

    buildGUI();

    createScene();
}

//-----------------------------------------------

void PauseState::createSceneManager( void )
{
    mSceneMgr = mOgre->mRoot->createSceneManager( Ogre::ST_GENERIC, "PauseSceneMgr" );
    mSceneMgr->setAmbientLight( Ogre::ColourValue( 0.0f, 0.0f, 0.0f ) );
}

//-----------------------------------------------

void PauseState::createCamera( void )
{
    if ( mSceneMgr )
    {
        mCamera = mSceneMgr->createCamera( "PauseCam" );
        mCamera->setPosition( Ogre::Vector3( 0, 25, -50 ) );
        mCamera->lookAt( Ogre::Vector3( 0, 0, 0 ) );
        mCamera->setNearClipDistance( 1 );
        mOgre->mCamera = mCamera;
    }
}

//-----------------------------------------------

void PauseState::setViewport( void )
{
    mOgre->mViewport->setCamera( mCamera );
    mOgre->mViewport->setBackgroundColour( Ogre::ColourValue( 0.0f, 0.0f, 0.0f, 1.0f ) );
    
    mCamera->setAspectRatio( Ogre::Real( mOgre->mViewport->getActualWidth() ) / Ogre::Real( mOgre->mViewport->getActualHeight() ) );
}

//-----------------------------------------------

void PauseState::buildGUI( void )
{
    mOgre->mTrayMgr->destroyAllWidgets();
    mOgre->mTrayMgr->showFrameStats( OgreBites::TL_BOTTOMLEFT );
    mOgre->mTrayMgr->showLogo( OgreBites::TL_BOTTOMRIGHT );
    mOgre->mTrayMgr->showCursor();
    mOgre->mTrayMgr->createButton( OgreBites::TL_CENTER, "BackToSimulationBtn", "Return to SimulationState", 250 );
    mOgre->mTrayMgr->createButton( OgreBites::TL_CENTER, "BackToMenuBtn", "Return to Menu", 250 );
    mOgre->mTrayMgr->createButton( OgreBites::TL_CENTER, "ExitBtn", "Exit Demo", 250 );
    mOgre->mTrayMgr->createLabel( OgreBites::TL_TOP, "PauseLbl", "Pause mode", 250 );
}


//-----------------------------------------------

void PauseState::createScene( void )
{
}

//-----------------------------------------------

void PauseState::exit( void )
{
    mOgre->mLog->logMessage( "Leaving PauseState..." );

    if ( mSceneMgr )
    {
        mSceneMgr->destroyCamera( mCamera );
        mOgre->mRoot->destroySceneManager( mSceneMgr );
    }

    mOgre->mTrayMgr->clearAllTrays();
    mOgre->mTrayMgr->destroyAllWidgets();
    mOgre->mTrayMgr->setListener( 0 );
}

//-----------------------------------------------

bool PauseState::keyPressed( const OIS::KeyEvent &keyEventRef )
{
    if (mOgre->mKeyboard->isKeyDown( OIS::KC_ESCAPE ) && !mQuestionActive )
    {
        mQuit = true;
        return true;
    }

    mOgre->keyPressed( keyEventRef );
    return true;
}

//-----------------------------------------------

bool PauseState::keyReleased( const OIS::KeyEvent &keyEventRef )
{
    mOgre->keyReleased( keyEventRef );
    return true;
}

//-----------------------------------------------

bool PauseState::mouseMoved( const OIS::MouseEvent &evt )
{
    if ( mOgre->mouseMoved( evt ) )
        return true;
    return true;
}

//-----------------------------------------------

bool PauseState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if ( mOgre->mousePressed( evt, id ) )
        return true;
    return true;
}

//-----------------------------------------------

bool PauseState::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
    if ( mOgre->mouseReleased( evt, id ) )
        return true;
    return true;
}

//-----------------------------------------------

void PauseState::update( double timeSinceLastFrame )
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    mOgre->mTrayMgr->frameRenderingQueued( mFrameEvent );

    if ( mQuit == true )
    {
        popAppState();
        return;
    }
}

//-----------------------------------------------

void PauseState::buttonHit( OgreBites::Button *button )
{
    if ( button->getName() == "ExitBtn" )
    {
        mOgre->mTrayMgr->showYesNoDialog( "Sure?", "Really leave?" );
        mQuestionActive = true;
    }
    else if ( button->getName() == "BackToSimulationBtn" )
        mQuit = true;
    else if ( button->getName() == "BackToMenuBtn" )
        popAllAndPushAppState( findByName( "MenuState" ) );
}

//-----------------------------------------------

void PauseState::yesNoDialogClosed( const Ogre::DisplayString& question, bool hitYes )
{
    if ( hitYes == true )
        shutdown();
    else
        mOgre->mTrayMgr->closeDialog();

    mQuestionActive = false;
}

//-----------------------------------------------
