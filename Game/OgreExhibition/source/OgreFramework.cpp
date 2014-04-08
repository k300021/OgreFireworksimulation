#include "OgreFramework.h"

//-----------------------------------------------

OgreFramework::OgreFramework( void ): 
    mRoot( NULL ),
    mViewport( NULL ),
    mCamera( NULL ),
    mSceneMgr( NULL ),
    mWindow( NULL ),
    mResourcesCfg( Ogre::StringUtil::BLANK ),
    mPluginsCfg( Ogre::StringUtil::BLANK ),
    mTrayMgr( NULL ),
    mCursorWasVisible( false ),
    mShutDown( false ),
    mInputManager( NULL ),
    mMouse( NULL ),
    mKeyboard( NULL ),
    mLog( NULL ),
    mTimer( NULL )
{
}

//-----------------------------------------------

OgreFramework::~OgreFramework( void )
{
    if ( mTrayMgr ) 
        delete mTrayMgr;
    
    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener( mWindow, this );
    windowClosed( mWindow );
    delete mRoot;
}

//-----------------------------------------------

bool OgreFramework::configure( const Ogre::String Title )
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if( mRoot->showConfigDialog() )
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise( true, Title );
        return true;
    }
    else
        return false;
}

//-----------------------------------------------

void OgreFramework::createFrameListener( void )
{
    mLog->logMessage( "*** Initializing OIS ***" );
    
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute( "WINDOW", &windowHnd );
    windowHndStr << windowHnd;
    pl.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str() ) );

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast< OIS::Keyboard* > ( mInputManager->createInputObject( OIS::OISKeyboard, true ) );
    mMouse = static_cast< OIS::Mouse* > ( mInputManager->createInputObject( OIS::OISMouse, true ) );

    mMouse->setEventCallback( this );
    mKeyboard->setEventCallback( this );

    //Set initial mouse clipping size
    windowResized( mWindow );

    mSceneMgr = mRoot->createSceneManager( Ogre::ST_GENERIC, "OgreSceneMgr" );
    mCamera = mSceneMgr->createCamera( "OgreCam" );
    mViewport = mWindow->addViewport( mCamera );
    
    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener( mWindow, this );

    Ogre::FontManager::getSingleton().getByName("SdkTrays/Caption")->load();

    mTrayMgr = new OgreBites::SdkTrayManager( "InterfaceName", mWindow, mMouse, this );
    
    CEGUI::OgreRenderer::bootstrapSystem();
    
    CEGUI::Imageset::setDefaultResourceGroup( "Imagesets" );
    CEGUI::Font::setDefaultResourceGroup( "Fonts" );
    CEGUI::Scheme::setDefaultResourceGroup( "Schemes" );
    CEGUI::WidgetLookManager::setDefaultResourceGroup( "LookNFeel" );
    CEGUI::WindowManager::setDefaultResourceGroup( "Layouts" );
    
    CEGUI::SchemeManager::getSingleton().create( "OgreTray.scheme" );

    CEGUI::System::getSingleton().setDefaultMouseCursor( "OgreTrayImages", "MouseArrow" );
    CEGUI::MouseCursor::getSingleton().setImage( CEGUI::System::getSingleton().getDefaultMouseCursor() );
    
    CEGUI::MouseCursor::getSingleton().setVisible( false );

    mRoot->addFrameListener( this );
}

//-----------------------------------------------

void OgreFramework::setupResources( void )
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load( mResourcesCfg );

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while ( seci.hasMoreElements() )
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for ( i = settings->begin(); i != settings->end(); ++i )
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation( archName, typeName, secName);
        }
    }
}

//-----------------------------------------------

void OgreFramework::createResourceListener( void )
{
}

//-----------------------------------------------

void OgreFramework::loadResources( void )
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//-----------------------------------------------

bool OgreFramework::go( const Ogre::String Title )
{
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    if ( !setup( Title ) )
        return false;
    
    return true;
}

//-----------------------------------------------

bool OgreFramework::setup( const Ogre::String Title )
{
    new Ogre::LogManager();
    
    mLog = Ogre::LogManager::getSingleton().createLog( "OgreLogfile.log", true, true, false );
    mLog->setDebugOutputEnabled( true );

    mRoot = new Ogre::Root( mPluginsCfg );

    setupResources();

    bool carryOn = configure( Title );
    if ( !carryOn ) 
        return false;

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps( 5 );

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    createFrameListener();
    
    mTimer = new Ogre::Timer();
    mTimer->reset();
    return true;
};

//-----------------------------------------------

bool OgreFramework::frameRenderingQueued( const Ogre::FrameEvent& evt )
{
    if( mWindow->isClosed() )
        return false;

    if( mShutDown )
        return false;

    // Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    mTrayMgr->frameRenderingQueued( evt );
    return true;
}

//-----------------------------------------------

bool OgreFramework::keyPressed( const OIS::KeyEvent &arg )
{
     // don't process any more keys if dialog is up
     if ( mTrayMgr->isDialogVisible() ) 
        return true;  
    // toggle visibility of advanced frame stats
    if ( arg.key == OIS::KC_F )   
        mTrayMgr->toggleAdvancedFrameStats();
    // refresh all textures
    else if ( arg.key == OIS::KC_R )   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::PolygonMode pm;

        switch ( mCamera->getPolygonMode() )
        {
        case Ogre::PM_SOLID:
            pm = Ogre::PM_WIREFRAME;
            break;
        case Ogre::PM_WIREFRAME:
            pm = Ogre::PM_POINTS;
            break;
        default:
            pm = Ogre::PM_SOLID;
        }

        mCamera->setPolygonMode( pm );
    }
    else if( arg.key == OIS::KC_F5 )  
        Ogre::TextureManager::getSingleton().reloadAll();
    // take a screenshot
    else if ( arg.key == OIS::KC_P )   
        mWindow->writeContentsToTimestampedFile( "screenshot", ".jpg" );
    else if ( arg.key == OIS::KC_ESCAPE )
        mShutDown = true;
    
    return true;
}

//-----------------------------------------------

bool OgreFramework::keyReleased( const OIS::KeyEvent &arg )
{
    return true;
}

//-----------------------------------------------

bool OgreFramework::mouseMoved( const OIS::MouseEvent &arg )
{
    if ( mTrayMgr->injectMouseMove( arg ) ) 
        return true;
    return true;
}

//-----------------------------------------------

bool OgreFramework::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if ( mTrayMgr->injectMouseDown( arg, id ) ) 
        return true;
    return true;
}

//-----------------------------------------------

bool OgreFramework::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if ( mTrayMgr->injectMouseUp( arg, id ) ) 
        return true;
    return true;
}

//-----------------------------------------------
//Adjust mouse clipping area
void OgreFramework::windowResized( Ogre::RenderWindow* rw )
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics( width, height, depth, left, top );

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//-----------------------------------------------
//Unattach OIS before window shutdown (very important under Linux)
void OgreFramework::windowClosed( Ogre::RenderWindow* rw )
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem( mInputManager );
            mInputManager = 0;
        }
}

//-----------------------------------------------
