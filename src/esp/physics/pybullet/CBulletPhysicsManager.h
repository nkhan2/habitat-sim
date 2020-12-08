// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef ESP_PHYSICS_PYBULLET_CBULLETPHYSICSMANAGER_H_
#define ESP_PHYSICS_PYBULLET_CBULLETPHYSICSMANAGER_H_

/** @file
 * @brief Class @ref esp::physics::CBulletPhysicsManager
 */

/* Bullet Physics Integration */
#include "esp/physics/PhysicsManager.h"

// TODO: import BulletRobotics. Includes are not found currently. Added by
// deps/bullet3/Extras/BulletRobotics/CMakeLists.txt. Included with
// BUILD_EXTRAS?
//#include <bullet_robotics/PhysicsClientC_API.h>
//#include "bullet_robotics/PhysicsClientC_API.h"

namespace esp {
namespace physics {

/**
@brief Dynamic stage and object manager interfacing with Bullet physics C-Bullet
API: https://github.com/bulletphysics/bullet3.

This class handles initialization and stepping of the world as well as getting
and setting global simulation parameters.
*/
class CBulletPhysicsManager : public PhysicsManager {
 public:
  /**
   * @brief Construct a @ref CBulletPhysicsManager with access to specific
   * resourse assets.
   *
   * @param _resourceManager The @ref esp::assets::ResourceManager which
   * tracks the assets this
   * @ref BulletPhysicsManager will have access to.
   */
  explicit CBulletPhysicsManager(
      assets::ResourceManager& _resourceManager,
      const metadata::attributes::PhysicsManagerAttributes::cptr
          _physicsManagerAttributes)
      : PhysicsManager(_resourceManager, _physicsManagerAttributes){};

  /** @brief Destructor which destructs necessary Bullet physics structures.*/
  virtual ~CBulletPhysicsManager();

  //============ Simulator functions =============

  /** @brief Step the physical world forward in time. Time may only advance in
   * increments of @ref fixedTimeStep_. See @ref
   * btMultiBodyDynamicsWorld::stepSimulation.
   * @param dt The desired amount of time to advance the physical world.
   */
  void stepPhysics(double dt) override;

  /** @brief Set the gravity of the physical world.
   * @param gravity The desired gravity force of the physical world.
   */
  void setGravity(const Magnum::Vector3& gravity) override;

  /** @brief Get the current gravity in the physical world.
   * @return The current gravity vector in the physical world.
   */
  Magnum::Vector3 getGravity() const override;

  //============ Interacting with objects =============
  // NOTE: engine specifics for interaction are handled by the objects
  // themselves...

  //============ Bullet-specific Object Setter functions =============

  /** @brief Set the scalar collision margin of an object.
   * See @ref BulletRigidObject::setMargin.
   * @param  physObjectID The object ID and key identifying the object in @ref
   * PhysicsManager::existingObjects_.
   * @param  margin The desired collision margin for the object.
   */
  void setMargin(const int physObjectID, const double margin) override;

  /** @brief Set the friction coefficient of the stage collision geometry. See
   * @ref staticStageObject_. See @ref
   * BulletRigidObject::setFrictionCoefficient.
   * @param frictionCoefficient The scalar friction coefficient of the stage
   * geometry.
   */
  void setStageFrictionCoefficient(const double frictionCoefficient) override;

  /** @brief Set the coefficient of restitution for the stage collision
   * geometry. See @ref staticStageObject_. See @ref
   * BulletRigidObject::setRestitutionCoefficient.
   * @param restitutionCoefficient The scalar coefficient of restitution to set.
   */
  void setStageRestitutionCoefficient(
      const double restitutionCoefficient) override;

  //============ Bullet-specific Object Getter functions =============

  /** @brief Get the scalar collision margin of an object.
   * See @ref BulletRigidObject::getMargin.
   * @param  physObjectID The object ID and key identifying the object in @ref
   * PhysicsManager::existingObjects_.
   * @return The scalar collision margin of the object or @ref
   * esp::PHYSICS_ATTR_UNDEFINED if failed..
   */
  double getMargin(const int physObjectID) const override;

  /** @brief Get the current friction coefficient of the stage collision
   * geometry. See @ref staticStageObject_ and @ref
   * BulletRigidObject::getFrictionCoefficient.
   * @return The scalar friction coefficient of the stage geometry.
   */
  double getStageFrictionCoefficient() const override;

  /** @brief Get the current coefficient of restitution for the stage
   * collision geometry. This determines the ratio of initial to final relative
   * velocity between the stage and collidiing object. See @ref
   * staticStageObject_ and BulletRigidObject::getRestitutionCoefficient.
   * @return The scalar coefficient of restitution for the stage geometry.
   */
  double getStageRestitutionCoefficient() const override;

  /**
   * @brief Query the Aabb from bullet physics for the root compound shape of a
   * rigid body in its local space. See @ref btCompoundShape::getAabb.
   * @param physObjectID The object ID and key identifying the object in @ref
   * PhysicsManager::existingObjects_.
   * @return The Aabb.
   */
  const Magnum::Range3D getCollisionShapeAabb(const int physObjectID) const;

  /**
   * @brief Query the Aabb from bullet physics for the root compound shape of
   * the static stage in its local space. See @ref btCompoundShape::getAabb.
   * @return The stage collision Aabb.
   */
  const Magnum::Range3D getStageCollisionShapeAabb() const;

  /** @brief Render the debugging visualizations provided by @ref
   * Magnum::BulletIntegration::DebugDraw. This draws wireframes for all
   * collision objects.
   * @param projTrans The composed projection and transformation matrix for the
   * render camera.
   */
  virtual void debugDraw(const Magnum::Matrix4& projTrans) const override;

  /**
   * @brief Check whether an object is in contact with any other objects or the
   * stage.
   *
   * @param physObjectID The object ID and key identifying the object in @ref
   * PhysicsManager::existingObjects_.
   * @return Whether or not the object is in contact with any other collision
   * enabled objects.
   */
  bool contactTest(const int physObjectID) override;

  /**
   * @brief Cast a ray into the collision world and return a @ref RaycastResults
   * with hit information.
   *
   * @param ray The ray to cast. Need not be unit length, but returned hit
   * distances will be in units of ray length.
   * @param maxDistance The maximum distance along the ray direction to search.
   * In units of ray length.
   * @return The raycast results sorted by distance.
   */
  virtual RaycastResults castRay(const esp::geo::Ray& ray,
                                 double maxDistance = 100.0) override;

  // The number of contact points that were active during the last step. An
  // object resting on another object will involve several active contact
  // points. Once both objects are asleep, the contact points are inactive. This
  // count can be used as a metric for the complexity/cost of collision-handling
  // in the current scene.
  int getNumActiveContactPoints() override;

 protected:
  //============ Initialization =============
  /**
   * @brief Finalize physics initialization: Setup staticStageObject_ and
   * initialize any other physics-related values.
   */
  bool initPhysicsFinalize() override;

  //============ Object/Stage Instantiation =============
  /**
   * @brief Finalize stage initialization. Checks that the collision
   * mesh can be used by Bullet. See @ref BulletRigidObject::initializeStage.
   * Bullet mesh conversion adapted from:
   * https://github.com/mosra/magnum-integration/issues/20
   * @param handle The handle of the attributes structure defining physical
   * properties of the stage.
   * @return true if successful and false otherwise
   */
  bool addStageFinalize(const std::string& handle) override;

  /** @brief Create and initialize an @ref RigidObject and add
   * it to existingObjects_ map keyed with newObjectID
   * @param newObjectID valid object ID for the new object
   * @param meshGroup The object's mesh.
   * @param handle The handle to the physical object's template defining its
   * physical parameters.
   * @param objectNode Valid, existing scene node
   * @return whether the object has been successfully initialized and added to
   * existingObjects_ map
   */
  bool makeAndAddRigidObject(int newObjectID,
                             const std::string& handle,
                             scene::SceneNode* objectNode) override;

 private:
  /** @brief Check if a particular mesh can be used as a collision mesh for
   * Bullet.
   * @param meshData The mesh to validate. Only a triangle mesh is valid. Checks
   * that the only #ref Magnum::MeshPrimitive are @ref
   * Magnum::MeshPrimitive::Triangles.
   * @return true if valid, false otherwise.
   */
  bool isMeshPrimitiveValid(const assets::CollisionMeshData& meshData) override;

  ESP_SMART_POINTERS(CBulletPhysicsManager)

};  // end class BulletPhysicsManager
}  // end namespace physics
}  // end namespace esp

#endif  // ESP_PHYSICS_BULLET_BULLETPHYSICSMANAGER_H_