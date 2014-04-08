#include "DemoApp.h"
#include "MenuState.h"
#include "ExhibitionState.h"
#include "CreationState.h"
#include "PauseState.h"
#include "MenuState-CEGUI.h"
#include "CreationState-CEGUI.h"

//-----------------------------------------------

DemoApp::DemoApp( void ):
    mAppStateManager( NULL )
{
}

//-----------------------------------------------

DemoApp::~DemoApp( void )
{
	delete mAppStateManager;
}

//-----------------------------------------------

void DemoApp::startDemo( void )
{
    mAppStateManager = new AppStateManager();
    
    MenuState::create( mAppStateManager, "MenuState" );
	MenuState_CEGUI::create( mAppStateManager, "MenuState_CEGUI" );
	PauseState::create( mAppStateManager, "PauseState" );
	ExhibitionState::create( mAppStateManager, "ExhibitionState" );
	CreationState::create( mAppStateManager, "CreationState" );
	CreationState_CEGUI::create( mAppStateManager, "CreationState_CEGUI" );
	mAppStateManager->start( mAppStateManager->findByName( "MenuState" ) );
}

//-----------------------------------------------

int main(int argc, char *argv[]) 
{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    Ogre::String workingDir = Ogre::macBundlePath() + "/Contents/Resources";
    chdir( workingDir.c_str() );
    std::cout << "working directory: " + workingDir + "\n";
#endif
	DemoApp app;
	srand( time( NULL ) );
    std::cout << "start" << std::endl;
    app.startDemo();  
	return 0;
}

//-----------------------------------------------
