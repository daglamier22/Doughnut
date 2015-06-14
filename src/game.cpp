#include "game.h"

Game::Game()
{
	m_running = true;
	m_BufferedKeys = true;
}//Game::Game

Game::~Game() {
}//Game::~Game

bool Game::initialize() {
	if( !initializeOgre() )
		return false;

	if( !initializeCEGUI() )
		return false;

	if( !initializeOIS() )
		return false;

	Ogre::String meshname = "Pill.mesh";
	Ogre::Entity* entity = m_pScene->createEntity(meshname);
	m_pPlayerSceneNode = m_pRootSceneNode->createChildSceneNode();
	m_pPlayerSceneNode->attachObject(entity);
	m_pPlayerSceneNode->scale(Ogre::Vector3(0.5f, 0.5f, 0.5f));
	m_PlayerPosition = Ogre::Vector3(0.0f, 0.0f, -10.0f);
	m_PlayerVelocity = Ogre::Vector3(0.0f, 0.0f, 0.0f);
	m_pPlayerSceneNode->translate(m_PlayerPosition);
	m_pPlayerSceneNode->setDirection(m_PlayerVelocity);

	Ogre::Light* light = m_pScene->createLight();
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDiffuseColour(0.8f, 0.8f, 0.8f);
	light->setSpecularColour(1.0f, 1.0f, 1.0f);
	light->setDirection(Ogre::Vector3(0, 0, -1));
	Ogre::SceneNode* lightnode = m_pRootSceneNode->createChildSceneNode();
	lightnode->attachObject(light);
	m_pScene->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));

	return true;
}//Game::initialize

bool Game::initializeOgre() {
	try {
		// setup ogre initialization filenames, setting config and plugins filenames to blank disables these features
		Ogre::String configFileName = "";
		Ogre::String pluginsFileName = "";
		Ogre::String logFileName = "Ogre.log";

		// create the root ogre object
		m_pRoot = new Ogre::Root(configFileName, pluginsFileName, logFileName);

		// load the necessary plugins
		std::vector<Ogre::String> pluginNames;
		pluginNames.push_back("/RenderSystem_GL");
		pluginNames.push_back("/Plugin_ParticleFX");
		//pluginNames.push_back("Plugin_CgProgramManager");
		//pluginNames.push_back("Plugin_PCZSceneManager");
		//pluginNames.push_back("Plugin_OctreeZone");
		pluginNames.push_back("/Plugin_OctreeSceneManager");
		//pluginNames.push_back("Plugin_BSPSceneManager");

		for( std::vector<Ogre::String>::iterator itr = pluginNames.begin(); itr != pluginNames.end(); itr++ ) {
			Ogre::String& pluginName = (*itr);
			if( OGRE_DEBUG_MODE ) {
				pluginName.append("_d");
			}
			m_pRoot->loadPlugin(OGRE_PLUGINDIR + pluginName);
		}

		// select the rendersystem from the loaded plugins
		const Ogre::RenderSystemList& renderSystemList = m_pRoot->getAvailableRenderers();
		if( renderSystemList.size() == 0 ) {
			std::cout << "Sorry, no rendersystem was found." << std::endl;
			throw(3);
		}

		Ogre::RenderSystem *renderSystem = renderSystemList[0];
		m_pRoot->setRenderSystem(renderSystem);

		// initialize the root but do not automatically create a window
		bool createAWindowAutomatically = false;
		Ogre::String windowTitle = "";
		Ogre::String customCapacities = "";
		m_pRoot->initialise(createAWindowAutomatically, windowTitle, customCapacities);

		// create a window
		// This is just an example of parameters that we can put. Check the API for more details.
		Ogre::NameValuePairList params;
		// fullscreen antialiasing.
		params["FSAA"] = "0";
		// vertical synchronisation
		params["vsync"] = "true";

		//save the window dimensions to help track mouse usage
		m_WindowWidth = 1024;
		m_WindowHeight = 768;
		m_Fullscreen = false;
		m_pWindow = m_pRoot->createRenderWindow("Doughnut", m_WindowWidth, m_WindowHeight, m_Fullscreen, &params);

		// cleaning of windows events managed by Ogre::WindowEventUtilities
		m_pRoot->clearEventTimes();

		// create the scene manager
		m_pScene = m_pRoot->createSceneManager(Ogre::ST_GENERIC,"TestScene");

		//grab the root node for future use
		m_pRootSceneNode = m_pScene->getRootSceneNode();

		// create the camera
		Ogre::Camera* pCamera = m_pScene->createCamera("MainCamera");

		// attach the camera to a new SceneNode
		m_pCameraNode = m_pRootSceneNode->createChildSceneNode("MainCameraNode",Ogre::Vector3(0.0f, 0.0f, 0.0f));
		m_pCameraNode->attachObject(pCamera);

		// create a viewport
		Ogre::Viewport* vp = m_pWindow->addViewport(pCamera, 100, 0.0f, 0.0f, 1.0f, 1.0f);
		vp->setAutoUpdated(true);
		vp->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f));
		m_pWindow->setActive(true);

		// choose the visual ratio of the camera based on the viewport
		float ratio = float(vp->getActualWidth()) / float(vp->getActualHeight());
		pCamera->setAspectRatio(ratio);

		// choose the clipping far & near planes, if far/near>2000, you can get z buffer problem
		pCamera->setNearClipDistance(1.5f);
		pCamera->setFarClipDistance(3000.0f);

		Ogre::String RGs[8] = {"schemes", "imagesets", "fonts", "layouts", "looknfeel", "lua_scripts", "meshes", "materials"};
		Ogre::ResourceGroupManager& lRgMgr = Ogre::ResourceGroupManager::getSingleton();
		for( int i = 0; i < 8; i++ ) {
			lRgMgr.createResourceGroup(RGs[i]);
			lRgMgr.addResourceLocation("datafiles/"+RGs[i], "FileSystem", RGs[i], false);
			lRgMgr.initialiseResourceGroup(RGs[i]);
		}

		m_pRoot->addFrameListener(this);
	} catch(Ogre::Exception &e) {
		std::cout << "!!!!Ogre::Exception!!!!" << e.what() << std::endl;
	}

	return true;
}//Game:initializeOgre

bool Game::frameRenderingQueued( const Ogre::FrameEvent &evt ) {
	return update(evt.timeSinceLastFrame);
}//Game::frameRenderingQueued

bool Game::initializeCEGUI() {
	//link into the Ogre system
	CEGUI::OgreRenderer& myRenderer = CEGUI::OgreRenderer::bootstrapSystem(*m_pWindow);

	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

	//CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeel");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	// Load the scheme
	CEGUI::SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );

	// Set the defaults
	CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-10.font");
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

	// Create a test GUI
	CEGUI::Window* myRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", (CEGUI::utf8*)"_MasterRoot" );
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(myRoot);

/*	CEGUI::FrameWindow* fWnd = static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().createWindow( "TaharezLook/FrameWindow", (CEGUI::utf8*)"testWindow" ));
	myRoot->addChild( fWnd );
	// position a quarter of the way in from the top-left of parent.
	fWnd->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.25f, 0.0f ), CEGUI::UDim( 0.25f, 0.0f ) ) );
	// set size to be half the size of the parent
	fWnd->setSize( CEGUI::USize( CEGUI::UDim( 0.5f, 0.0f ), CEGUI::UDim( 0.5f, 0.0f ) ) );
	fWnd->setText( (CEGUI::utf8*)"Hello World!" );

	CEGUI::Window *quit = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", (CEGUI::utf8*)"CEGUIDemo/QuitButton");
	quit->setText((CEGUI::utf8*)"Quit");
	quit->setSize( CEGUI::USize( CEGUI::UDim(0.15f, 0.0f), CEGUI::UDim(0.05f, 0.0f) ) );
	myRoot->addChild(quit);
	quit->show();
*/
	return true;
}//Game:initializeCEGUI

bool Game::initializeOIS() {
	//get the window handle;
	size_t windowHandle = 0;
	m_pWindow->getCustomAttribute("WINDOW", &windowHandle);

	std::ostringstream windowHandleStr;
	windowHandleStr << windowHandle;
	// create the input system
	OIS::ParamList pl;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));
#if defined OIS_WIN32_PLATFORM
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
	pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
	pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
	pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
	pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
	m_pInputManager = OIS::InputManager::createInputSystem(pl);
	// create the keyboard
	m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputManager->createInputObject(OIS::OISKeyboard, m_BufferedKeys));
	m_pKeyboard->setEventCallback(this);

	// create the mouse
	m_pMouse = static_cast<OIS::Mouse*>(m_pInputManager->createInputObject( OIS::OISMouse, m_BufferedMouse));
	m_pMouse->setEventCallback(this);
	// tell OIS the range of mouse movement
	OISSetWindowSize();

	return true;
}//Game::initializeOIS

void Game::OISSetWindowSize() {
	// Set mouse region
	const OIS::MouseState &mouseState = m_pMouse->getMouseState();
	mouseState.width  = m_WindowWidth;
	mouseState.height = m_WindowHeight;
}//Game:OISSetWindowSize

bool Game::keyPressed( const OIS::KeyEvent &e ) {

    //send to cegui first
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown((CEGUI::Key::Scan)e.key);
	if( CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text) )
		return true;

	if( e.key == OIS::KC_ESCAPE )
		m_running = false;

	if( e.key == OIS::KC_W )
		m_PlayerVelocity += Ogre::Vector3( 0.0f, 5.0f, 0.0f);
	if( e.key == OIS::KC_A )
		m_PlayerVelocity += Ogre::Vector3(-5.0f, 0.0f, 0.0f);
	if( e.key == OIS::KC_S )
		m_PlayerVelocity += Ogre::Vector3( 0.0f,-5.0f, 0.0f);
	if( e.key == OIS::KC_D )
		m_PlayerVelocity += Ogre::Vector3( 5.0f, 0.0f, 0.0f);

	return true;
}//Game::keyPressed

bool Game::keyReleased( const OIS::KeyEvent &e ) {
	//send to cegui first
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown((CEGUI::Key::Scan)e.key);
	if( CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text) )
		return true;

	if( e.key == OIS::KC_W )
		m_PlayerVelocity -= Ogre::Vector3( 0.0f, 5.0f, 0.0f);
	if( e.key == OIS::KC_A )
		m_PlayerVelocity -= Ogre::Vector3(-5.0f, 0.0f, 0.0f);
	if( e.key == OIS::KC_S )
		m_PlayerVelocity -= Ogre::Vector3( 0.0f,-5.0f, 0.0f);
	if( e.key == OIS::KC_D )
		m_PlayerVelocity -= Ogre::Vector3( 5.0f, 0.0f, 0.0f);

	return true;
}//Game::keyReleased

bool Game::mouseMoved( const OIS::MouseEvent &e ) {
	//send to cegui first
	if( e.state.Z.rel == 0 ) {
		if( m_isMouseInWindow ) {
			if( e.state.X.abs == 0 || e.state.Y.abs == 0 || e.state.X.abs == (int)m_WindowWidth || e.state.Y.abs == (int)m_WindowHeight ) {
				CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
#ifdef __linux__
				// TODO: need linux version
#elif defined _WIN32 || defined _WIN64
				ShowCursor(true); // windows only
#else
#error "unknown platform"
#endif
				m_isMouseInWindow = false;
			}
			if( CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition((float)e.state.X.abs, (float)e.state.Y.abs) )
				return true;
		}
		else if( e.state.X.abs > 0 && e.state.Y.abs > 0 && e.state.X.abs < (int)m_WindowWidth && e.state.Y.abs < (int)m_WindowHeight) {
			CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
#ifdef __linux__
			// TODO: need linux version
#elif defined _WIN32 || defined _WIN64
			ShowCursor(false); // windows only
#else
#error "unknown platform"
#endif
			m_isMouseInWindow = true;
		}
	} else {
		if( CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseWheelChange((float)e.state.Z.rel) )
			return true;
	}

	return true;
}//Game::mouseMoved

bool Game::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	//send to cegui first
	CEGUI::MouseButton translatedButton = CEGUI::NoButton;
	if( id == OIS::MB_Left ) {
		translatedButton = CEGUI::LeftButton;
	}
	else if( id == OIS::MB_Middle ) {
		translatedButton = CEGUI::MiddleButton;
	}
	else if( id == OIS::MB_Right ) {
		translatedButton = CEGUI::RightButton;
	}

	if( CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(translatedButton) )
		return true;

	return true;
}//Game::mousePressed

bool Game::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	//send to cegui first
	CEGUI::MouseButton translatedButton = CEGUI::NoButton;
	if( id == OIS::MB_Left ) {
		translatedButton = CEGUI::LeftButton;
	}
	else if( id == OIS::MB_Middle ) {
		translatedButton = CEGUI::MiddleButton;
	}
	else if( id == OIS::MB_Right ) {
		translatedButton = CEGUI::RightButton;
	}

	if( CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(translatedButton) )
		return true;

	return true;
}//Game::mouseReleased

bool Game::run() {
	while( m_running ) {
		m_pKeyboard->capture();
		m_pMouse->capture();

		m_pWindow->swapBuffers();;
		m_pRoot->renderOneFrame();
		Ogre::WindowEventUtilities::messagePump();
		const Ogre::RenderTarget::FrameStats& lStats = m_pWindow->getStatistics();
		std::cout << "FPS: " << lStats.lastFPS << "; AvgFPS : " << lStats.avgFPS << "; batchcount :" << lStats.batchCount << std::endl;
	}
}//Game::run

bool Game::update( float deltaSec ) {
	m_pPlayerSceneNode->translate(m_PlayerVelocity * deltaSec );
	Ogre::Vector3 src = m_pPlayerSceneNode->getOrientation() * Ogre::Vector3::UNIT_X;
	Ogre::Quaternion quat = src.getRotationTo(m_PlayerVelocity);
	m_pPlayerSceneNode->rotate(quat);

	return true;
}//Game::update

bool Game::shutdown() {
	shutdownOgre();
	shutdownCEGUI();
	shutdownOIS();
}//Game::shutdown

void Game::shutdownOgre() {
	try {
		m_pWindow->removeAllViewports();
		delete m_pWindow;
		m_pScene->destroyAllCameras();
		m_pScene->destroyAllManualObjects();
		m_pScene->destroyAllEntities();
		m_pScene->destroyAllLights();
		delete m_pScene;
		m_pRootSceneNode->removeAndDestroyAllChildren();
		delete m_pRootSceneNode;
	} catch(Ogre::Exception &e) {
		std::cout << "!!!!Ogre::Exception!!!!" << e.what() << std::endl;
	}
}//Game::shutdownOgre

void Game::shutdownCEGUI() {
	CEGUI::OgreRenderer::destroySystem();
	CEGUI::System::destroy();
}//Game::shutdownCEGUI

void Game::shutdownOIS() {
	if( m_pInputManager ) {
		if( m_pKeyboard ) {
			m_pInputManager->destroyInputObject(m_pKeyboard);
			m_pKeyboard = NULL;
		}
		if( m_pMouse ) {
			m_pInputManager->destroyInputObject(m_pMouse);
			m_pMouse = NULL;
		}
		OIS::InputManager::destroyInputSystem(m_pInputManager);
		m_pInputManager = NULL;
	}
}//Game::shutdownOIS
