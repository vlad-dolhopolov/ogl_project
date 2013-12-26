#ifndef GAME_H_
#define GAME_H_

#include "GLSH.h"

class Game : public glsh::App {
public:
                            Game();
                            ~Game();

    void                    initialize(int w, int h)    override;
    void                    shutdown()                  override;
    void                    resize(int w, int h)        override;
    void                    draw()                      override;
    bool                    update(float dt)            override;
};

#endif
