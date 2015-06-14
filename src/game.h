#ifndef GAME_H
#define GAME_H

#include <OGRE/OgreRoot.h>
#include "OGRE/OgreRenderSystem.h"
#include "OGRE/OgreRenderWindow.h"
#include "OGRE/OgreWindowEventUtilities.h"
#include "OGRE/OgreEntity.h"
#include "OGRE/OgreMaterialManager.h"
#include "OGRE/OgreMeshManager.h"

#include "CEGUI/CEGUI.h"
#include "CEGUI/RendererModules/Ogre/Renderer.h"

#include "OIS/OIS.h"

#include <btBulletDynamicsCommon.h>

class Game: public OIS::KeyListener, public OIS::MouseListener, public Ogre::FrameListener
{
	bool									m_running;

	//Ogre graphics system
	Ogre::Root*								m_pRoot;
	Ogre::RenderWindow*						m_pWindow;
	Ogre::SceneManager*						m_pScene;
	Ogre::SceneNode*						m_pRootSceneNode;
	Ogre::SceneNode*						m_pCameraNode;
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

	//bullet physics system
	btBroadphaseInterface*					m_pBroadphase;
	btDefaultCollisionConfiguration*		m_pCollisionConfiguration;
	btCollisionDispatcher*					m_pDispatcher;
	btSequentialImpulseConstraintSolver*	m_pSolver;
	btDiscreteDynamicsWorld*				m_pDynamicsWorld;
	btCollisionShape*						m_pGroundShape;
	btDefaultMotionState*					m_pGroundMotionState;
	btRigidBody*							m_pGroundBody;
	btCollisionShape*						m_pPlayerShape;
	btDefaultMotionState*					m_pPlayerMotionState;
	btRigidBody*							m_pPlayerBody;
	std::map<std::string, btRigidBody*>		physicsAccessors;

	//Game system
	Ogre::SceneNode*						m_pPlayerSceneNode;
	Ogre::Vector3							m_PlayerPosition;
	Ogre::Vector3							m_PlayerVelocity;
	Ogre::Vector3							m_PlayerOrientation;

public:
	Game();
	~Game();

	bool initialize();

	bool initializeOgre();

	bool frameRenderingQueued( const Ogre::FrameEvent& evt );

	bool initializeCEGUI();

	bool initializeOIS();
	void OISSetWindowSize();

	bool keyPressed( const OIS::KeyEvent &e );
	bool keyReleased( const OIS::KeyEvent &e );
	bool mouseMoved( const OIS::MouseEvent &e );
	bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );

	bool initializeBullet();

	bool run();
	bool update( float deltaSec );

	bool shutdown();
	void shutdownOgre();
	void shutdownCEGUI();
	void shutdownOIS();
	void shutdownBullet();
};//Game

#endif // GAME_H
