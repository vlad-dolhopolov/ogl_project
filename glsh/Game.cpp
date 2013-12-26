#include "Game.h"

Game::Game() 
{
    // No OpenGL calls here.
    //
    // The window and rendering context don't exist yet.
    //
    // Just intialize all member variables to some reasonable defaults
    // or known invalid values like NULL or 0 or -1.
    //
    // Do all graphics initialization in the initialize() method instead.
}

Game::~Game()
{
    // No OpenGL calls here.
    //
    // The window and rendering context don't exist anymore.
    //
    // Release all OpenGL resources in the shutdown() method instead.
}

void Game::initialize(int w, int h)
{
    // set screen clearing color
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

    //
    // ...
    //
    // This method gets called once right after the window is created.
    // The arguments are the dimensions of the screen.
    //
    // You should use this method for most of the initialization,
    // especially loading and generating OpenGL resources.
    //
    // Load meshes, build shaders, create entities, enable or disable
    // rendering features, etc.
    //
    // ...
    //

}

void Game::shutdown()
{
    // Delete entities, meshes, shaders, and other resources.
    //
    // This method gets called once just before the window is destroyed.
}

void Game::resize(int w, int h)
{
    // set viewport (subrect of screen to draw on)
    glViewport(0, 0, w, h);

    //
    // ...
    //
    // This method is called every time the window is resized
    // and also once after the window is created.
    //
    // You can use the new screen dimensions and aspect ratio
    // to compute a suitable projection matrix here.
    //
    // ...
    //
}

void Game::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    //
    // ...
    //
    // Draw the current snapshot of everything in the scene.
    //
    // This method gets called once per frame.
    //
    // ...
    //
}

bool Game::update(float dt)
{
    const glsh::Keyboard* kb = getKeyboard();

    if (kb->keyPressed(glsh::KC_ESCAPE)) {
        return false; // request to exit
    }

    //
    // ...
    //
    // Handle player input to enable interactions.
    //
    // Update dynamic elements in the scene.
    // Transform, animate, create, or destroy scene elements.
    //
    // This method gets called once per frame.
    // The argument is the time elapsed since last frame (in seconds).
    //
    // ...
    //

    return true; // request to keep going
}
