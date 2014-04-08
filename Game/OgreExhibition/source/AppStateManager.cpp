#include <OgreWindowEventUtilities.h>
#include "AppStateManager.h"

//-----------------------------------------------

AppStateManager::AppStateManager( void )
{
	mShutdown = false;
    mOgre.go( "DemoApp" );
    mOgre.mLog->logMessage("Demo initialized!");
}

//-----------------------------------------------

AppStateManager::~AppStateManager( void )
{
	state_info si;

    while( !mActiveStateStack.empty() )
	{
		mActiveStateStack.back()->exit();
		mActiveStateStack.pop_back();
	}

	while( !mStates.empty() )
	{
		si = mStates.back();
        si.state->destroy();
        mStates.pop_back();
	}
}

//-----------------------------------------------

void AppStateManager::manageAppState( Ogre::String stateName, AppState* state )
{
	try
	{
		state_info new_state_info;
		new_state_info.name = stateName;
		new_state_info.state = state;
		mStates.push_back( new_state_info );
	}
	catch( std::exception& e )
	{
		delete state;
		throw Ogre::Exception( Ogre::Exception::ERR_INTERNAL_ERROR, "Error while trying to manage a new AppState\n" + Ogre::String(e.what()), "AppStateManager.cpp (39)" );
	}
}

//-----------------------------------------------

AppState* AppStateManager::findByName( Ogre::String stateName )
{
	std::vector<state_info>::iterator itr;

	for( itr=mStates.begin(); itr != mStates.end(); itr++ )
		if( itr->name == stateName )
			return itr->state;
	
	return 0;
}

//-----------------------------------------------

void AppStateManager::start( AppState* state )
{
	changeAppState( state );

	double timeSinceLastFrame = 0;
	int startTime = 0;

	while( !mShutdown )
	{
		if( mOgre.mWindow->isClosed() )
            mShutdown = true;

		Ogre::WindowEventUtilities::messagePump();

		if( mOgre.mWindow->isActive() )
		{
			startTime = mOgre.mTimer->getMillisecondsCPU();

			mOgre.mKeyboard->capture();
			mOgre.mMouse->capture();

			mActiveStateStack.back()->update( timeSinceLastFrame );

			mOgre.mRoot->renderOneFrame();

			timeSinceLastFrame = double( mOgre.mTimer->getMillisecondsCPU() - startTime ) / 1000;
        }
		else
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep( 1000 );
#else
            sleep( 1 );
#endif
		}
	}

	mOgre.mLog->logMessage( "Main loop quit" );
}

//-----------------------------------------------

void AppStateManager::changeAppState( AppState* state )
{
	if( !mActiveStateStack.empty() )
	{
		mActiveStateStack.back()->exit();
		mActiveStateStack.pop_back();
	}

	mActiveStateStack.push_back( state );
	init( state );
	mActiveStateStack.back()->enter();
}

//-----------------------------------------------

bool AppStateManager::pushAppState( AppState* state )
{
	if( !mActiveStateStack.empty() )
		if( !mActiveStateStack.back()->pause() )
			return false;
	
	mActiveStateStack.push_back( state );
	init( state );
	mActiveStateStack.back()->enter();

	return true;
}

//-----------------------------------------------

void AppStateManager::popAppState( void )
{
	if( !mActiveStateStack.empty() )
	{
		mActiveStateStack.back()->exit();
		mActiveStateStack.pop_back();
	}

	if( !mActiveStateStack.empty() )
	{
		init(mActiveStateStack.back());
		mActiveStateStack.back()->resume();
	}
    else
		shutdown();
}

//-----------------------------------------------

void AppStateManager::popAllAndPushAppState( AppState* state )
{
    while( !mActiveStateStack.empty() )
    {
        mActiveStateStack.back()->exit();
        mActiveStateStack.pop_back();
    }

    pushAppState( state );
}

//-----------------------------------------------

void AppStateManager::pauseAppState( void )
{
	if(!mActiveStateStack.empty() )
		mActiveStateStack.back()->pause();
	
	if( mActiveStateStack.size() > 2 )
	{
		init( mActiveStateStack.at( mActiveStateStack.size() - 2 ) );
		mActiveStateStack.at( mActiveStateStack.size() - 2 )->resume();
	}
}

//-----------------------------------------------

void AppStateManager::shutdown( void )
{
	mShutdown = true;
}

//-----------------------------------------------

void AppStateManager::init(AppState* state)
{
    mOgre.mKeyboard->setEventCallback( state );
	mOgre.mMouse->setEventCallback( state );
    mOgre.mTrayMgr->setListener( state );

	mOgre.mWindow->resetStatistics();
}

//-----------------------------------------------
