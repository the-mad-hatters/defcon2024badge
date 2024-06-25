#ifndef SCENE_BASE_H
#define SCENE_BASE_H

/**
 * @brief Scene base class
 *     All LED animation scenes must inherit from this class
 */
class SceneBase {
  public:
    virtual void tick() = 0;
};

#endif // SCENE_BASE_H