#include "CreationState-CEGUI.h"

//-----------------------------------------------

CreationState_CEGUI::CreationState_CEGUI( void )
{
    mQuit           = false;
    mQuestionActive = false;
    mFrameEvent     = Ogre::FrameEvent();
}

//-----------------------------------------------

void CreationState_CEGUI::enter()
{
    mOgre->mLog->logMessage( "Entering CreationState_CEGUI..." );

    createSceneManager();
    createCamera();
    setViewport();

    buildGUI();

    createScene();
}

//-----------------------------------------------

void CreationState_CEGUI::createSceneManager( void )
{
    mSceneMgr = mOgre->mRoot->createSceneManager( Ogre::ST_GENERIC, "MenuSceneMgr" );
    mSceneMgr->setAmbientLight( Ogre::ColourValue( 0.0f, 0.0f, 0.0f ) );
    mOgre->mSceneMgr = mSceneMgr;
}

//-----------------------------------------------

void CreationState_CEGUI::createCamera( void )
{
    if ( mSceneMgr )
    {
        mCamera = mSceneMgr->createCamera( "CreationCam" );
        mCamera->setPosition( Ogre::Vector3( 0, 25, -50 ) );
        mCamera->lookAt( Ogre::Vector3( 0, 0, 0 ) );
        mCamera->setNearClipDistance( 1 );
        mOgre->mCamera = mCamera;
    }
}

//-----------------------------------------------

void CreationState_CEGUI::setViewport( void )
{
    mOgre->mViewport->setCamera( mCamera );
    mOgre->mViewport->setBackgroundColour( Ogre::ColourValue( 0.0f, 0.0f, 0.0f, 1.0f ) );

    mCamera->setAspectRatio( Ogre::Real( mOgre->mViewport->getActualWidth() ) / Ogre::Real( mOgre->mViewport->getActualHeight() ) );
}

//-----------------------------------------------

void CreationState_CEGUI::buildGUI( void )
{
    mOgre->mTrayMgr->destroyAllWidgets();
    mOgre->mTrayMgr->showFrameStats( OgreBites::TL_BOTTOMLEFT );
    mOgre->mTrayMgr->showLogo( OgreBites::TL_BOTTOMRIGHT );
    mOgre->mTrayMgr->hideCursor();

    CEGUI::MouseCursor::getSingleton().setVisible( true );

    CEGUI::Window *newWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout( "CreationState.layout" );
    CEGUI::System::getSingleton().setGUISheet( newWindow );

//    CEGUI::WindowManager::getSingleton().getWindow( "DefaultWindow/Menubar/Loadbtn" )->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CreationState_CEGUI::EnterExhibitionState, this ) );
 //   CEGUI::WindowManager::getSingleton().getWindow( "DefaultWindow/Menubar/Savebtn" )->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CreationState_CEGUI::EnterCreationState, this ) );
//    CEGUI::WindowManager::getSingleton().getWindow( "DefaultWindow/Menubar/Detailbtn" )->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CreationState_CEGUI::ReturnMenu, this ) );
//    CEGUI::WindowManager::getSingleton().getWindow( "DefaultWindow/Menubar/Cleanbtn" )->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CreationState_CEGUI::ShowCheckEnd, this ) );
    CEGUI::WindowManager::getSingleton().getWindow( "DefaultWindow/Menubar/EndDemoBtn" )->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CreationState_CEGUI::ShowCheckEnd, this ) );
    CEGUI::WindowManager::getSingleton().getWindow( "DefaultWindow/CheckEndBG/Checkbar/NoBtn" )->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CreationState_CEGUI::ReturnDemo, this ) );
    CEGUI::WindowManager::getSingleton().getWindow( "DefaultWindow/CheckEndBG/Checkbar/YesBtn" )->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CreationState_CEGUI::ExitDemoApp, this ) );
}

//-----------------------------------------------

void CreationState_CEGUI::createScene( void )
{
}

//-----------------------------------------------

void CreationState_CEGUI::exit( void )
{
    mOgre->mLog->logMessage( "Leaving CreationState_CEGUI..." );

    CEGUI::WindowManager::getSingleton().destroyAllWindows();
    CEGUI::MouseCursor::getSingleton().setVisible( false );

    mOgre->mTrayMgr->clearAllTrays();
    mOgre->mTrayMgr->destroyAllWidgets();

    if ( mSceneMgr )
    {
        mSceneMgr->destroyCamera( mCamera );
        mOgre->mRoot->destroySceneManager(mSceneMgr);
    }
}

//-----------------------------------------------

bool CreationState_CEGUI::keyPressed( const OIS::KeyEvent &keyEventRef )
{
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectKeyDown( keyEventRef.key );
    sys.injectChar( keyEventRef.text );
    return true;
}

//-----------------------------------------------

bool CreationState_CEGUI::keyReleased( const OIS::KeyEvent &keyEventRef )
{
    CEGUI::System::getSingleton().injectKeyUp( keyEventRef.key );
    return true;
}

//-----------------------------------------------

bool CreationState_CEGUI::mouseMoved( const OIS::MouseEvent &evt )
{
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectMouseMove( evt.state.X.rel, evt.state.Y.rel );

    if ( evt.state.Z.rel )
        sys.injectMouseWheelChange( evt.state.Z.rel / 120.0f );
    return true;
}

//-----------------------------------------------

bool CreationState_CEGUI::mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
    CEGUI::System::getSingleton().injectMouseButtonDown( convertButton( id ) );
    return true;
}

//-----------------------------------------------

bool CreationState_CEGUI::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
    CEGUI::System::getSingleton().injectMouseButtonUp( convertButton( id ) );
    return true;
}

//-----------------------------------------------

void CreationState_CEGUI::update( double timeSinceLastFrame )
{
    mFrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    CEGUI::System::getSingleton().injectTimePulse( timeSinceLastFrame );

    if ( mQuit == true )
    {
        shutdown();
        return;
    }
}

//-----------------------------------------------

bool CreationState_CEGUI::EnterExhibitionState( const CEGUI::EventArgs &arg )
{

    return true;
}

//-----------------------------------------------

bool CreationState_CEGUI::EnterCreationState( const CEGUI::EventArgs &arg )
{

    return true;
}

//-----------------------------------------------

bool CreationState_CEGUI::ReturnMenu( const CEGUI::EventArgs &arg )
{
    popAllAndPushAppState( findByName( "CreationState" ) );
    return true;
}

//-----------------------------------------------

bool CreationState_CEGUI::ShowCheckEnd( const CEGUI::EventArgs &arg )
{
    CEGUI::WindowManager::getSingleton().getWindow( "DefaultWindow/CheckEndBG" )->setVisible( true );
    return true;
}

//-----------------------------------------------

bool CreationState_CEGUI::ReturnDemo( const CEGUI::EventArgs &arg )
{
    CEGUI::WindowManager::getSingleton().getWindow( "DefaultWindow/CheckEndBG" )->setVisible( false );
    return true;
}
//-----------------------------------------------

bool CreationState_CEGUI::ExitDemoApp( const CEGUI::EventArgs &arg )
{
    mQuit = true;
    return true;
}
//-----------------------------------------------

void CreationState_CEGUI::yesNoDialogClosed( const Ogre::DisplayString& question, bool hitYes )
{
    if ( hitYes == true )
        shutdown();
    else
        mOgre->mTrayMgr->closeDialog();

    mQuestionActive = false;
}

//-----------------------------------------------

CEGUI::MouseButton convertButton( OIS::MouseButtonID buttonID )
{
    switch (buttonID)
    {
        case OIS::MB_Left:
            return CEGUI::LeftButton;
        case OIS::MB_Right:
            return CEGUI::RightButton;
        case OIS::MB_Middle:
            return CEGUI::MiddleButton;
        default:
            return CEGUI::LeftButton;
    }
}

//-----------------------------------------------

