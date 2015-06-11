#ifndef GAME_H
#define GAME_H

#include <memory>

#include <OGRE/OgreRoot.h>
#include "OGRE/OgreRenderSystem.h"
#include "OGRE/OgreRenderWindow.h"
#include "OGRE/OgreWindowEventUtilities.h"
#include "OGRE/OgreEntity.h"
#include "OGRE/OgreMaterialManager.h"

#include "CEGUI/CEGUI.h"
#include "CEGUI/RendererModules/Ogre/Renderer.h"

#include "OIS/OIS.h"

class Game: public OIS::KeyListener, public OIS::MouseListener
{
	bool									m_running;

	//Ogre graphics system
	std::shared_ptr<Ogre::Root>				m_pRoot;
	Ogre::RenderWindow*						m_pWindow;
	std::shared_ptr<Ogre::SceneManager>		m_pScene;
	std::shared_ptr<Ogre::SceneNode>		m_pRootSceneNode;
	std::shared_ptr<Ogre::SceneNode>		m_pCameraNode;
	unsigned int							m_WindowWidth;
	unsigned int							m_WindowHeight;
	bool									m_Fullscreen;

    //CEGUI gui system
	bool									m_isMouseInWindow;

	//OIS input system
	OIS::InputManager*						m_pInputManager;
	OIS::Keyboard*							m_pKeyboard;
	OIS::Mouse*								m_pMouse;
	bool									m_BufferedKeys;
	bool									m_BufferedMouse;

public:
	Game();
	~Game();

	bool initialize();
	bool initializeOgre();
	bool initializeCEGUI();
	bool initializeOIS();
	void OISSetWindowSize();

	bool keyPressed( const OIS::KeyEvent &e );
	bool keyReleased( const OIS::KeyEvent &e );
	bool mouseMoved( const OIS::MouseEvent &e );
	bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );

	bool run();

	bool shutdown();
	void shutdownOgre();
	void shutdownCEGUI();
	void shutdownOIS();
};//Game

#endif // GAME_H
