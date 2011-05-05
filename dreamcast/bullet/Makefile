#
# CubicVR Bullet Makefile http://www.cubicvr.org/
#   

#TARGET = cubicvr.elf

BULLET_PREFIX=../../cubicvr/external/bullet-2.78/
INCDIR = $(BULLET_PREFIX)src/

DYNAMICS_OBJS = btSimpleDynamicsWorld.o btContinuousDynamicsWorld.o btDiscreteDynamicsWorld.o btRigidBody.o btSimpleDynamicsWorld.o btConeTwistConstraint.o btContactConstraint.o btGeneric6DofConstraint.o btHinge2Constraint.o btHingeConstraint.o btPoint2PointConstraint.o btSequentialImpulseConstraintSolver.o btSliderConstraint.o btSolve2LinearConstraint.o btTypedConstraint.o btUniversalConstraint.o btRaycastVehicle.o btWheelInfo.o btKinematicCharacterController.o



#btOdeContactJoint.o btOdeJoint.o btOdeQuickstepConstraintSolver.o btOdeTypedJoint.o btSorLcp.o 

COLLISION_OBJS = btAxisSweep3.o btBroadphaseProxy.o btCollisionAlgorithm.o btDbvt.o btDbvtBroadphase.o btDispatcher.o btMultiSapBroadphase.o btOverlappingPairCache.o btQuantizedBvh.o btSimpleBroadphase.o btBoxBoxCollisionAlgorithm.o btBoxBoxDetector.o btCollisionDispatcher.o btCollisionObject.o btCollisionWorld.o btCompoundCollisionAlgorithm.o btConvexConcaveCollisionAlgorithm.o btConvexConvexAlgorithm.o btConvexPlaneCollisionAlgorithm.o btDefaultCollisionConfiguration.o btEmptyCollisionAlgorithm.o btManifoldResult.o btSimulationIslandManager.o btSphereBoxCollisionAlgorithm.o btSphereSphereCollisionAlgorithm.o btSphereTriangleCollisionAlgorithm.o btUnionFind.o SphereTriangleDetector.o btBoxShape.o btBvhTriangleMeshShape.o btCapsuleShape.o btCollisionShape.o btCompoundShape.o btConcaveShape.o btConeShape.o btConvexHullShape.o btConvexInternalShape.o btConvexShape.o btConvexTriangleMeshShape.o btCylinderShape.o btEmptyShape.o btHeightfieldTerrainShape.o btMinkowskiSumShape.o btMultiSphereShape.o btOptimizedBvh.o btPolyhedralConvexShape.o btShapeHull.o btSphereShape.o btStaticPlaneShape.o btStridingMeshInterface.o btTetrahedronShape.o btTriangleBuffer.o btTriangleCallback.o btTriangleIndexVertexArray.o btTriangleMesh.o btTriangleMeshShape.o btUniformScalingShape.o btContinuousConvexCollision.o btConvexCast.o btGjkConvexCast.o btGjkEpa2.o btGjkEpaPenetrationDepthSolver.o btGjkPairDetector.o btMinkowskiPenetrationDepthSolver.o btPersistentManifold.o btRaycastCallback.o btSubSimplexConvexCast.o btVoronoiSimplexSolver.o btMultimaterialTriangleMeshShape.o btTriangleIndexVertexMaterialArray.o btActivatingCollisionAlgorithm.o btBox2dBox2dCollisionAlgorithm.o btConvex2dConvex2dAlgorithm.o btGhostObject.o btInternalEdgeUtility.o btBox2dShape.o btConvexPointCloudShape.o btPolyhedralContactClipping.o


#btConvexHullComputer.o 

MATH_OBJS=btAlignedAllocator.o btConvexHull.o btGeometryUtil.o btQuickprof.o btSerializer.o

SOFTBODY_OBJS=btDefaultSoftBodySolver.o btSoftBody.o btSoftBodyConcaveCollisionAlgorithm.o btSoftBodyHelpers.o btSoftBodyRigidBodyCollisionConfiguration.o btSoftRigidCollisionAlgorithm.o btSoftRigidDynamicsWorld.o btSoftRigidDynamicsWorld.o btSoftSoftCollisionAlgorithm.o

all: bulletmath bulletdynamics bulletcollision bulletsoftbody
	mkdir ../build || true
	mkdir ../build/lib || true
	mkdir ../build/include || true
	cp *.a ../build/lib/
	cp -R $(INCDIR)* ../build/include/
	rm `find ../build/include/ | grep "\.cpp"`

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS) $(DYNAMICS_OBJS) $(COLLISION_OBJS) $(MATH_OBJS) $(SOFTBODY_OBJS) *.a romdisk.*

rm-elf:
	-rm -f $(TARGET) romdisk.*

#$(TARGET): $(OBJS) romdisk.o
#	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
#		$(OBJS) romdisk.o $(OBJEXTRA) -L$(KOS_BASE)/lib -lkosutils 


BUILDCMD=$(KOS_CC) $(KOS_CFLAGS) -funroll-loops -O3 $(KOS_LDFLAGS) $(OBJEXTRA) -L$(KOS_BASE)/lib -I$(INCDIR) -I./ -c -o 


# DYNAMICS LIB

btSimpleDynamicsWorld.o: $(BULLET_PREFIX)src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp
	$(BUILDCMD) btSimpleDynamicsWorld.o $^
btContinuousDynamicsWorld.o: $(BULLET_PREFIX)src/BulletDynamics/Dynamics/btContinuousDynamicsWorld.cpp
	$(BUILDCMD) btContinuousDynamicsWorld.o $^
btDiscreteDynamicsWorld.o: $(BULLET_PREFIX)src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp
	$(BUILDCMD) btDiscreteDynamicsWorld.o $^
btRigidBody.o: $(BULLET_PREFIX)src/BulletDynamics/Dynamics/btRigidBody.cpp
	$(BUILDCMD) btRigidBody.o $^


btConeTwistConstraint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp
	$(BUILDCMD) btConeTwistConstraint.o $^
btContactConstraint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp
	$(BUILDCMD) btContactConstraint.o $^
btGeneric6DofConstraint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp
	$(BUILDCMD) btGeneric6DofConstraint.o $^
btHinge2Constraint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp
	$(BUILDCMD) btHinge2Constraint.o $^
btHingeConstraint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp
	$(BUILDCMD) btHingeConstraint.o $^
#btOdeContactJoint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btOdeContactJoint.cpp
#	$(BUILDCMD) btOdeContactJoint.o $^
#btOdeJoint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btOdeJoint.cpp
#	$(BUILDCMD) btOdeJoint.o $^
#btOdeQuickstepConstraintSolver.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btOdeQuickstepConstraintSolver.cpp
#	$(BUILDCMD) btOdeQuickstepConstraintSolver.o $^
#btOdeTypedJoint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btOdeTypedJoint.cpp
#	$(BUILDCMD) btOdeTypedJoint.o $^
btPoint2PointConstraint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp
	$(BUILDCMD) btPoint2PointConstraint.o $^
btSequentialImpulseConstraintSolver.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp
	$(BUILDCMD) btSequentialImpulseConstraintSolver.o $^
btSliderConstraint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp
	$(BUILDCMD) btSliderConstraint.o $^
btSolve2LinearConstraint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp
	$(BUILDCMD) btSolve2LinearConstraint.o $^
#btSorLcp.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btSorLcp.cpp
#	$(BUILDCMD) btSorLcp.o $^
btTypedConstraint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp
	$(BUILDCMD) btTypedConstraint.o $^
btUniversalConstraint.o: $(BULLET_PREFIX)src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp
	$(BUILDCMD) btUniversalConstraint.o $^


btRaycastVehicle.o: $(BULLET_PREFIX)src/BulletDynamics/Vehicle/btRaycastVehicle.cpp
	$(BUILDCMD) btRaycastVehicle.o $^
btWheelInfo.o: $(BULLET_PREFIX)src/BulletDynamics/Vehicle/btWheelInfo.cpp
	$(BUILDCMD) btWheelInfo.o $^

btKinematicCharacterController.o: $(BULLET_PREFIX)src/BulletDynamics/Character/btKinematicCharacterController.cpp
	$(BUILDCMD) btKinematicCharacterController.o $^


	
	
# COLLISION LIB

## broadphase
	
btAxisSweep3.o: $(BULLET_PREFIX)src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp
	$(BUILDCMD) btAxisSweep3.o $^
btBroadphaseProxy.o: $(BULLET_PREFIX)src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp
	$(BUILDCMD) btBroadphaseProxy.o $^
btCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp
	$(BUILDCMD) btCollisionAlgorithm.o $^
btDbvt.o: $(BULLET_PREFIX)src/BulletCollision/BroadphaseCollision/btDbvt.cpp
	$(BUILDCMD) btDbvt.o $^
btDbvtBroadphase.o: $(BULLET_PREFIX)src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp
	$(BUILDCMD) btDbvtBroadphase.o $^
btDispatcher.o: $(BULLET_PREFIX)src/BulletCollision/BroadphaseCollision/btDispatcher.cpp
	$(BUILDCMD) btDispatcher.o $^
btMultiSapBroadphase.o: $(BULLET_PREFIX)src/BulletCollision/BroadphaseCollision/btMultiSapBroadphase.cpp
	$(BUILDCMD) btMultiSapBroadphase.o $^
btOverlappingPairCache.o: $(BULLET_PREFIX)src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp
	$(BUILDCMD) btOverlappingPairCache.o $^
btQuantizedBvh.o: $(BULLET_PREFIX)src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp
	$(BUILDCMD) btQuantizedBvh.o $^
btSimpleBroadphase.o: $(BULLET_PREFIX)src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp
	$(BUILDCMD) btSimpleBroadphase.o $^
	
## collision dispatch	
	
btActivatingCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp
	$(BUILDCMD) btActivatingCollisionAlgorithm.o $^
btBoxBoxCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp
	$(BUILDCMD) btBoxBoxCollisionAlgorithm.o $^
btBox2dBox2dCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp
	$(BUILDCMD) btBox2dBox2dCollisionAlgorithm.o $^
btBoxBoxDetector.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp
	$(BUILDCMD) btBoxBoxDetector.o $^
btCollisionDispatcher.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp
	$(BUILDCMD) btCollisionDispatcher.o $^
btCollisionObject.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btCollisionObject.cpp
	$(BUILDCMD) btCollisionObject.o $^
btCollisionWorld.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp
	$(BUILDCMD) btCollisionWorld.o $^
btCompoundCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp
	$(BUILDCMD) btCompoundCollisionAlgorithm.o $^
btConvex2dConvex2dAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp
	$(BUILDCMD) btConvex2dConvex2dAlgorithm.o $^
btConvexConcaveCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp
	$(BUILDCMD) btConvexConcaveCollisionAlgorithm.o $^
btConvexConvexAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp
	$(BUILDCMD) btConvexConvexAlgorithm.o $^
btConvexPlaneCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp
	$(BUILDCMD) btConvexPlaneCollisionAlgorithm.o $^
btDefaultCollisionConfiguration.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp
	$(BUILDCMD) btDefaultCollisionConfiguration.o $^
btEmptyCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp
	$(BUILDCMD) btEmptyCollisionAlgorithm.o $^
btGhostObject.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btGhostObject.cpp
	$(BUILDCMD) btGhostObject.o $^
btInternalEdgeUtility.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp
	$(BUILDCMD) btInternalEdgeUtility.o $^
btManifoldResult.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btManifoldResult.cpp
	$(BUILDCMD) btManifoldResult.o $^
btSimulationIslandManager.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp
	$(BUILDCMD) btSimulationIslandManager.o $^
btSphereBoxCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp
	$(BUILDCMD) btSphereBoxCollisionAlgorithm.o $^
btSphereSphereCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp
	$(BUILDCMD) btSphereSphereCollisionAlgorithm.o $^
btSphereTriangleCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp
	$(BUILDCMD) btSphereTriangleCollisionAlgorithm.o $^
btUnionFind.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/btUnionFind.cpp
	$(BUILDCMD) btUnionFind.o $^
SphereTriangleDetector.o: $(BULLET_PREFIX)src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp
	$(BUILDCMD) SphereTriangleDetector.o $^
	
## shapes 
	
btBox2dShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btBox2dShape.cpp
	$(BUILDCMD) btBox2dShape.o $^
btBoxShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btBoxShape.cpp
	$(BUILDCMD) btBoxShape.o $^
btBvhTriangleMeshShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp
	$(BUILDCMD) btBvhTriangleMeshShape.o $^
btCapsuleShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btCapsuleShape.cpp
	$(BUILDCMD) btCapsuleShape.o $^
btCollisionShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btCollisionShape.cpp
	$(BUILDCMD) btCollisionShape.o $^
btCompoundShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btCompoundShape.cpp
	$(BUILDCMD) btCompoundShape.o $^
btConcaveShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btConcaveShape.cpp
	$(BUILDCMD) btConcaveShape.o $^
btConeShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btConeShape.cpp
	$(BUILDCMD) btConeShape.o $^
btConvexHullShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btConvexHullShape.cpp
	$(BUILDCMD) btConvexHullShape.o $^
btConvexInternalShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp
	$(BUILDCMD) btConvexInternalShape.o $^
btConvexPointCloudShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp
	$(BUILDCMD) btConvexPointCloudShape.o $^
btConvexPolyhedron.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp
	$(BUILDCMD) btConvexPolyhedron.o $^
btConvexShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btConvexShape.cpp
	$(BUILDCMD) btConvexShape.o $^
btConvexTriangleMeshShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp
	$(BUILDCMD) btConvexTriangleMeshShape.o $^
btCylinderShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btCylinderShape.cpp
	$(BUILDCMD) btCylinderShape.o $^
btEmptyShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btEmptyShape.cpp
	$(BUILDCMD) btEmptyShape.o $^
btHeightfieldTerrainShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp
	$(BUILDCMD) btHeightfieldTerrainShape.o $^
btMinkowskiSumShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp
	$(BUILDCMD) btMinkowskiSumShape.o $^
btMultimaterialTriangleMeshShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp
	$(BUILDCMD) btMultimaterialTriangleMeshShape.o $^
btMultiSphereShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp
	$(BUILDCMD) btMultiSphereShape.o $^
btOptimizedBvh.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp
	$(BUILDCMD) btOptimizedBvh.o $^
btPolyhedralConvexShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp
	$(BUILDCMD) btPolyhedralConvexShape.o $^
btScaledBvhTriangleMeshShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp
	$(BUILDCMD) btScaledBvhTriangleMeshShape.o $^
btShapeHull.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btShapeHull.cpp
	$(BUILDCMD) btShapeHull.o $^
btSphereShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btSphereShape.cpp
	$(BUILDCMD) btSphereShape.o $^
btStaticPlaneShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp
	$(BUILDCMD) btStaticPlaneShape.o $^
btStridingMeshInterface.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp
	$(BUILDCMD) btStridingMeshInterface.o $^
btTetrahedronShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp
	$(BUILDCMD) btTetrahedronShape.o $^
btTriangleBuffer.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp
	$(BUILDCMD) btTriangleBuffer.o $^
btTriangleCallback.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btTriangleCallback.cpp
	$(BUILDCMD) btTriangleCallback.o $^
btTriangleIndexVertexArray.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp
	$(BUILDCMD) btTriangleIndexVertexArray.o $^
btTriangleIndexVertexMaterialArray.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp
	$(BUILDCMD) btTriangleIndexVertexMaterialArray.o $^
btTriangleMesh.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btTriangleMesh.cpp
	$(BUILDCMD) btTriangleMesh.o $^
btTriangleMeshShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp
	$(BUILDCMD) btTriangleMeshShape.o $^
btUniformScalingShape.o: $(BULLET_PREFIX)src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp
	$(BUILDCMD) btUniformScalingShape.o $^
	
## narrow phase	
	
btContinuousConvexCollision.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp
	$(BUILDCMD) btContinuousConvexCollision.o $^
btConvexCast.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp
	$(BUILDCMD) btConvexCast.o $^
btGjkConvexCast.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp
	$(BUILDCMD) btGjkConvexCast.o $^
#btGjkEpa.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btGjkEpa.cpp
#	$(BUILDCMD) btGjkEpa.o $^
btGjkEpa2.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp
	$(BUILDCMD) btGjkEpa2.o $^
btGjkEpaPenetrationDepthSolver.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp
	$(BUILDCMD) btGjkEpaPenetrationDepthSolver.o $^
btGjkPairDetector.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp
	$(BUILDCMD) btGjkPairDetector.o $^
btMinkowskiPenetrationDepthSolver.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp
	$(BUILDCMD) btMinkowskiPenetrationDepthSolver.o $^
btPersistentManifold.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp
	$(BUILDCMD) btPersistentManifold.o $^
btPolyhedralContactClipping.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp
	$(BUILDCMD) btPolyhedralContactClipping.o $^
btRaycastCallback.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp
	$(BUILDCMD) btRaycastCallback.o $^
btSubSimplexConvexCast.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp
	$(BUILDCMD) btSubSimplexConvexCast.o $^
btVoronoiSimplexSolver.o: $(BULLET_PREFIX)src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp
	$(BUILDCMD) btVoronoiSimplexSolver.o $^


# math 
btAlignedAllocator.o: $(BULLET_PREFIX)src/LinearMath/btAlignedAllocator.cpp
	$(BUILDCMD) btAlignedAllocator.o $^
btConvexHull.o: $(BULLET_PREFIX)src/LinearMath/btConvexHull.cpp
	$(BUILDCMD) btConvexHull.o $^
btConvexHullComputer.o: $(BULLET_PREFIX)src/LinearMath/btConvexHullComputer.cpp
	$(BUILDCMD) btConvexHullComputer.o $^
btGeometryUtil.o: $(BULLET_PREFIX)src/LinearMath/btGeometryUtil.cpp
	$(BUILDCMD) btGeometryUtil.o $^
btQuickprof.o: $(BULLET_PREFIX)src/LinearMath/btQuickprof.cpp
	$(BUILDCMD) btQuickprof.o $^
btSerializer.o: $(BULLET_PREFIX)src/LinearMath/btSerializer.cpp
	$(BUILDCMD) btSerializer.o $^


# soft body 

btDefaultSoftBodySolver.o: $(BULLET_PREFIX)src/BulletSoftBody/btDefaultSoftBodySolver.cpp
	$(BUILDCMD) btDefaultSoftBodySolver.o $^
	
btSoftBody.o: $(BULLET_PREFIX)src/BulletSoftBody/btSoftBody.cpp
	$(BUILDCMD) btSoftBody.o $^

btSoftBodyConcaveCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp
	$(BUILDCMD) btSoftBodyConcaveCollisionAlgorithm.o $^

btSoftBodyHelpers.o: $(BULLET_PREFIX)src/BulletSoftBody/btSoftBodyHelpers.cpp
	$(BUILDCMD) btSoftBodyHelpers.o $^

btSoftBodyRigidBodyCollisionConfiguration.o: $(BULLET_PREFIX)src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp 
	$(BUILDCMD) btSoftBodyRigidBodyCollisionConfiguration.o $^

btSoftRigidCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp
	$(BUILDCMD) btSoftRigidCollisionAlgorithm.o $^

btSoftRigidDynamicsWorld.o: $(BULLET_PREFIX)src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp
	$(BUILDCMD) btSoftRigidDynamicsWorld.o $^

btSoftSoftCollisionAlgorithm.o: $(BULLET_PREFIX)src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp
	$(BUILDCMD) btSoftSoftCollisionAlgorithm.o $^


bulletdynamics: libbulletdynamics.a

bulletcollision: libbulletcollision.a

bulletmath: libbulletmath.a

bulletsoftbody: libbulletsoftbody.a

libbulletsoftbody.a: $(SOFTBODY_OBJS)
	$(KOS_AR) cru $@ $^
	$(KOS_RANLIB) $@

libbulletcollision.a: $(COLLISION_OBJS)
	$(KOS_AR) cru $@ $^
	$(KOS_RANLIB) $@

libbulletdynamics.a: $(DYNAMICS_OBJS)
	$(KOS_AR) cru $@ $^
	$(KOS_RANLIB) $@

libbulletmath.a: $(MATH_OBJS)
	$(KOS_AR) cru $@ $^
	$(KOS_RANLIB) $@

romdisk.img:
	$(KOS_GENROMFS) -f romdisk.img -d romdisk -v

romdisk.o: romdisk.img
	$(KOS_BASE)/utils/bin2o/bin2o romdisk.img romdisk romdisk.o

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS) romdisk.o romdisk.img
	$(KOS_STRIP) $(TARGET)

